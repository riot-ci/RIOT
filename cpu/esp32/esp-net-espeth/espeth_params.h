/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_net_espeth
 * @{
 *
 * @file
 * @brief       Parameters for the netdev interface for ESP32 Ethernet MAC module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESPETH_PARAMS_H
#define ESPETH_PARAMS_H

#if defined(MODULE_ESP_NET_ESPETH) || defined(DOXYGEN)

/**
 * @name    Set default configuration parameters for the ESP-ETH netdev driver
 * @{
 */
#ifndef ESPETH_STACKSIZE
/** The size of the stack used for the ESP-ETH netdev driver thread */
#define ESPETH_STACKSIZE    THREAD_STACKSIZE_DEFAULT
#endif

#ifndef ESPETH_PRIO
/** The priority of the ESP-ETH netdev driver thread */
#define ESPETH_PRIO         GNRC_NETIF_PRIO
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* MODULE_ESP_NET_ESPETH || DOXYGEN */

#endif /* ESPETH_PARAMS_H */
