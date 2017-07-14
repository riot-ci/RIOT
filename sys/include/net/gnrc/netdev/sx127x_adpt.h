/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netdev
 * @{
 *
 * @file
 * @brief       GNRC to SX1272/SX1276 netdev glue code interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef NET_GNRC_NETDEV_SX127X_ADPT_H
#define NET_GNRC_NETDEV_SX127X_ADPT_H

#include "sx127x.h"
#include "net/gnrc/netdev.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize GNRC handler for netdev SX1272/SX1276 devices
 *
 * @param[out] gnrc_netdev     gnrc_netdev struct to initialize
 * @param[in]  dev             SX1272/SX1276 device to handle
 */
void gnrc_netdev_sx127x_init(gnrc_netdev_t *gnrc_netdev, sx127x_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETDEV_SX127X_ADPT_H */
/** @} */
