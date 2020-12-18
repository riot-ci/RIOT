/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_native
 * @{
 *
 * @file
 * @brief       EUI providers found on the board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef EUI_PROVIDER_PARAMS_H
#define EUI_PROVIDER_PARAMS_H

#include "net/eui64.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Get a command-line provided EUI-64
 *
 * @param index     index of ZEP device
 * @param addr[out] user supplied EUI-64
 *
 * @return 0 on success, negatvie if no more EUIs are available.
 */
int native_get_eui64(uint8_t index, eui64_t *addr);

/**
 * @name    EUI sources on the board
 *          EUI-64 can be provided with the -Z command line argument
 * @{
 */
#define EUI64_PROVIDER_FUNC   native_get_eui64
#define EUI64_PROVIDER_TYPE   NETDEV_ANY
#define EUI64_PROVIDER_INDEX  NETDEV_INDEX_ANY
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* EUI_PROVIDER_PARAMS_H */
/** @} */
