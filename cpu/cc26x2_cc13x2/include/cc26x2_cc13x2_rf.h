/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @ingroup         drivers_netdev
 * @brief           Driver for using the CC13x2's radio in IEEE802.15.4 mode.
 *
 * ## Implementation state ##
 *
 * Netdev events supported:
 *
 * - #NETDEV_EVENT_RX_COMPLETE.
 * - #NETDEV_EVENT_TX_COMPLETE.
 *
 * Transmission options not yet implemented:
 * - Send acknowledgement for frames (only available using IEEE 802.15.4g HAL).
 * - Request acknowledgement.
 * - Retransmit unacked packages (only available using when `ieee802154_hal` and
 *   `ieee802154_submac`).
 * - Carrier Sense Multiple Access (CSMA) and Implementation of Clear Channel
 *   Assessment Control (CCACTRL), (only available using `ieee802154_hal` and
 *   `ieee802154_submac`).
 *
 * @{
 *
 * @file
 * @brief           Driver interface for using the CC13x2 in IEEE 802.15.4g mode.
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26X2_CC13X2_RF_H
#define CC26X2_CC13X2_RF_H

#if !IS_USED(MODULE_IEEE802154_RADIO_HAL)
#include "net/netdev/ieee802154.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   IEEE 802.15.4 radio timer configuration
 *
 * This radio relies on a dedicated hardware timer to maintain IFS,
 * the default timer may be overwritten in the board configuration.
 */
#ifndef CC26X2_CC13X2_RF_TIMER
#define CC26X2_CC13X2_RF_TIMER TIMER_DEV(4)
#endif

#if !IS_USED(MODULE_IEEE802154_RADIO_HAL)
/**
 * @brief   CC13x2 netdev IEEE 802.15.4g device descriptor
 */
extern netdev_ieee802154_t cc26x2_cc13x2_rf_dev;
#endif

/**
 * @brief   Initialize radio hardware (RF Core)
 */
void cc26x2_cc13x2_rf_init(void);

#if !IS_USED(MODULE_IEEE802154_RADIO_HAL)
/**
 * @brief   Setup the netdev interface.
 *
 * @pre     (@p dev != NULL)
 *
 * @param[in] dev The CC13x2 netdev device.
 */
void cc26x2_cc13x2_rf_setup(netdev_ieee802154_t *dev);
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_RF_H */

/*@}*/
