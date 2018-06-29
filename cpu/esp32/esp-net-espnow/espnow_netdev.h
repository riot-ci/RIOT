/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_esp32_esp_net_espnow ESPNOW
 * @ingroup         cpu_esp32
 * @brief           WiFi based ESPNOW network device driver
 *
 * This module realizes a netdev interface using Espressif's ESPNOW
 * technology which uses the built-in WiFi module.
 */

/**
 * @ingroup     cpu_esp32_esp_net_espnow
 * @{
 *
 * @file
 * @brief       Netdev interface for the ESP-NOW WiFi P2P protocol
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESPNOW_NETDEV_H
#define ESPNOW_NETDEV_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum packet size that can be used with ESP-NOW
 */
#define ESPNOW_MAX_SIZE (250)

/**
 * @brief   Length of ESP-NOW addresses
 */
#define ESPNOW_ADDR_LEN ETHERNET_ADDR_LEN

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t espnow_driver;

/**
 * @brief   Device descriptor for ESP-NOW devices
 */
typedef struct
{
    netdev_t netdev;                 /**< netdev parent struct */

    uint8_t addr[ESPNOW_ADDR_LEN];   /**< device addr (MAC address) */

    uint8_t rx_len;                  /**< number of bytes received */
    uint8_t rx_buf[ESPNOW_MAX_SIZE]; /**< receive buffer */
    uint8_t rx_mac[ESPNOW_ADDR_LEN]; /**< source address */

    uint8_t tx_len;                  /**< number of bytes in transmit buffer */
    uint8_t tx_buf[ESPNOW_MAX_SIZE]; /**< transmit buffer */

    gnrc_netif_t* netif;             /**< reference to the corresponding netif */

    uint8_t peers_all;               /**< number of peers reachable */
    uint8_t peers_enc;               /**< number of encrypted peers */

    mutex_t dev_lock;                /**< device is already in use */

} espnow_netdev_t;

#ifdef __cplusplus
}
#endif

#endif /* ESPNOW_NETDEV_H */
/** @} */
