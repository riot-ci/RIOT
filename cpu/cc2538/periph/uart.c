/*
 * Copyright (C) 2014 Loci Controls Inc.
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include <stddef.h>

#include "board.h"
#include "cpu.h"
#include "periph/uart.h"
#include "periph_conf.h"

#undef BIT
#define BIT(n) ( 1 << (n) )

enum {
    FIFO_LEVEL_1_8TH = 0,
    FIFO_LEVEL_2_8TH = 1,
    FIFO_LEVEL_4_8TH = 2,
    FIFO_LEVEL_6_8TH = 3,
    FIFO_LEVEL_7_8TH = 4,
};

/* Valid word lengths for the LCRHbits.WLEN bit field: */
enum {
    WLEN_5_BITS = 0,
    WLEN_6_BITS = 1,
    WLEN_7_BITS = 2,
    WLEN_8_BITS = 3,
};

/* Bit field definitions for the UART Line Control Register: */
#define FEN   BIT( 4) /**< Enable FIFOs */

/* Bit masks for the UART Masked Interrupt Status (MIS) Register: */
#define OEMIS BIT(10) /**< UART overrun error masked status */
#define BEMIS BIT( 9) /**< UART break error masked status */
#define FEMIS BIT( 7) /**< UART framing error masked status */
#define RTMIS BIT( 6) /**< UART RX time-out masked status */
#define RXMIS BIT( 4) /**< UART RX masked interrupt status */

#define UART_CTL_HSE_VALUE    0
#define DIVFRAC_NUM_BITS      6
#define DIVFRAC_MASK          ( (1 << DIVFRAC_NUM_BITS) - 1 )

/** @brief Indicates if there are bytes available in the UART0 receive FIFO */
#define uart0_rx_avail() ( uart_config[0].dev->cc2538_uart_fr.FRbits.RXFE == 0 )

/** @brief Indicates if there are bytes available in the UART1 receive FIFO */
#define uart1_rx_avail() ( uart_config[1].dev->cc2538_uart_fr.FRbits.RXFE == 0 )

/** @brief Read one byte from the UART0 receive FIFO */
#define uart0_read()     ( uart_config[0].dev->DR )

/** @brief Read one byte from the UART1 receive FIFO */
#define uart1_read()     ( uart_config[1].dev->DR )

/*---------------------------------------------------------------------------*/

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

/*---------------------------------------------------------------------------*/
static void reset(cc2538_uart_t *u)
{
    /* Make sure the UART is disabled before trying to configure it */
    u->cc2538_uart_ctl.CTLbits.UARTEN = 0;

    u->cc2538_uart_ctl.CTLbits.RXE = 1;
    u->cc2538_uart_ctl.CTLbits.TXE = 1;
    u->cc2538_uart_ctl.CTLbits.HSE = UART_CTL_HSE_VALUE;

    /* Clear error status */
    u->cc2538_uart_dr.ECR = 0xFF;

    /* Flush FIFOs by clearing LCHR.FEN */
    u->cc2538_uart_lcrh.LCRH &= ~FEN;

    /* Restore LCHR configuration */
    u->cc2538_uart_lcrh.LCRH |= FEN;

    /* UART Enable */
    u->cc2538_uart_ctl.CTLbits.UARTEN = 1;
}
/*---------------------------------------------------------------------------*/


static int init_base(uart_t uart, uint32_t baudrate);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* initialize basic functionality */
    int res = init_base(uart, baudrate);
    if (res != UART_OK) {
        return res;
    }

    /* register callbacks */
    uart_ctx[uart].rx_cb = rx_cb;
    uart_ctx[uart].arg = arg;

    /* configure interrupts and enable RX interrupt */
    NVIC_SetPriority(uart_config[uart].irq, UART_IRQ_PRIO);
    NVIC_EnableIRQ(uart_config[uart].irq);

    return UART_OK;
}

