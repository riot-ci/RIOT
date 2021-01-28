/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_wm-200
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GPIO configuration
 */
static const saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "DFU",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED RED",
        .pin = LED0_PIN,
        .mode = LED0_MODE,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_SET),
    },
    {
        .name = "LED GREEN",
        .pin = LED1_PIN,
        .mode = LED1_MODE,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_SET),
    },
    {
        .name = "LED BLUE",
        .pin = LED2_PIN,
        .mode = LED2_MODE,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_SET),
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
