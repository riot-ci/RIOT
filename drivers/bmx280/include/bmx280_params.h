/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bmx280
 *
 * @{
 * @file
 * @brief       Default configuration for BMX280
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BMX280_PARAMS_H
#define BMX280_PARAMS_H

#include "bmx280.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the BMX280
 * @{
 */
#ifndef BMX280_PARAM_I2C_DEV
#define BMX280_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef BMX280_PARAM_I2C_ADDR
#define BMX280_PARAM_I2C_ADDR        (0x77)
#endif

/* Defaults for Weather Monitoring */
#define BMX280_PARAMS_DEFAULT              \
    {                                      \
        .i2c_dev = BMX280_PARAM_I2C_DEV,   \
        .i2c_addr = BMX280_PARAM_I2C_ADDR, \
        .t_sb = BMX280_SB_0_5,             \
        .filter = BMX280_FILTER_OFF,       \
        .run_mode = BMX280_MODE_FORCED,     \
        .temp_oversample = BMX280_OSRS_X1,  \
        .press_oversample = BMX280_OSRS_X1, \
        .humid_oversample = BMX280_OSRS_X1, \
    }
/**@}*/

/**
 * @brief   Configure BMX280
 */
static const bmx280_params_t bmx280_params[] =
{
#ifdef BMX280_PARAMS_BOARD
    BMX280_PARAMS_BOARD,
#else
    BMX280_PARAMS_DEFAULT
#endif
};

/**
 * @brief   The number of configured sensors
 */
#define BMX280_NUMOF    (sizeof(bmx280_params) / sizeof(bmx280_params[0]))

/**
 * @brief   Configuration details of SAUL registry entries
 *
 * This two dimensional array contains static details of the sensors
 * for each device. Please be awar that the indexes are used in
 * auto_init_bmx280, so make sure the indexes match.
 */
static const saul_reg_info_t bmx280_saul_reg_info[BMX280_NUMOF] =
{
#if defined(MODULE_BME280)
        { .name = "bme280" }
#else
        { .name = "bmp280" }
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* BMX280_PARAMS_H */
/** @} */
