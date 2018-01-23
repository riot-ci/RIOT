/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_thingy52
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Thingy:52
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_PIN_RX         GPIO_PIN(0, 2)
#define UART_PIN_TX         GPIO_PIN(0, 3)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        /* main I2C bus */
        .dev = NRF_TWIM0,
        .sda = 7,
        .scl = 8,
        .speed = I2C_SPEED_NORMAL
    },
    {
        /* EXT I2C bus */
        .dev = NRF_TWIM1,
        .sda = 14,
        .scl = 15,
        .speed = I2C_SPEED_NORMAL
    },
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
