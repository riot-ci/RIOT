/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC13x2 IEEE 802.15.4 netdev driver
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC13X2_PROP_RF_NETDEV_H
#define CC13X2_PROP_RF_NETDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/netdev/ieee802154.h"

extern netdev_driver_t cc13x2_prop_rf_driver; /**< CC13x2 netdev driver*/

/**
 * @brief   IEEE 802.15.4 Sub-GHz netdev device.
 */
typedef struct {
    netdev_ieee802154_t netdev; /**< IEEE 802.15.4 network device */
} cc13x2_prop_rf_netdev_t;

/**
 * @brief   Setup the netdev interface.
 *
 * @pre     (@p dev != NULL)
 *
 * @param[in] dev The CC13x2 netdev device.
 */
void cc13x2_prop_rf_setup(cc13x2_prop_rf_netdev_t *dev);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC13X2_PROP_RF_NETDEV_H */

/*@}*/
