/*
 * Copyright (C) 2019 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_cryptoauthlib cryptoauthlib security crypto
 * @{
 *
 * @file
 * @brief       Default configuration for Microchip CryptoAuth devices
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#ifndef ATCA_PARAMS_H
#define ATCA_PARAMS_H

#include "cryptoauthlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default GPIO pin for wake function.
 * 
 * @brief   If the device is asleep it needs to be woken up by driving the SDA pin
 *          low for some time and reinitializing the bus. The GPIO pin must be the
 *          same as the SDA pin of the I2C device.
 * 
 * @ingroup  config
 * @{
 */
#ifndef ATCA_GPIO_WAKE
#define ATCA_GPIO_WAKE (GPIO_PIN(0, 16))
#endif

/**
 * @name    Set default configuration parameters for the ATCA device
 *
 * @brief   The CryptoAuth library defines the data structure ATCAIfaceCfg for
 *          device initialization. We use this instead of a self defined params
 *          struct and store it in the params array.
 *          ATCAIfaceCfg contains a variable for the bus address, which is never
 *          used by the library. We use it to store Riot's I2C_DEV.
 *          We also initialize the baud rate with zero, because Riot doesn't use
 *          baud.
 *
 * @ingroup  config
 * @{
 */

#ifndef ATCA_PARAM_I2C
#define ATCA_PARAM_I2C           I2C_DEV(0)
#endif
#ifndef ATCA_PARAM_ADR
#define ATCA_PARAM_ADR          (ATCA_I2C_ADR)
#endif

#ifndef ATCA_PARAMS
#define ATCA_PARAMS                {    .iface_type             = ATCA_I2C_IFACE, \
                                        .devtype                = ATECC508A, \
                                        .atcai2c.slave_address  = ATCA_PARAM_ADR, \
                                        .atcai2c.bus            = ATCA_PARAM_I2C, \
                                        .atcai2c.baud           = 0, \
                                        .wake_delay             = 1500, \
                                        .rx_retries             = 20 }
#endif

/**@}*/

/**
 * @brief   Allocation of ATCA device descriptors
 */
static const ATCAIfaceCfg atca_params[] =
{
    ATCA_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* ATCA_PARAMS_H */
/** @} */
