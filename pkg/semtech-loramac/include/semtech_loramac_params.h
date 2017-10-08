/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         pkg_loramac-semtech
 * @brief           Default definitions for Semtech LoRaMAC package
 * @{
 *
 * @file
 * @brief           Default definitions for Semtech LoRaMAC package
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef SEMTECH_LORAMAC_PARAMS_H
#define SEMTECH_LORAMAC_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default device EUI
 *
 *          8 bytes key, required for join procedure
 */
#define LORAMAC_DEV_EUI_DEFAULT                     { 0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00 }

/**
 * @brief   Default application EUI
 *
 *          8 bytes key, required for join procedure
 */
#define LORAMAC_APP_EUI_DEFAULT                     { 0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00 }

/**
 * @brief   Default application key
 *
 *          16 bytes key, required for join procedure
 */
#define LORAMAC_APP_KEY_DEFAULT                     { 0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00 }

/**
 * @brief   Default device address
 */
#define LORAMAC_DEV_ADDR_DEFAULT                    { 0x00, 0x00, 0x00, 0x00 }

/**
 * @brief   Default network session key
 *
 *          16 bytes key, only required for ABP join procedure type.
 */
#define LORAMAC_NET_SKEY_DEFAULT                    { 0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00 }

/**
 * @brief   Default application session key
 *
 *          16 bytes key, only required for ABP join procedure type
 */
#define LORAMAC_APP_SKEY_DEFAULT                    { 0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00, \
                                                      0x00, 0x00, 0x00, 0x00 }

#ifdef __cplusplus
}
#endif

#endif /* SEMTECH_LORAMAC_PARAMS_H */
/** @} */
