/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-g474re
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-g474re board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "g4/cfg_clock_default.h"
#include "cfg_rtt_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM5,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR1_TIM5EN,
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    }
};

#define TIMER_0_ISR         isr_tim5

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = LPUART1,
        .rcc_mask   = RCC_APB1ENR2_LPUART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF12,
        .tx_af      = GPIO_AF12,
        .bus        = APB12,
        .irqn       = LPUART1_IRQn,
        .type       = STM32_LPUART,
        .clk_src    = 0, /* Use APB clock */
    },
    { /* Connected to Arduino D0/D1 */
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_C, 5),
        .tx_pin     = GPIO_PIN(PORT_C, 4),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART1_IRQn,
        .type       = STM32_LPUART,
        .clk_src    = 0, /* Use APB clock */
    },
};

#define UART_0_ISR          (isr_lpuart1)
#define UART_1_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
