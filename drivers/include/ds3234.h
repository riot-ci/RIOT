/*
 * Copyright 2018 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ds3234  DS3234 Extremely Accurate SPI RTC

 * @ingroup     drivers_sensors
 * @brief       Driver for Maxim DS3234 Extremely Accurate SPI Bus RTC with
 *              Integrated Crystal and SRAM

 * @{
 *
 * @file
 * @brief       DS3234 device driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef DS3234_H
#define DS3234_H

#include <periph/gpio.h>
#include <periph/spi.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Parameters for the DS3234 device driver
 */
typedef struct {
    spi_t spi;              /**< SPI bus the sensor is connected to */
    spi_clk_t clk;          /**< SPI bus clock speed */
    gpio_t cs;              /**< CS pin GPIO handle */
} ds3234_params_t;


/**
 * @brief   Initialize the DS3234 RTC as a PPS device
 *
 * This will enable square wave output on the SQW pin at 1 Hz
 *
 * @param  params     DS3234 circuit parameters
 *
 * @retval  0         Success
 * @retval -EIO       IO failure (`spi_init_cs()` failed)
 */
int ds3234_pps_init(const ds3234_params_t *params);

#ifdef __cplusplus
}
#endif

#endif /* SHT1X_H */
/** @} */
