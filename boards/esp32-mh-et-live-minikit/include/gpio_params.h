/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

/**
 * @ingroup     boards_esp32_mh-et-live-minikit
 * @brief       Board specific configuration of direct mapped GPIOs
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#include "board.h"
#include "saul/periph.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INIT_CLEAR
    }
};

/**
 * @brief GPIO information for SAUL registry
 */
static const saul_reg_info_t saul_gpio_info[] =
{
    { .name = "LED" }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */
