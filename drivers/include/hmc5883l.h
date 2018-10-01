/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_hmc5883l HMC5883L 3-axis digital compass
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the Honeywell HMC5883L 3-axis digital compass
 *
 * The driver implements basic polling mode. The application can use the
 * different approaches to get new data, either
 *
 * - using the #hmc5883l_read function at a lower rate than the the DOR, or
 * - using the data-ready interrupt (**DRDY**).
 *
 * The data-ready interrupt (**DRDY**) of the sensor is enabled permanently. The
 * application has only to configure and initialize the GPIO to which the
 * interrupt signal is connected.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifndef HMC5883L_H
#define HMC5883L_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

/** HMC5883L I2C address*/
#define HMC5883L_I2C_ADDRESS    (0x1e)

/** HMC5883L chip id defined in Identification Registers A..C */
#define HMC5883L_ID             { 0x48, 0x34, 0x33 }

/** HMC5883L chip id length */
#define HMC5883L_ID_LEN         (3)

/** Definition of error codes */
typedef enum {
    HMC5883L_OK,                   /**< success */
    HMC5883L_ERROR_I2C,            /**< I2C communication error */
    HMC5883L_ERROR_WRONG_ID,       /**< wrong id read */
    HMC5883L_ERROR_NO_DATA,        /**< no data are available */
    HMC5883L_ERROR_RAW_DATA,       /**< reading raw data failed */
} hmc5883l_error_codes_t;

/**
 * @brief   Data output rates (DOR)
 */
typedef enum {

    HMC5883L_DOR_0_75 = 0,  /**< 0.75 Hz           */
    HMC5883L_DOR_1_5,       /**<  1.5 Hz           */
    HMC5883L_DOR_3,         /**<    3 Hz           */
    HMC5883L_DOR_7_5,       /**<  7.5 Hz           */
    HMC5883L_DOR_15,        /**<   15 Hz (default) */
    HMC5883L_DOR_30,        /**<   30 Hz           */
    HMC5883L_DOR_75,        /**<   75 Hz           */

} hmc5883l_dor_t;

/**
 * @brief   Measurement modes
 *
 */
typedef enum {

    HMC5883L_MEAS_MODE_NORMAL = 0, /**< Normal measurement config */
    HMC5883L_MEAS_MODE_BIAS_POS,   /**< Positive bias config for all axes */
    HMC5883L_MEAS_MODE_BIAS_NEG,   /**< Negative bias config for all axes */

} hmc5883l_meas_mode_t;

/**
 * @brief   Measurment avaraging (number of samples are averaged for output)
 *
 */
typedef enum {

    HMC5883L_MEAS_AVG_NONE = 0, /**< No averaging */
    HMC5883L_MEAS_AVG_2,        /**< 2 samples are averaged */
    HMC5883L_MEAS_AVG_4,        /**< 4 samples are averaged */
    HMC5883L_MEAS_AVG_8,        /**< 8 samples are averaged */

} hmc5883l_meas_avg_t;

/**
 * @brief   Operation modes
 *
 */
typedef enum {

    HMC5883L_OP_MODE_CONTINUOUS = 0,    /**< Continuous measurement */
    HMC5883L_OP_MODE_SINGLE,            /**< Single measurement */
    HMC5883L_OP_MODE_IDLE,              /**< Idle mode */

} hmc5883l_op_mode_t;

/**
 * @brief   Gain (determine the sensitivity and the range)
 */
typedef enum {
    HMC5883L_GAIN_1370 = 0, /**< Range +-0.88 Gs, Resolution 0.73 mG/LSb */
    HMC5883L_GAIN_1090,     /**< Range  +-1.3 Gs, Resolution 0.92 mG/LSb */
    HMC5883L_GAIN_820,      /**< Range  +-1.9 Gs, Resolution 1.22 mG/LSb */
    HMC5883L_GAIN_660,      /**< Range  +-2.5 Gs, Resolution 1.52 mG/LSb */
    HMC5883L_GAIN_440,      /**< Range  +-4.0 Gs, Resolution 2.27 mG/LSb */
    HMC5883L_GAIN_390,      /**< Range  +-4.7 Gs, Resolution 2.56 mG/LSb */
    HMC5883L_GAIN_330,      /**< Range  +-5.6 Gs, Resolution 3.03 mG/LSb */
    HMC5883L_GAIN_230,      /**< Range  +-8.1 Gs, Resolution 4.35 mG/LSb */
} hmc5883l_gain_t;

/**
 * @brief   Magnetic field values in micro-Gauss (uGs)
 */
typedef struct {

    int32_t x;  /**< magnetic field x-axis */
    int32_t y;  /**< magnetic field y-axis */
    int32_t z;  /**< magnetic field y-axis */

} hmc5883l_data_t;

