/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_frdm-k22f
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
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
    {
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
};

/**
 * @brief GPIO information for SAUL registry
 */
static const saul_reg_info_t saul_gpio_info[] =
{
    { .name = "LED(red)" },
    { .name = "LED(green)" },
    { .name = "LED(blue)" }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
