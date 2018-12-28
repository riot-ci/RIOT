/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *               2015 Jan Wagner <mail@jwagner.eu>
 *               2018 Inria
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral UART interface
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "periph/uart.h"
#include "periph/gpio.h"

#ifdef CPU_MODEL_NRF52840XXAA
#define PSEL_RXD         dev(uart)->PSEL.RXD
#define PSEL_TXD         dev(uart)->PSEL.TXD
#define PSEL_RTS         dev(uart)->PSEL.RTS
#define PSEL_CTS         dev(uart)->PSEL.CTS
#define UART_IRQN        uart_config[uart].irqn
#define UART_PIN_RX      uart_config[uart].rx_pin
#define UART_PIN_TX      uart_config[uart].tx_pin
#define UART_PIN_RTS     uart_config[uart].rts_pin
#define UART_PIN_CTS     uart_config[uart].cts_pin
#define UART_HWFLOWCTRL  (uart_config[uart].rts_pin != GPIO_UNDEF && \
                          uart_config[uart].cts_pin != GPIO_UNDEF)
#else
#define PSEL_RXD         dev(uart)->PSELRXD
#define PSEL_TXD         dev(uart)->PSELTXD
#define PSEL_RTS         dev(uart)->PSELRTS
#define PSEL_CTS         dev(uart)->PSELCTS
#define UART_0_ISR       isr_uart0
#ifndef UART_PIN_RTS
#define UART_PIN_RTS     GPIO_UNDEF
#endif
#ifndef UART_PIN_CTS
#define UART_PIN_CTS     GPIO_UNDEF
#endif
#ifndef UART_HWFLOWCTRL
#define UART_HWFLOWCTRL  0
#endif
#endif

/**
 * @brief Allocate memory for the interrupt context
 */
static uart_isr_ctx_t isr_ctx;

#ifdef CPU_MODEL_NRF52840XXAA
static inline NRF_UARTE_Type *dev(uart_t uart)
{
    return uart_config[uart].dev;
}
static uint8_t rx_buf[UART_NUMOF];
#else
static inline NRF_UART_Type *dev(uart_t uart)
{
    (void)uart;
    return  NRF_UART0;
}
#endif

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);

    /* remember callback addresses and argument */
    isr_ctx.rx_cb = rx_cb;
    isr_ctx.arg = arg;

#ifdef CPU_FAM_NRF51
   /* power on the UART device */
    dev(uart)->POWER = 1;
#endif

    /* reset configuration registers */
    dev(uart)->CONFIG = 0;

    /* configure RX pin */
    if (rx_cb) {
        gpio_init(UART_PIN_RX, GPIO_IN);
        PSEL_RXD = UART_PIN_RX;
    }

    /* configure TX pin */
    gpio_init(UART_PIN_TX, GPIO_OUT);
    PSEL_TXD = UART_PIN_TX;

    /* enable HW-flow control if defined */
    if (UART_HWFLOWCTRL) {
        /* set pin mode for RTS and CTS pins */
        gpio_init(UART_PIN_RTS, GPIO_OUT);
        gpio_init(UART_PIN_CTS, GPIO_IN);
        /* configure RTS and CTS pins to use */
        PSEL_RTS = UART_PIN_RTS;
        PSEL_CTS = UART_PIN_CTS;
        dev(uart)->CONFIG |= UART_CONFIG_HWFC_Msk;  /* enable HW flow control */
    } else {
        PSEL_RTS = 0xffffffff;            /* pin disconnected */
        PSEL_CTS = 0xffffffff;            /* pin disconnected */
    }

    /* select baudrate */
    switch (baudrate) {
        case 1200:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud1200;
            break;
        case 2400:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud2400;
            break;
        case 4800:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud4800;
            break;
        case 9600:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud9600;
            break;
        case 14400:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud14400;
            break;
        case 19200:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud19200;
            break;
        case 28800:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud28800;
            break;
        case 38400:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud38400;
            break;
        case 57600:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud57600;
            break;
        case 76800:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud76800;
            break;
        case 115200:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200;
            break;
        case 230400:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud230400;
            break;
        case 250000:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud250000;
            break;
        case 460800:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud460800;
            break;
        case 921600:
            dev(uart)->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud921600;
            break;
        default:
            return UART_NOBAUD;
    }

    /* enable the UART device */
