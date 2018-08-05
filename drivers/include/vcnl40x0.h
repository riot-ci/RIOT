/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_vcnl40x0 VCNL40X0
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the VCNL40X0 Proximity and Ambient Light Sensors.
 * @{
 *
 * @file
 * @brief       Device driver interface for the VCNL40X0 sensors family.
 *
 * @note        VCNL4010, VCNL4020 and VCNL4040 are supported.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef VCNL40X0_H
#define VCNL40X0_H

#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Proximity measure rate
 */
enum {
    VCNL40X0_PROXIMITY_RATE_2 = 0,               /**< Proximity measurement rate 2 */
    VCNL40X0_PROXIMITY_RATE_4,                   /**< Proximity measurement rate 4 */
    VCNL40X0_PROXIMITY_RATE_8,                   /**< Proximity measurement rate 8 */
    VCNL40X0_PROXIMITY_RATE_16,                  /**< Proximity measurement rate 16 */
    VCNL40X0_PROXIMITY_RATE_31,                  /**< Proximity measurement rate 31 */
    VCNL40X0_PROXIMITY_RATE_62,                  /**< Proximity measurement rate 62 */
    VCNL40X0_PROXIMITY_RATE_125,                 /**< Proximity measurement rate 125 */
    VCNL40X0_PROXIMITY_RATE_250,                 /**< Proximity measurement rate 250 */
};

/**
 * @brief Ambient light measurement average
 */
enum {
    VCNL40X0_AMBIENT_AVERAGE_1 = 0,              /**< Ambient average measurement 1 */
    VCNL40X0_AMBIENT_AVERAGE_2,                  /**< Ambient average measurement 2 */
    VCNL40X0_AMBIENT_AVERAGE_4,                  /**< Ambient average measurement 4 */
    VCNL40X0_AMBIENT_AVERAGE_8,                  /**< Ambient average measurement 8 */
    VCNL40X0_AMBIENT_AVERAGE_16,                 /**< Ambient average measurement 16 */
    VCNL40X0_AMBIENT_AVERAGE_32,                 /**< Ambient average measurement 32 */
    VCNL40X0_AMBIENT_AVERAGE_64,                 /**< Ambient average measurement 64 */
    VCNL40X0_AMBIENT_AVERAGE_128,                /**< Ambient average measurement 128 */
};

/**
 * @brief Ambient light measurement rate
 */
enum {
    VCNL40X0_AMBIENT_RATE_1 = 0,                 /**< Ambient measurement rate 1 */
    VCNL40X0_AMBIENT_RATE_2,                     /**< Ambient measurement rate 2 */
    VCNL40X0_AMBIENT_RATE_3,                     /**< Ambient measurement rate 3 */
    VCNL40X0_AMBIENT_RATE_4,                     /**< Ambient measurement rate 4 */
    VCNL40X0_AMBIENT_RATE_5,                     /**< Ambient measurement rate 5 */
    VCNL40X0_AMBIENT_RATE_6,                     /**< Ambient measurement rate 6 */
    VCNL40X0_AMBIENT_RATE_8,                     /**< Ambient measurement rate 8 */
    VCNL40X0_AMBIENT_RATE_10,                    /**< Ambient measurement rate 10 */
};


/**
 * @brief   Status and error return codes
 */
enum {
    VCNL40X0_OK = 0,                             /**< Everything was fine */
    VCNL40X0_ERR_I2C,                            /**< Error initializing the I2C bus */
    VCNL40X0_ERR_NODEV                           /**< Error wrong device */
};

/**
 * @brief Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                               /**< I2C device which is used */
    uint8_t i2c_addr;                            /**< Address on I2C bus */
    uint8_t led_current;                         /**< LED current */
    uint8_t proximity_rate;                      /**< Proximity rate */
    uint8_t ambient_avg;                         /**< Ambient light average */
    uint8_t ambient_rate;                        /**< Ambient light rate */
} vcnl40x0_params_t;

/**
 * @brief Device descriptor for the VCNL40X0 sensor
 */
typedef struct {
    vcnl40x0_params_t params;                    /**< Device parameters */
} vcnl40x0_t;

/**
 * @brief Initialize the given VCNL40X0 device
 *
 * @param[out] dev          Initialized device descriptor of VCNL40X0 device
 * @param[in]  params       The parameters for the VCNL40X0 device
 *
 * @return                  VCNL40X0_OK on success
 * @return                  -VCNL40X0_ERR_I2C if given I2C is not enabled in board config
 * @return                  -VCNL40X0_ERR_NODEV if not a vcnl40X0 device
 */
int vcnl40x0_init(vcnl40x0_t *dev, const vcnl40x0_params_t *params);

/**
 * @brief Read proximity value from the vcnl40X0 device
 *
 * @param[in] dev           Device descriptor of VCNL40X0 device to read from
 *
 * @return                  Proximity in counts
 */
uint16_t vcnl40x0_read_proximity(const vcnl40x0_t *dev);

/**
 * @brief Read ambient light value from the vcnl40X0 device
 *
 * @param[in] dev           Device descriptor of VCNL40X0 device to read from
 *
 * @return                  Ambient light in counts
 */
uint16_t vcnl40x0_read_ambient_light(const vcnl40x0_t *dev);

/**
 * @brief Read illuminance value from the vcnl40X0 device
 *
 * @param[in] dev           Device descriptor of VCNL40X0 device to read from
 *
 * @return                  Illuminance in lux
 */
uint16_t vcnl40x0_read_illuminance(const vcnl40x0_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* VCNL40X0_H */
/** @} */
