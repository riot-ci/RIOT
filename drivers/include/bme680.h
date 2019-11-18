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

#include "bme680_internal.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   BME680 device initialization parameters
 */
typedef struct {
    uint8_t temp_os;
    uint8_t hum_os;
    uint8_t pres_os;
    uint8_t filter;
    uint8_t gas_measure;
    uint16_t heater_dur;
    uint16_t heater_temp;
    uint8_t power_mode;
    uint8_t settings;           /**< Settings used by @ref bme680_set_settings */
    uint8_t intf;               /**< Device communication interface */

    
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

int bme680_init(bme680_t *dev, const bme680_params_t *params);
#endif /* BME680_H */
/** @} */