/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_rn2xx3
 * @{
 *
 * @file
 * @brief       Default configuration for RN2483/RN2903 devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef RN2XX3_PARAMS_H
#define RN2XX3_PARAMS_H

#include "rn2xx3.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default LoRaMac configuration parameters
 * @{
 */
/**
 * @brief   Default device EUI
 */
#ifndef LORAMAC_DEV_EUI
#define LORAMAC_DEV_EUI           { 0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00 }
#endif

/**
 * @brief   Default application EUI
 */
#ifndef LORAMAC_APP_EUI
#define LORAMAC_APP_EUI           { 0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00 }
#endif

/**
 * @brief   Default application key
 */
#ifndef LORAMAC_APP_KEY
#define LORAMAC_APP_KEY           { 0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00 }
#endif

/**
 * @brief   Default application session key
 */
#ifndef LORAMAC_APP_SKEY
#define LORAMAC_APP_SKEY          { 0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00 }
#endif

/**
 * @brief   Default network session key
 */
#ifndef LORAMAC_NWK_SKEY
#define LORAMAC_NWK_SKEY          { 0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00, \
                                    0x00, 0x00, 0x00, 0x00 }
#endif

/**
 * @brief   Default device address
 */
#ifndef LORAMAC_DEV_ADDR
#define LORAMAC_DEV_ADDR          { 0x00, 0x00, 0x00, 0x00 }
#endif

/**
 * @brief   LoRaMAC params
 */
#define LORAMAC_PARAMS            { .dev_eui  = LORAMAC_DEV_EUI, \
                                    .app_eui  = LORAMAC_APP_EUI, \
                                    .app_key  = LORAMAC_APP_KEY, \
                                    .dev_addr = LORAMAC_DEV_ADDR }
/**@}*/

/**
 * @name    Set default configuration parameters for the RN2483/RN2903 driver
 * @{
 */
#ifndef RN2XX3_PARAM_UART
#define RN2XX3_PARAM_UART         (UART_DEV(1))
#endif
#ifndef RN2XX3_PARAM_BAUDRATE
#define RN2XX3_PARAM_BAUDRATE     (57600U)
#endif
#ifndef RN2XX3_PARAM_PIN_RESET
#define RN2XX3_PARAM_PIN_RESET    (GPIO_UNDEF)
#endif

#ifndef RN2XX3_PARAMS
#define RN2XX3_PARAMS             { .uart      = RN2XX3_PARAM_UART,      \
                                    .baudrate  = RN2XX3_PARAM_BAUDRATE,  \
                                    .pin_reset = RN2XX3_PARAM_PIN_RESET, \
                                    .loramac   = LORAMAC_PARAMS }
#endif
/**@}*/

/**
 * @brief   RN2483/RN2903 configuration
 */
static const rn2xx3_params_t rn2xx3_params[] =
{
    RN2XX3_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* RN2XX3_PARAMS_H */
/** @} */
