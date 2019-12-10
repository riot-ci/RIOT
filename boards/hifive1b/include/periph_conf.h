/*
 * Copyright (C) 2019 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_hifive1b
 * @{
 *
 * @file
 * @brief       Peripheral specific definitions for the HiFive1b RISC-V board
 *
 * @author      Ken Rabold
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Core Clock configuration
 * @{
 */
#define USE_CLOCK_PLL               (1)
#define USE_CLOCK_HFROSC            (0)
#define USE_CLOCK_HFXOSC            (0)

#if USE_CLOCK_PLL && (USE_CLOCK_HFXOSC || USE_CLOCK_HFROSC)
#error "Cannot use HFXOSC/HFROSC with PLL"
#endif
#if USE_CLOCK_HFXOSC && (USE_CLOCK_PLL || USE_CLOCK_HFROSC)
#error "Cannot use PLL/HFROSC with HFXOSC"
#endif

#if USE_CLOCK_PLL
#define CLOCK_PLL_R                 (1)             /* Divide input clock by 2 */
#define CLOCK_PLL_F                 (23)            /* Multiply REFR by 48, e.g 2 * (23 + 1) */
#define CLOCK_PLL_Q                 (3)             /* Divide VCO by 8, e.g 2^3 */
#define CLOCK_PLL_OUTDIV            (1)             /* Divide output PLL freq by 1 */
#define CLOCK_PLL_INPUT_CLOCK       (16000000UL)
#define CLOCK_PLL_REFR              (CLOCK_PLL_INPUT_CLOCK / (CLOCK_PLL_R + 1))
#define CLOCK_PLL_VCO               (CLOCK_PLL_REFR * (2 * (CLOCK_PLL_F + 1)))
#define CLOCK_PLL_OUT               (CLOCK_PLL_VCO / (1 << CLOCK_PLL_Q))
#define CLOCK_CORECLOCK             (CLOCK_PLL_OUT / CLOCK_PLL_OUTDIV)
#elif USE_CLOCK_HFROSC
#define CLOCK_HFROSC_TRIM           (6)             /* 72000000Hz input freq */
#define CLOCK_HFROSC_DIV            (1)             /* Divide by 2 */
#define CLOCK_CORECLOCK             (72000000UL / (CLOCK_HFROSC_DIV + 1))
#elif USE_CLOCK_HFXOSC
#define CLOCK_CORECLOCK             (16000000UL)
#else /* Default HFROSC clock source */
#define CLOCK_CORECLOCK             (13800000UL)
#endif
/** @} */

/**
 * @name    Timer configuration
 *
 * @{
 */
#define TIMER_NUMOF                 (1)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .addr       = UART0_CTRL_ADDR,
        .rx         = GPIO_PIN(0, 16),
        .tx         = GPIO_PIN(0, 17),
        .isr_num    = INT_UART0_BASE,
    },
    {
        .addr       = UART1_CTRL_ADDR,
        .rx         = GPIO_PIN(0, 18),
        .tx         = GPIO_PIN(0, 23),
        .isr_num    = INT_UART1_BASE,
    },
};


#define UART_ISR_PRIO               (2)
#define UART_NUMOF                  ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    RTT/RTC configuration
 *
 * @{
 */
#define RTT_FREQUENCY               (1)             /* in Hz */
#define RTT_MAX_VALUE               (0xFFFFFFFF)
#define RTT_INTR_PRIORITY           (2)

/** @} */

/**
 * @name    GPIO configuration
 *
 * @{
 */
#define GPIO_INTR_PRIORITY          (3)
/** @} */

/**
 * @name    PWM configuration
 *
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {   .addr  = PWM1_CTRL_ADDR,
        .chan = {  /* GPIO pin, channel comparator index */
                    { .pin = GPIO_PIN(0, 19), .cmp = 0 }, /* D3, on-board green LED */
                    { .pin = GPIO_PIN(0, 20), .cmp = 1 }, /* D4 */
                    { .pin = GPIO_PIN(0, 21), .cmp = 2 }, /* D5, On-board blue LED */
                    { .pin = GPIO_PIN(0, 22), .cmp = 3 }} /* D6, On-board red LED */
    },
    {   .addr  = PWM2_CTRL_ADDR,
        .chan = {  /* GPIO pin, channel comparator index */
                    { .pin = GPIO_PIN(0, 10), .cmp = 0 }, /* D16 */
                    { .pin = GPIO_PIN(0, 11), .cmp = 1 }, /* D17 */
                    { .pin = GPIO_UNDEF, .cmp = 2 },
                    { .pin = GPIO_UNDEF, .cmp = 3 }}
    },
};

#define PWM_NUMOF                   ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
