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
 * @brief       Default STM32F4 clock configuration for 168MHz boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef F2F4F7_CFG_CLOCK_DEFAULT_168_H
#define F2F4F7_CFG_CLOCK_DEFAULT_168_H

#include "f2f4f7/cfg_clock_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock PLL settings (168MHz)
 * @{
 */
/* The following parameters configure a 168MHz system clock with HSE
   (8MHz, 12MHz or 16MHz) or HSI (16MHz) as PLL input clock */
#ifndef CONFIG_CLOCK_PLL_M
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE == MHZ(12))
#define CONFIG_CLOCK_PLL_M              (12)
#else
#define CONFIG_CLOCK_PLL_M              (4)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_N
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE == MHZ(12))
#define CONFIG_CLOCK_PLL_N              (336)
#elif IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE == MHZ(8))
#define CONFIG_CLOCK_PLL_N              (168)
#else
#define CONFIG_CLOCK_PLL_N              (84)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_P
#define CONFIG_CLOCK_PLL_P              (2)
#endif
#ifndef CONFIG_CLOCK_PLL_Q
#define CONFIG_CLOCK_PLL_Q              (7)
#endif
#ifndef CONFIG_CLOCK_PLL_R
#define CONFIG_CLOCK_PLL_R              (0)
#endif
/** @} */

/**
 * @name    Clock bus settings (APB1 and APB2)
 */
#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (4)         /* max 42MHz */
#endif
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (2)         /* max 84MHz */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#include "f2f4f7/cfg_clock_values.h"

#if CLOCK_CORECLOCK > MHZ(168)
#error "SYSCLK cannot exceed 168MHz"
#endif

#endif /* F2F4F7_CFG_CLOCK_DEFAULT_168_H */
/** @} */
