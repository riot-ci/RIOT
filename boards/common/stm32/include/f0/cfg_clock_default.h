/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Default clock configuration for STM32F0
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef F0_CFG_CLOCK_DEFAULT_H
#define F0_CFG_CLOCK_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 * @{
 */
/* Select the desired system clock source between PLL, HSE or HSI */
#ifndef CONFIG_USE_CLOCK_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CONFIG_USE_CLOCK_PLL            (0)
#else
#define CONFIG_USE_CLOCK_PLL            (1)     /* Use PLL by default */
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#ifndef CONFIG_USE_CLOCK_HSE
#define CONFIG_USE_CLOCK_HSE            (0)
#endif /* CONFIG_USE_CLOCK_HSE */

#ifndef CONFIG_USE_CLOCK_HSI
#define CONFIG_USE_CLOCK_HSI            (0)
#endif /* CONFIG_USE_CLOCK_HSI */

#if CONFIG_USE_CLOCK_PLL && \
    (CONFIG_USE_CLOCK_HSE || CONFIG_USE_CLOCK_HSI)
#error "Cannot use PLL as clock source with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_HSE && \
    (CONFIG_USE_CLOCK_PLL || CONFIG_USE_CLOCK_HSI)
#error "Cannot use HSE as clock source with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_HSI && \
    (CONFIG_USE_CLOCK_PLL || CONFIG_USE_CLOCK_HSE)
#error "Cannot use HSI as clock source with other clock configurations"
#endif

#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE            (0)
#endif

#ifndef CLOCK_HSE
#define CLOCK_HSE                       MHZ(8)
#endif
#if CONFIG_BOARD_HAS_HSE && (CLOCK_HSE < MHZ(4) || CLOCK_HSE > MHZ(32))
#error "HSE clock frequency must be between 4MHz and 32MHz"
#endif

#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            (0)
#endif
#if CONFIG_BOARD_HAS_LSE
#define CLOCK_LSE                       (1)
#else
#define CLOCK_LSE                       (0)
#endif

#define CLOCK_HSI                       MHZ(8)

/* The following parameters configure a 48MHz system clock with HSI (or default HSE) as input clock */
#ifndef CONFIG_CLOCK_PLL_PREDIV
#define CONFIG_CLOCK_PLL_PREDIV         (1)
#endif
#ifndef CONFIG_CLOCK_PLL_MUL
#define CONFIG_CLOCK_PLL_MUL            (6)
#endif

#if CONFIG_USE_CLOCK_HSI
#define CLOCK_CORECLOCK                 (CLOCK_HSI)

#elif CONFIG_USE_CLOCK_HSE
#if CONFIG_BOARD_HAS_HSE == 0
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CLOCK_HSE)

#elif CONFIG_USE_CLOCK_PLL
#if CONFIG_BOARD_HAS_HSE
#define CLOCK_PLL_SRC                   (CLOCK_HSE)
#else /* CLOCK_HSI */
#define CLOCK_PLL_SRC                   (CLOCK_HSI)
#endif
#define CLOCK_CORECLOCK                 ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_PREDIV) * CONFIG_CLOCK_PLL_MUL)
#if CLOCK_CORECLOCK > MHZ(48)
#error "SYSCLK cannot exceed 48MHz"
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_AHB                       CLOCK_CORECLOCK  /* max: 48MHz */

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (1)
#endif
#define CLOCK_APB1                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB1_DIV)   /* max: 48MHz */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* F0_CFG_CLOCK_DEFAULT_H */
/** @} */
