/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dylan Laduranty <dylanladuranty@gmail.com>
 *
 * @}
 */

#include "cpu.h"

#include "periph/uart.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* do not build the file in case no UART is defined */
#ifdef UART_NUMOF

/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

/**
 * @brief   Get the pointer to the base register of the given UART device
 *
 * @param[in] dev       UART device identifier
 *
 * @return              base register address
 */
static inline SercomUsart *_uart(uart_t dev)
{
    return uart_config[dev].dev;
}

#ifdef CPU_FAM_SAML21
static uint8_t sercom_gclk_id[] =
        {
            SERCOM0_GCLK_ID_CORE,
            SERCOM1_GCLK_ID_CORE,
            SERCOM2_GCLK_ID_CORE,
            SERCOM3_GCLK_ID_CORE,
            SERCOM4_GCLK_ID_CORE,
            SERCOM5_GCLK_ID_CORE
        };
#endif

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    /* configure pins */
    gpio_init(uart_config[uart].rx_pin, GPIO_IN);
    gpio_init_mux(uart_config[uart].rx_pin, uart_config[uart].mux);
    gpio_init(uart_config[uart].tx_pin, GPIO_OUT);
    gpio_set(uart_config[uart].tx_pin);
    gpio_init_mux(uart_config[uart].tx_pin, uart_config[uart].mux);

#ifdef CPU_FAM_SAMD21
    uart_poweron(uart);
#endif

    /* reset the UART device */
    _uart(uart)->CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
    while (_uart(uart)->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_SWRST) {}

    /* set asynchronous mode w/o parity, LSB first, TX and RX pad as specified
     * by the board in the periph_conf.h, x16 sampling and use internal clock */
    _uart(uart)->CTRLA.reg = (SERCOM_USART_CTRLA_DORD |
                      SERCOM_USART_CTRLA_SAMPR(0x1) |
                      SERCOM_USART_CTRLA_TXPO(uart_config[uart].tx_pad) |
                      SERCOM_USART_CTRLA_RXPO(uart_config[uart].rx_pad) |
                      SERCOM_USART_CTRLA_MODE(0x1) |
                      (uart_config[uart].runstdby ?
                              SERCOM_USART_CTRLA_RUNSTDBY : 0));

    /* calculate and set baudrate */
    uint32_t baud = ((((uint32_t)CLOCK_CORECLOCK * 10) / baudrate) / 16);
    _uart(uart)->BAUD.FRAC.FP = (baud % 10);
    _uart(uart)->BAUD.FRAC.BAUD = (baud / 10);

    /* enable transmitter, and configure 8N1 mode */
    _uart(uart)->CTRLB.reg = (SERCOM_USART_CTRLB_TXEN);
    /* enable receiver and RX interrupt if configured */
    if (rx_cb) {
        uart_ctx[uart].rx_cb = rx_cb;
        uart_ctx[uart].arg = arg;
        NVIC_EnableIRQ(SERCOM0_IRQn + sercom_id(_uart(uart)));
        _uart(uart)->CTRLB.reg |= SERCOM_USART_CTRLB_RXEN;
        _uart(uart)->INTENSET.reg |= SERCOM_USART_INTENSET_RXC;
    }
    while (_uart(uart)->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_CTRLB) {}

    /* and finally enable the device */
#ifdef CPU_FAM_SAML21
    uart_poweron(uart);
#else
    _uart(uart)->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
#endif

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (!(_uart(uart)->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE)) {}
        _uart(uart)->DATA.reg = data[i];
        while (_uart(uart)->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) {}
    }
}

void uart_poweron(uart_t uart)
{
#ifdef CPU_FAM_SAMD21
    PM->APBCMASK.reg |= (PM_APBCMASK_SERCOM0 << sercom_id(_uart(uart)));
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN |
                         GCLK_CLKCTRL_GEN(uart_config[uart].gclk_src) |
                         (SERCOM0_GCLK_ID_CORE + sercom_id(_uart(uart))) <<
                          GCLK_CLKCTRL_ID_Pos);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

#elif CPU_FAM_SAML21
    /* Enable the peripheral channel */
    GCLK->PCHCTRL[sercom_gclk_id[sercom_id(_uart(uart))]].reg |=
            GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(uart_config[uart].gclk_src);

    while (!(GCLK->PCHCTRL[sercom_gclk_id[sercom_id(_uart(uart))]].reg &
           GCLK_PCHCTRL_CHEN)) {}
    if(sercom_gclk_id[sercom_id(_uart(uart))] < 5) {
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM0 << sercom_id(_uart(uart));
    }
    else {
        MCLK->APBDMASK.reg |= MCLK_APBDMASK_SERCOM5;
    }
    while (_uart(uart)->SYNCBUSY.reg) {}
#endif

    /* finally, enable the device */
    _uart(uart)->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}

void uart_poweroff(uart_t uart)
{
    /* disable device */
    _uart(uart)->CTRLA.reg &= ~(SERCOM_USART_CTRLA_ENABLE);

#ifdef CPU_FAM_SAMD21
    PM->APBCMASK.reg &= ~(PM_APBCMASK_SERCOM0 << sercom_id(_uart(uart)));
    GCLK->CLKCTRL.reg = ((SERCOM0_GCLK_ID_CORE + sercom_id(_uart(uart))) <<
                          GCLK_CLKCTRL_ID_Pos);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

#elif CPU_FAM_SAML21
    /* Enable the peripheral channel */
    GCLK->PCHCTRL[sercom_gclk_id[sercom_id(_uart(uart))]].reg &= ~GCLK_PCHCTRL_CHEN;

    if(sercom_gclk_id[sercom_id(_uart(uart))] < 5) {
        MCLK->APBCMASK.reg &= ~(MCLK_APBCMASK_SERCOM0 << sercom_id(_uart(uart)));
    }
    else {
        MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_SERCOM5;
    }
    while (_uart(uart)->SYNCBUSY.reg) {}
#endif
}

static inline void irq_handler(uint8_t uartnum)
{
    if (_uart(uartnum)->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) {
        /* interrupt flag is cleared by reading the data register */
        uart_ctx[uartnum].rx_cb(uart_ctx[uartnum].arg,
                                (uint8_t)(_uart(uartnum)->DATA.reg));
    }
    else if (_uart(uartnum)->INTFLAG.reg & SERCOM_USART_INTFLAG_ERROR) {
        /* clear error flag */
        _uart(uartnum)->INTFLAG.reg = SERCOM_USART_INTFLAG_ERROR;
    }

    cortexm_isr_end();
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(0);
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(1);
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    irq_handler(2);
}
#endif

#ifdef UART_3_ISR
void UART_3_ISR(void)
{
    irq_handler(3);
}
#endif

#ifdef UART_4_ISR
void UART_4_ISR(void)
{
    irq_handler(4);
}
#endif

#ifdef UART_5_ISR
void UART_5_ISR(void)
{
    irq_handler(5);
}
#endif

#endif /* UART_NUMOF */
