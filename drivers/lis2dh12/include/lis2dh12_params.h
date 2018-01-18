/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lis2dh12
 *
 * @{
 * @file
 * @brief       Default configuration for LIS2DH12 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LIS2DH12_PARAMS_H
#define LIS2DH12_PARAMS_H

#include "board.h"
#include "lis2dh12.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for LIS2DH12 devices
 * @{
 */
#ifndef LIS2DH12_PARAM_SPI
#define LIS2DH12_PARAM_SPI          SPI_DEV(0)
#endif
#ifndef LIS2DH12_PARAM_CS
#define LIS2DH12_PARAM_CS           GPIO_PIN(0, 0)
#endif
#ifndef LIS2DH12_PARAM_SCALE
#define LIS2DH12_PARAM_SCALE        LIS2DH12_SCALE_2G
#endif
#ifndef LIS2DH12_PARAM_RATE
#define LIS2DH12_PARAM_RATE         LIS2DH12_RATE_100HZ
#endif

#define LIS2DH12_PARAMS_DEFAULT     { .spi = LIS2DH12_PARAM_SPI,     \
                                      .cs = LIS2DH12_PARAM_CS,       \
                                      .scale = LIS2DH12_PARAM_SCALE, \
                                      .rate  = LIS2DH12_PARAM_RATE }
/**@}*/

/**
 * @brief   LIS2DH12 configuration
 */
static const lis2dh12_params_t lis2dh12_params[] =
{
#ifdef LIS2DH12_PARAMS_BOARD
    LIS2DH12_PARAMS_BOARD,
#else
    LIS2DH12_PARAMS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* LIS2DH12_PARAMS_H */
/** @} */
