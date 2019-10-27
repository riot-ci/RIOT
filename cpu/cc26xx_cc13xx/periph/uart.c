/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26xx_cc13xx
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Leon M. George <leon@georgemail.eu>
 * @author      Anton Gerasimov <tossel@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph/uart.h"
#include "periph_conf.h"

/**
 * @brief   Bit mask for the fractional part of the baudrate
 */
#define FRAC_BITS           (6U)
#define FRAC_MASK           (0x3f)

/**
 * @brief   Get the enable mask depending on enabled HW flow control
 */
#if UART_HW_FLOW_CONTROL
#define ENABLE_MASK         (UART_CTSEN | UART_CTL_RTSEN | \
                             UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN)
#else
#define ENABLE_MASK         (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN)
#endif

/**
 * @brief allocate memory to store callback functions
 */
static uart_isr_ctx_t ctx[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);

    uart_regs_t *uart_reg = uart_config[uart].regs;
    int tx_pin = uart_config[uart].tx_pin;
    int rx_pin = uart_config[uart].rx_pin;
    int intn = uart_config[uart].intn;
    int flow = uart_config[uart].flow_control;
    int rts_pin = uart_config[uart].rts_pin;
    int cts_pin = uart_config[uart].cts_pin;

    /* enable clocks: serial power domain and UART */
    PRCM->PDCTL0SERIAL = 1;
    while (!(PRCM->PDSTAT0 & PDSTAT0_SERIAL_ON)) ;
    uart_poweron(uart);

    /* disable and reset the UART */
    uart_reg->CTL = 0;

    /* save context */
    ctx[uart].rx_cb = rx_cb;
    ctx[uart].arg = arg;

    /* configure pins */
    IOC->CFG[tx_pin] =  IOCFG_PORTID_UART0_TX;
    IOC->CFG[rx_pin] = (IOCFG_PORTID_UART0_RX | IOCFG_INPUT_ENABLE);
    if (flow == 1) {
      IOC->CFG[rts_pin] =  IOCFG_PORTID_UART0_RTS;
      IOC->CFG[cts_pin] = (IOCFG_PORTID_UART0_CTS | IOCFG_INPUT_ENABLE);
    }

    /* calculate baud-rate */
    uint32_t tmp = (CLOCK_CORECLOCK * 4);
    tmp += (baudrate / 2);
    tmp /= baudrate;
    uart_reg->IBRD = (tmp >> FRAC_BITS);
    uart_reg->FBRD = (tmp & FRAC_MASK);

    /* configure line to 8N1 mode, LRCH must be written after IBRD and FBRD! */
    uart_reg->LCRH = UART_LCRH_WLEN_8;

    /* enable the RX interrupt */
    uart_reg->IMSC = UART_IMSC_RXIM;
    NVIC_EnableIRQ(intn);

    /* start the UART */
    uart_reg->CTL = ENABLE_MASK;

    return UART_OK;
}


#ifdef MODULE_PERIPH_UART_MODECFG
int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    assert(data_bits == UART_DATA_BITS_5 ||
           data_bits == UART_DATA_BITS_6 ||
           data_bits == UART_DATA_BITS_7 ||
           data_bits == UART_DATA_BITS_8);

    assert(parity == UART_PARITY_NONE ||
           parity == UART_PARITY_EVEN ||
           parity == UART_PARITY_ODD ||
           parity == UART_PARITY_MARK ||
           parity == UART_PARITY_SPACE);

    assert(stop_bits == UART_STOP_BITS_1 ||
           stop_bits == UART_STOP_BITS_2);

    assert(uart < UART_NUMOF);

    uart_regs_t *uart_reg = uart_config[uart].regs;

    /* cc26xx/cc13xx does not support mark or space parity */
    if (parity == UART_PARITY_MARK || parity == UART_PARITY_SPACE) {
        return UART_NOMODE;
    }

    /* Disable UART and clear old settings */
    uart_reg->CTL = 0;
    uart_reg->LCRH = 0;

    /* Apply setting and enable UART */
    /* cppcheck-suppress redundantAssignment
     * (reason: disable-enable cycle requires writing zero first) */
    uart_reg->LCRH = data_bits | parity | stop_bits;
    uart_reg->CTL = ENABLE_MASK;

    return UART_OK;
}
#endif

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);

    uart_regs_t *uart_reg = uart_config[uart].regs;

    for (size_t i = 0; i < len; i++) {
        while (uart_reg->FR & UART_FR_TXFF) {}
        uart_reg->DR = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

    uart_regs_t *uart_reg = uart_config[uart].regs;

    PRCM->UARTCLKGR |= 0x1;
    PRCM->CLKLOADCTL = CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) {}

    uart_reg->CTL = ENABLE_MASK;
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

    uart_regs_t *uart_reg = uart_config[uart].regs;

    uart_reg->CTL = 0;

    PRCM->UARTCLKGR = 0;
    PRCM->CLKLOADCTL = CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) {}

}

static void isr_uart(uart_t uart)
{
    assert(uart < UART_NUMOF);

    uart_regs_t *uart_reg = uart_config[uart].regs;

    /* remember pending interrupts */
    uint32_t mis = uart_reg->MIS;
    /* clear them */
    uart_reg->ICR = mis;

    /* read received byte and pass it to the RX callback */
    if (mis & UART_MIS_RXMIS) {
        ctx[uart].rx_cb(ctx[uart].arg, (uint8_t)uart_reg->DR);
    }

    cortexm_isr_end();
}

void isr_uart0(void) {
    isr_uart(0);
}

void isr_uart1(void) {
    isr_uart(1);
}
