/*
 * Copyright (C) 2016-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_nrf52xxxdk NRF52 DK common
 * @ingroup     boards_common_nrf52
 * @{
 *
 * @file
 * @brief       Common peripheral configuration for the nRF52DK/nRF52840DK
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef PERIPH_CONF_COMMON_H
#define PERIPH_CONF_COMMON_H

#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SPI_PORT
#define SPI_PORT 0
#endif

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev = NRF_SPI0,
        .sclk = GPIO_PIN(SPI_PORT, 15),
        .mosi = GPIO_PIN(SPI_PORT, 13),
        .miso = GPIO_PIN(SPI_PORT, 14),
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM1,
        .scl = GPIO_PIN(0, 27),
        .sda = GPIO_PIN(0, 26),
        .speed = I2C_SPEED_NORMAL
    }
};
#define I2C_NUMOF           (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        NRF_PWM0, {
            GPIO_PIN(0, 28), GPIO_PIN(0, 29), GPIO_PIN(0, 30),
            GPIO_PIN(0, 31)
        }
    }
};
#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
