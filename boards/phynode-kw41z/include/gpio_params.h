/*
 * Copyright (C) 2018 Eistec AB
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_phynode-kw41z
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author    José Alamos <jose.alamos@haw-hamburg.de>
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LED configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    /* There are 2 user controlled LEDs on board. An orange LED (D2) and an
     * RGB LED (D10). The RGB LED is configured with 3 LEDx macros. */
#ifdef LED0_PIN
    {
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif /* LED0_PIN */
#ifdef LED1_PIN
    {
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif /* LED1_PIN */
#ifdef LED2_PIN
    {
        .pin = LED2_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif /* LED2_PIN */
#ifdef LED3_PIN
    {
        .pin = LED3_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
#endif /* LED3_PIN */
    {
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = (SAUL_GPIO_INVERTED),
    },
};

/**
 * @brief GPIO information for SAUL registry
 */
static const saul_reg_info_t saul_gpio_info[] =
{
#ifdef LED0_PIN
    { .name = "D2 (Orange)" },
#endif /* LED0_PIN */
#ifdef LED1_PIN
    { .name = "D10 RGB (Red)" },
#endif /* LED1_PIN */
#ifdef LED2_PIN
    { .name = "D10 RGB (Green)" },
#endif /* LED2_PIN */
#ifdef LED3_PIN
    { .name = "D10 RGB (Blue)" },
#endif /* LED3_PIN */
    { .name = "S2 (Button)" }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