static int init_base(uart_t uart, uint32_t baudrate)
{
    cc2538_uart_t *u = NULL;

    u = uart_config[uart].dev;
    gpio_init_af(uart_config[uart].rx_pin, uart_config[uart].rxd, GPIO_IN);
    gpio_init_af(uart_config[uart].tx_pin, uart_config[uart].txd, GPIO_OUT);

    /* Enable clock for the UART while Running, in Sleep and Deep Sleep */
    unsigned int uart_num = ( (uintptr_t)u - (uintptr_t)uart_config[uart].dev ) / 0x1000;
    SYS_CTRL_RCGCUART |= (1 << uart_num);
    SYS_CTRL_SCGCUART |= (1 << uart_num);
    SYS_CTRL_DCGCUART |= (1 << uart_num);

    /* Make sure the UART is disabled before trying to configure it */
    u->cc2538_uart_ctl.CTL = 0;

    /* Run on SYS_DIV */
    u->CC = 0;

    /* On the CC2538, hardware flow control is supported only on UART1 */
    if (uart_config[uart].rts_pin != GPIO_UNDEF){
        gpio_init_af(uart_config[uart].rts_pin, UART1_RTS, GPIO_OUT);
        u->cc2538_uart_ctl.CTLbits.RTSEN = 1;
    }

    if (uart_config[uart].cts_pin != GPIO_UNDEF){
        gpio_init_af(uart_config[uart].cts_pin, UART1_CTS, GPIO_IN);
        u->cc2538_uart_ctl.CTLbits.CTSEN = 1;
    }

    /* Enable clock for the UART while Running, in Sleep and Deep Sleep */
    uart_num = ( (uintptr_t)u - (uintptr_t)uart_config[0].dev ) / 0x1000;
    SYS_CTRL_RCGCUART |= (1 << uart_num);
    SYS_CTRL_SCGCUART |= (1 << uart_num);
    SYS_CTRL_DCGCUART |= (1 << uart_num);

    /*
     * UART Interrupt Masks:
     * Acknowledge RX and RX Timeout
     * Acknowledge Framing, Overrun and Break Errors
     */
    u->cc2538_uart_im.IM = 0;
    u->cc2538_uart_im.IMbits.RXIM = 1; /**< UART receive interrupt mask */
    u->cc2538_uart_im.IMbits.RTIM = 1; /**< UART receive time-out interrupt mask */
    u->cc2538_uart_im.IMbits.OEIM = 1; /**< UART overrun error interrupt mask */
    u->cc2538_uart_im.IMbits.BEIM = 1; /**< UART break error interrupt mask */
    u->cc2538_uart_im.IMbits.FEIM = 1; /**< UART framing error interrupt mask */

    /* Set FIFO interrupt levels: */
    u->cc2538_uart_ifls.IFLSbits.RXIFLSEL = FIFO_LEVEL_4_8TH; /**< MCU default */
    u->cc2538_uart_ifls.IFLSbits.TXIFLSEL = FIFO_LEVEL_4_8TH; /**< MCU default */

    u->cc2538_uart_ctl.CTLbits.RXE = 1;
    u->cc2538_uart_ctl.CTLbits.TXE = 1;
    u->cc2538_uart_ctl.CTLbits.HSE = UART_CTL_HSE_VALUE;

    /* Set the divisor for the baud rate generator */
    uint32_t divisor = sys_clock_freq();
    divisor <<= UART_CTL_HSE_VALUE + 2;
    divisor += baudrate / 2; /**< Avoid a rounding error */
    divisor /= baudrate;
    u->IBRD = divisor >> DIVFRAC_NUM_BITS;
    u->FBRD = divisor & DIVFRAC_MASK;

    /* Configure line control for 8-bit, no parity, 1 stop bit and enable  */
    u->cc2538_uart_lcrh.LCRH = (WLEN_8_BITS << 5) | FEN;

    /* UART Enable */
    u->cc2538_uart_ctl.CTLbits.UARTEN = 1;

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    cc2538_uart_t *u;

    u = uart_config[uart].dev;

    /* Block if the TX FIFO is full */
    for (size_t i = 0; i < len; i++) {
        while (u->cc2538_uart_fr.FRbits.TXFF) {}
        u->DR = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    (void) uart;

}

void uart_poweroff(uart_t uart)
{
    (void) uart;
}

static inline void irq_handler(uart_t uart)
{
    uint_fast16_t mis;

    /* Latch the Masked Interrupt Status and clear any active flags */
    mis = uart_config[uart].dev->cc2538_uart_mis.MIS;
    uart_config[uart].dev->ICR = mis;

    while (uart_config[uart].dev->cc2538_uart_fr.FRbits.RXFE == 0) {
        uart_ctx[uart].rx_cb(uart_ctx[uart].arg, uart_config[uart].dev->DR);
    }

    if (mis & (OEMIS | BEMIS | FEMIS)) {
        /* ISR triggered due to some error condition */
        reset(uart_config[uart].dev);
    }

    cortexm_isr_end();
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler((uart_t)0);
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler((uart_t)1);
}
#endif
