/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_shtc1
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @name        Device driver interface for the SHTC1 Temperature and humidity sensor
 * @{
 *
 * @file
 * @brief       Device driver interface for the SHTC1 Temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef SHTC1_H
#define SHTC1_H

#include <stdint.h>
#include "saul.h"

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   SHTC1 Default Address
 *
 */
#ifndef SHTC1_I2C_ADDRESS
#define SHTC1_I2C_ADDRESS         (0x70)
#endif

typedef enum {
    CRC_DISABLED = 0,
    CRC_ENABLED
} shtc1_crc_type_t;

/**
 * @brief   value struct, where all results obtained from the sensor will be saved.
 * @{
 */
typedef struct {
    int16_t temp;          /**< Temperature in centi °C (2372 = 23.72) after an call to the measurement function. */
    uint16_t rel_humidity; /**< Relative humidity in centi percent (9922 = 99.22) after an call to the measuerment funtion. */
    unsigned int id;       /**< ID read from the sensor, only available after shtc1_id() was called. */
} shtc1_values_t;
/** @} */

/**
 * @brief   settings struct with all relevant parameters
 * @{
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C bus descriptor. */
    uint8_t i2c_addr;           /**< I2C address of the sensor. */
    shtc1_crc_type_t crc;       /**< crc check enabled or disabled (CRC_ENABLED/CRC_DISABLED). */
} shtc1_params_t;
/** @} */

/**
 * @brief   device descriptor for the SHTC1
 * @{
 */
typedef struct {
    shtc1_values_t values;  /**< Values struct, where all read data will be stored. */
    shtc1_params_t params;  /**< Paramteres struct with all settings set. */
} shtc1_t;
/** @} */

enum {
    SHTC1_OK = 0,
    SHTC1_ERROR = -1
};

/**
 * @brief Initializes the sensor and I2C.
 *
 * @param[in] dev       I2C device descriptor.
 * @param[in] params    SHTC1 parameters to be used.
 *
 * @return              SHTC1_OK on a working initialization.
 * @return              SHTC1_ERROR on error.
 */
int8_t shtc1_init(shtc1_t* const dev, const shtc1_params_t* params);

/**
 * @brief Reads all register values from the device.
 * @details The values as raw data will be saved into reveived.
 *
 * @param[in] dev            The I2C device descriptor.
 * @param[in] received       Array to save the values.
 *
 * @return              SHTC1_OK if a measurement completed. The values will be stored
 *                      in the received array.
 * @return              SHTC1_ERROR on checksum error.
 */
int8_t shtc1_get_measurement(const shtc1_t *dev, uint8_t *received);

/**
 * @brief Reads temperatur value.
 * @details The value will be saved into temp.
 *
 * @param[in] dev            The I2C device descriptor.
 * @param[in] temp           Variable to save the temperature value. Temperature in centi °C
 *
 * @return              SHTC1_OK if a measurement completed.
 * @return              SHTC1_ERROR on checksum error.
 */
int8_t shtc1_read_temperature(const shtc1_t *dev, int16_t *temp);

/**
 * @brief Reads relative humidity value.
 * @details The value will be saved into rel_humidity.
 *
 * @param[in] dev            The I2C device descriptor.
 * @param[in] temp           Variable to save the humidity value. Humidity in centi %
 *
 * @return              SHTC1_OK if a measurement completed.
 * @return              SHTC1_ERROR on checksum error.
 */
int8_t shtc1_read_relative_humidity(const shtc1_t *dev, uint16_t *rel_humidity);



/**
 * @brief Reads the ID and saves it in the device descriptor
 *
 * @details             When working correctly ID should equal xxxx'xxxx'xx00'0111 where x is unspecified.
 *
 * @param[in] dev       The I2C device descriptor.
 *
 * @return              SHTC1_OK on everything done.
 * @return              SHTC1_ERROR on error.
 */

int8_t shtc1_id(shtc1_t* const dev);

/**
 * @brief Resets sensor
 *
 * This will reset all internal state machines and reload calibration data from the memory.
 *
 * @param[in] dev       The I2C device descriptor.
 *
 * @return              SHTC1_OK on everything done.
 * @return              SHTC1_ERROR on error.
 */
int8_t shtc1_reset(const shtc1_t* const dev);

#ifdef __cplusplus
}
#endif

#endif /* SHTC1_H */
/** @} */