/**
 * @brief   Raw data set as two complements
 */
typedef struct {

    int16_t x; /**< magnetic field x-axis as 16 bit two's complements */
    int16_t y; /**< magnetic field y-axis as 16 bit two's complements */
    int16_t z; /**< magnetic field z-axis as 16 bit two's complements */

} hmc5883l_raw_data_t;

/**
 * @brief   HMC5883L device initialization parameters
 */
typedef struct {

    unsigned  dev;   /**< I2C device */
    uint8_t   addr;  /**< I2C slave address */

    gpio_t    drdy;  /**< DRDY interrupt pin: #GPIO_UNDEF if DRDY is not used */

    hmc5883l_meas_mode_t meas_mode; /**< Measurement mode (default #HMC5883L_MEAS_MODE_NORMAL) */
    hmc5883l_meas_avg_t  meas_avg;  /**< Measurement avaraging (default #HMC5883L_MEAS_AVG_NONE) */
    hmc5883l_dor_t       dor;       /**< Data output rate (default #HMC5883L_DOR_15) */
    hmc5883l_op_mode_t   op_mode;   /**< Operation mode (#HMC5883L_OP_MODE_CONTINUOUS) */
    hmc5883l_gain_t      gain;      /**< Gain (default #HMC5883L_GAIN_1090) */

} hmc5883l_params_t;

/**
 * @brief   HMC5883L sensor device data structure type
 */
typedef struct {

    hmc5883l_params_t params;    /**< device initialization parameters */

} hmc5883l_t;

/**
 * @brief	Initialize the HMC5883L sensor device
 *
 * This function resets the sensor and initializes the sensor according to
 * given intialization parameters. All registers are reset to default values.
 *
 * @param[in]   dev     device descriptor of HMC5883L sensor to be initialized
 * @param[in]   params  HMC5883L initialization parameters
 *
 * @retval  HMC5883L_OK             on success
 * @retval  HMC5883L_ERROR_*        a negative error code on error,
 *                                  see #hmc5883l_error_codes_t
 */
int hmc5883l_init(hmc5883l_t *dev, const hmc5883l_params_t *params);

/**
 * @brief    Data-ready status function
 *
 * The function checks the status register and returns
 *
 * @param[in]   dev     device descriptor of HMC5883L sensor
 *
 * @retval  HMC5883L_OK             new data available
 * @retval  HMC5883L_ERROR_NO_DATA  no new data available
 * @retval  HMC5883L_ERROR_*        negative error code,
 *                                  see #hmc5883l_error_codes_t
 */
int hmc5883l_data_ready(const hmc5883l_t *dev);

/**
 * @brief   Read one sample of magnetic field values in micro-Gauss (uGs)
 *
 * Raw magnetometer data are read from the sensor and normalized them
 * with respect to configured gain. Magnetic field values are given in
 * micro-Gauss (uGs) to preserve full resolution:
 *
 * @param[in]   dev     device descriptor of HMC5883L sensor
 * @param[out]  data    result vector in micro-Gauss (uGs) per axis
 *
 * @retval  HMC5883L_OK         on success
 * @retval  HMC5883L_ERROR_*    a negative error code on error,
 *                              see #hmc5883l_error_codes_t
 */
int hmc5883l_read(const hmc5883l_t *dev, hmc5883l_data_t *data);

/**
 * @brief   Read one sample of raw sensor data as 16 bit two's complements
 *
 * @param[in]   dev     device descriptor of HMC5883L sensor
 * @param       raw     raw data vector
 *
 * @retval  HMC5883L_OK         on success
 * @retval  HMC5883L_ERROR_*    a negative error code on error,
 *                              see #hmc5883l_error_codes_t
 */
int hmc5883l_read_raw(const hmc5883l_t *dev, hmc5883l_raw_data_t *raw);

/**
 * @brief   Power down the sensor
 *
 * Changes the sensor operation mode to #HMC5883L_OP_MODE_IDLE in which
 * almost all internal blocks are switched off. I2C interface is
 * still active. The content of the configuration registers is preserved.
 *
 * @param[in]   dev     Device descriptor of HMC5883L device to read from
 *
 * @retval  HMC5883L_OK      on success
 * @retval  HMC5883L_ERROR_* negative error code, see #hmc5883l_error_codes_t
 */
int hmc5883l_power_down (hmc5883l_t *dev);

/**
 * @brief   Power up the sensor
 *
 * Swichtes the sensor back into the last active operation mode.
 *
 * @param[in]   dev     Device descriptor of HMC5883L device to read from
 *
 * @retval  HMC5883L_OK      on success
 * @retval  HMC5883L_ERROR_* negative error code, see #hmc5883l_error_codes_t
 */
int hmc5883l_power_up (hmc5883l_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* HMC5883L_H */
/** @} */