#ifdef CPU_MODEL_NRF52840XXAA
    dev(uart)->ENABLE = UARTE_ENABLE_ENABLE_Enabled;
    dev(uart)->RXD.MAXCNT = 1;
    dev(uart)->RXD.PTR = (uint32_t)&rx_buf[uart];
#else
    dev(uart)->ENABLE = UART_ENABLE_ENABLE_Enabled;
    /* enable TX and RX*/
    dev(uart)->TASKS_STARTTX = 1;
#endif

    if (rx_cb) {
        dev(uart)->TASKS_STARTRX = 1;
        /* enable global and receiving interrupt */
        NVIC_EnableIRQ(UART_IRQN);
#ifdef CPU_MODEL_NRF52840XXAA
        dev(uart)->INTENSET = UARTE_INTENSET_RXDRDY_Msk;
#else
        dev(uart)->INTENSET = UART_INTENSET_RXDRDY_Msk;
#endif
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);

#ifdef CPU_MODEL_NRF52840XXAA /* nrf52840 uses EasyDMA to transmit data */
    /* reset endtx flag */
    dev(uart)->EVENTS_ENDTX = 0;
    /* set data to transfer to DMA TX pointer */
    dev(uart)->TXD.PTR = (uint32_t)data;
    dev(uart)->TXD.MAXCNT = len;
    /* start transmission */
    dev(uart)->TASKS_STARTTX = 1;
    /* wait for the end of transmission */
    while (dev(uart)->EVENTS_ENDTX == 0) {}
#else
    for (size_t i = 0; i < len; i++) {
        /* This section of the function is not thread safe:
            - another thread may mess up with the uart at the same time.
           In order to avoid an infinite loop in the interrupted thread,
           the TXRDY flag must be cleared before writing the data to be
           sent and not after. This way, the higher priority thread will
           exit this function with the TXRDY flag set, then the interrupted
           thread may have not transmitted his data but will still exit the
           while loop.
        */

        /* reset ready flag */
        dev(uart)->EVENTS_TXDRDY = 0;
        /* write data into transmit register */
        dev(uart)->TXD = data[i];
        /* wait for any transmission to be done */
        while (dev(uart)->EVENTS_TXDRDY == 0) {}
    }
#endif
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

    dev(uart)->TASKS_STARTRX = 1;
    dev(uart)->TASKS_STARTTX = 1;
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

#ifndef CPU_MODEL_NRF52840XXAA
    dev(uart)->TASKS_SUSPEND;
#else
    (void)uart;
#endif
}

static inline void irq_handler(uart_t uart)
{
    assert(uart < UART_NUMOF);
#ifdef CPU_MODEL_NRF52840XXAA /* nrf52840 uses EasyDMA to receive data */
    if (dev(uart)->EVENTS_RXDRDY == 1) {
        dev(uart)->EVENTS_RXDRDY = 0;
        /* RXRDY doesn't mean that received byte is in RAM
           so wait for ENDRX event */
        while(dev(uart)->EVENTS_ENDRX == 0) {}
        dev(uart)->EVENTS_ENDRX = 0;
        /* Process received byte */
        isr_ctx.rx_cb(isr_ctx.arg, rx_buf[uart]);
        /* Restart RX task */
        dev(uart)->TASKS_STARTRX = 1;
    }
#else
    if (dev(uart)->EVENTS_RXDRDY == 1) {
        dev(uart)->EVENTS_RXDRDY = 0;
        uint8_t byte = (uint8_t)(dev(uart)->RXD & 0xff);
        isr_ctx.rx_cb(isr_ctx.arg, byte);
    }
#endif
    cortexm_isr_end();
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(UART_DEV(0));
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(UART_DEV(1));
}
#endif
