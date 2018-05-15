/*
 * Copyright (C) Inria 2018
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_stm32l476g-disco
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Alexandre Abadie <alexandre.abadie@inria.fr>
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
        .name = "LD4",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LD5",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "Joystick (Center)",
        .pin = BTN_JOYSTICK_CENTER,
        .mode = GPIO_IN_PD
    },
    {
        .name = "Joystick (Left)",
        .pin = BTN_JOYSTICK_LEFT,
        .mode = GPIO_IN_PD
    },
    {
        .name = "Joystick (Down)",
        .pin = BTN_JOYSTICK_DOWN,
        .mode = GPIO_IN_PD
    },
    {
        .name = "Joystick (Right)",
        .pin = BTN_JOYSTICK_RIGHT,
        .mode = GPIO_IN_PD
    },
    {
        .name = "Joystick (Up)",
        .pin = BTN_JOYSTICK_UP,
        .mode = GPIO_IN_PD
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
