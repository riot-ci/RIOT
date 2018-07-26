/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_net_espwifi
 * @{
 *
 * @file
 * @brief       Parameters for the netdev interface for ESP WiFi module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESPWIFI_PARAMS_H
#define ESPWIFI_PARAMS_H

#if defined(MODULE_ESP_NET_ESPWIFI) || defined(DOXYGEN)

/**
 * @name    Set default configuration parameters for the ESP WiFi netdev driver
 * @{
 */
#ifndef ESPWIFI_STACKSIZE
/** The size of the stack used for the ESP WiFi netdev driver thread */
#define ESPWIFI_STACKSIZE    THREAD_STACKSIZE_DEFAULT
#endif

#ifndef ESPWIFI_PRIO
/** The priority of the ESP WiFi netdev driver thread */
#define ESPWIFI_PRIO         GNRC_NETIF_PRIO
#endif

/**@}*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* MODULE_ESP_NET_ESPWIFI || DOXYGEN */

#endif /* ESPWIFI_PARAMS_H */
