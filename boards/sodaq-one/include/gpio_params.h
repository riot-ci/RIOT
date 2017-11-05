/*
 * Copyright (C) 2017 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-one
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Kees Bakker <kees@sodaq.com>
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    GPIO pin configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LED Green",
        .pin = LED_GREEN_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED Red",
        .pin = LED_RED_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED Blue",
        .pin = LED_BLUE_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "Button",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
