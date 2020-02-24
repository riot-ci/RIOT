/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bme680 BME680 Temperature/Humidity/Pressure/Gas sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for the Bosch BME680  sensor
 *
 * @{
 * @file
 * @brief       Interface definition for the Bosch BME680 sensor
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef BME680_H
#define BME680_H

#include "periph/i2c.h"
#include "periph/spi.h"

#ifdef MODULE_BME680_FP
#define BME680_FLOAT_POINT_COMPENSATION
#endif

#include "bme680_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    I2C address when SDO pin is LOW
 */
#define BME680_I2C_ADDR_1   (0x76)

/**
 * @brief    I2C address when SDO pin is HIGH
 */
#define BME680_I2C_ADDR_2   (0x77)

/**
 * @brief BME680 I2C parameters
 */
typedef struct {
    i2c_t dev;                    /**< I2C device which is used */
    uint8_t addr;                 /**< I2C address */
} bme680_intf_i2c_t;

/**
 * @brief   BME680 SPI parameters
 */
typedef struct {
    spi_t dev;                  /**< SPI device which is used */
    gpio_t nss_pin;             /**< Chip Select pin */
} bme680_intf_spi_t;

/**
 * @brief   BME680 Hardware interface parameters union
 */
typedef union {
    bme680_intf_i2c_t i2c;        /**< I2C specific interface parameters */
    bme680_intf_spi_t spi;        /**< SPI specific interface parameters */
} bme680_intf_t;

/**
 * @brief   BME680 device initialization parameters
 */
typedef struct {
    uint8_t ifsel;              /**< Interface selection */
    uint8_t temp_os;            /**< Temperature oversampling */
    uint8_t hum_os;             /**< Humidity oversampling */
    uint8_t pres_os;            /**< Pressure oversampling */
    uint8_t filter;             /**< IIR filter coefficient */
    uint8_t gas_measure;        /**< Enable gas measurement */
    uint16_t heater_dur;        /**< Heater duration in ms */
    uint16_t heater_temp;       /**< Heater temperature in °C */
    uint8_t power_mode;         /**< Power mode (sleep or forced) */
    uint8_t settings;           /**< Settings used by @ref bme680_set_settings */
    bme680_intf_t intf;         /**< Hardware interface parameters */
} bme680_params_t;

 /**
 * @brief   BME680 sensor device data structure type
 */
typedef struct {
    bme680_params_t params;     /**< Device initialization parameters */
    struct bme680_dev  dev;     /**< Device structure from bme680_driver pkg */
 } bme680_t;

 #ifdef __cplusplus
}
#endif

/**
 * @brief   Initialize the BME680 sensor driver.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  0 on success
 * @return                  < 0 on failures
  */
int bme680_init(bme680_t *dev, const bme680_params_t *params);

#endif /* BME680_H */
/** @} */
