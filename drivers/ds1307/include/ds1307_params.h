/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_ds1307
 * @{
 *
 * @file
 * @brief   Default configuration for DS1307 devices
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef DS1307_PARAMS_H
#define DS1307_PARAMS_H

#include "board.h"
#include "ds1307.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for the DS1307 driver
 * @{
 */
#ifndef DS1307_PARAM_I2C
#define DS1307_PARAM_I2C        (I2C_DEV(0))
#endif
#ifndef DS1307_PARAM_ADDR
#define DS1307_PARAM_ADDR       (DS1307_I2C_ADDRESS)
#endif
#ifndef DS1307_PARAM_I2C_CLK
#define DS1307_PARAM_I2C_CLK    (DS1307_I2C_MAX_CLK)

#define DS1307_PARAMS_DEFAULT   {
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* DS1307_PARAMS_H */
/** @} */
