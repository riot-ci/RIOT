/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *               2018-2020 Inria
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
 * @brief       Base STM32F4 clock configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef F4_CFG_CLOCK_COMMON_H
#define F4_CFG_CLOCK_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock common configuration
 * @{
 */
/* Select the desired system clock source between PLL, HSE or HSI */
#ifndef CONFIG_USE_CLOCK_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CONFIG_USE_CLOCK_PLL            0
#else
#define CONFIG_USE_CLOCK_PLL            1     /* Use PLL by default */
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#ifndef CONFIG_USE_CLOCK_HSE
#define CONFIG_USE_CLOCK_HSE            0
#endif /* CONFIG_USE_CLOCK_HSE */

#ifndef CONFIG_USE_CLOCK_HSI
#define CONFIG_USE_CLOCK_HSI            0
#endif /* CONFIG_USE_CLOCK_HSI */

#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use PLL as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use HSE as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_HSE))
#error "Cannot use HSI as clock source with other clock configurations"
#endif

#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE            0
#endif

#ifndef CLOCK_HSE
#define CLOCK_HSE                       MHZ(8)
#endif

#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            0
#endif
#if IS_ACTIVE(CONFIG_BOARD_HAS_LSE)
#define CLOCK_LSE                       (1)
#else
#define CLOCK_LSE                       (0)
#endif

#define CLOCK_HSI                       MHZ(16)

/* The following parameters configure a 48MHz I2S clock with HSE (8MHz) or HSI (16MHz) as PLL input clock */
#ifndef CONFIG_CLOCK_PLLI2S_M
#define CONFIG_CLOCK_PLLI2S_M           (4)
#endif
#ifndef CONFIG_CLOCK_PLLI2S_N
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CONFIG_CLOCK_PLLI2S_N           (192)
#else
#define CONFIG_CLOCK_PLLI2S_N           (96)
#endif
#endif
#ifndef CONFIG_CLOCK_PLLI2S_P
#define CONFIG_CLOCK_PLLI2S_P           (8)
#endif
#ifndef CONFIG_CLOCK_PLLI2S_Q
#define CONFIG_CLOCK_PLLI2S_Q           (8)
#endif
#ifndef CONFIG_CLOCK_PLLI2S_R
#define CONFIG_CLOCK_PLLI2S_R           (8)
#endif
/* The following parameters configure a 48MHz SAI clock with HSE (8MHz) or HSI (16MHz) as PLL input clock */
#ifndef CONFIG_CLOCK_PLLSAI_M
#define CONFIG_CLOCK_PLLSAI_M           (4)
#endif
#ifndef CONFIG_CLOCK_PLLSAI_N
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CONFIG_CLOCK_PLLSAI_N           (192)
#else
#define CONFIG_CLOCK_PLLSAI_N           (96)
#endif
#endif
#ifndef CONFIG_CLOCK_PLLSAI_P
#define CONFIG_CLOCK_PLLSAI_P           (8)
#endif
#ifndef CONFIG_CLOCK_PLLSAI_Q
#define CONFIG_CLOCK_PLLSAI_Q           (8)
#endif
#ifndef CONFIG_CLOCK_PLLSAI_R
#define CONFIG_CLOCK_PLLSAI_R           (8)
#endif

#ifndef CONFIG_CLOCK_ENABLE_PLLI2S
#if defined(MODULE_PERIPH_USBDEV)
#define CONFIG_CLOCK_ENABLE_PLLI2S      1
#else
#define CONFIG_CLOCK_ENABLE_PLLI2S      0
#endif
#endif

#ifndef CONFIG_CLOCK_ENABLE_PLLSAI
#if defined(MODULE_PERIPH_USBDEV)
#define CONFIG_CLOCK_ENABLE_PLLSAI      1
#else
#define CONFIG_CLOCK_ENABLE_PLLSAI      0
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* F4_CFG_CLOCK_COMMON_H */
/** @} */
