/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_atxmega-a1-xplained
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the ATxmegaA1 Xplained board.
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */
#include "mutex.h"

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <avr/io.h>

#include "periph_cpu.h"

/**
 * @brief   Timer configuration
 *
 * ATTETION Timer 0 is used for Xtimer which is system Timer
 *
 * RIOT Timer 0 is Timer Counter A1
 * @{
 */
#define TIMER_NUMOF         (1U)

#define TIMER_0             (&TCC1)
#define TIMER_0_MASK        ()
#define TIMER_0_FLAG        ()
#define TIMER_0_INT_LVL     (INT_LVL_LOW)
#define TIMER_0_OVF         TCC1_OVF_vect
#define TIMER_0_ISRA        TCC1_CCA_vect
#define TIMER_0_ISRB        TCC1_CCB_vect
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {   /* J1 */
        .dev = &USARTF0,
        .rx_pin = GPIO_PIN(PORT_F, PIN2_bm),
        .tx_pin = GPIO_PIN(PORT_F, PIN3_bm),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin = GPIO_UNDEF,
        .cts_pin = GPIO_UNDEF,
#endif
        .rx_int_lvl = INT_LVL_LOW,
        .tx_int_lvl = INT_LVL_OFF,
        .dre_int_lvl = INT_LVL_OFF,
    },
    {   /* J3 - Shared with SAUL */
        .dev = &USARTD0,
        .rx_pin = GPIO_PIN(PORT_D, PIN2_bm),
        .tx_pin = GPIO_PIN(PORT_D, PIN3_bm),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin = GPIO_UNDEF,
        .cts_pin = GPIO_UNDEF,
#endif
        .rx_int_lvl = INT_LVL_LOW,
        .tx_int_lvl = INT_LVL_OFF,
        .dre_int_lvl = INT_LVL_OFF,
    },
    {   /* J4 */
        .dev = &USARTC0,
        .rx_pin = GPIO_PIN(PORT_C, PIN2_bm),
        .tx_pin = GPIO_PIN(PORT_C, PIN3_bm),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin = GPIO_UNDEF,
        .cts_pin = GPIO_UNDEF,
#endif
        .rx_int_lvl = INT_LVL_LOW,
        .tx_int_lvl = INT_LVL_OFF,
        .dre_int_lvl = INT_LVL_OFF,
    },
};

/* interrupt function name mapping */
#define UART_0_RXC_ISR      USARTF0_RXC_vect    /* Reception Complete Interrupt */
#define UART_0_DRE_ISR      USARTF0_DRE_vect    /* Data Register Empty Interrupt */
#define UART_0_TXC_ISR      USARTF0_TXC_vect    /* Transmission Complete Interrupt */

#define UART_1_RXC_ISR      USARTD0_RXC_vect
#define UART_1_DRE_ISR      USARTD0_DRE_vect
#define UART_1_TXC_ISR      USARTD0_TXC_vect

#define UART_2_RXC_ISR      USARTC0_RXC_vect
#define UART_2_DRE_ISR      USARTC0_DRE_vect
#define UART_2_TXC_ISR      USARTC0_TXC_vect

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
