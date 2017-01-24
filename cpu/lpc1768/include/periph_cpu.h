/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_lpc1768
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"
#include "periph/dev_enums.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Override the default GPIO type
 * @{
 */
#define HAVE_GPIO_T
typedef uint16_t gpio_t;
/** @} */

/**
 * @brief   Define a custom GPIO_PIN macro for the lpc1768
 */
#define GPIO_PIN(port, pin)     (gpio_t)((port << 8) | pin)

/**
 * @brief   Override the default GPIO mode values
 * @{
 */
#define IN                  (0x0 << 0)
#define OUT                 (0x1 << 0)

#define PU                  (0x0 << 1)
#define PD                  (0x1 << 1)

#define OD                  (0x1 << 3)

#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = (IN),              /**< in without pull resistor */
    GPIO_IN_PD = (IN | PD),         /**< in with pull-down */
    GPIO_IN_PU = (IN | PU),         /**< in with pull-up */
    GPIO_OUT   = (OUT),             /**< push-pull output */
    GPIO_OD    = (OUT | OD),        /**< open-drain output */
    GPIO_OD_PU = (OUT | OD | PU)    /**< open-drain output with pull-up */
} gpio_mode_t;
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
