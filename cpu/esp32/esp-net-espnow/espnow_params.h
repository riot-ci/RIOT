/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_net_espnow
 * @{
 *
 * @file
 * @brief       Parameters for the netdev interface for ESP-NOW WiFi P2P
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESPNOW_PARAMS_H
#define ESPNOW_PARAMS_H

#if defined(MODULE_ESP_NET_ESPNOW) || defined(DOXYGEN)

/**
 * @name    Set default configuration parameters for the ESP-NOW netdev driver
 * @{
 */
#ifndef ESPNOW_STACKSIZE
/** The size of the stack used for the ESP-NOW netdev driver thread */
#define ESPNOW_STACKSIZE    THREAD_STACKSIZE_DEFAULT
#endif

#ifndef ESPNOW_PRIO
/** The priority of the ESP-NOW netdev driver thread */
#define ESPNOW_PRIO         GNRC_NETIF_PRIO
#endif

#ifndef ESPNOW_SCAN_PERIOD
/** Period at which the node scans for other nodes in its range */
#define ESPNOW_SCAN_PERIOD  (10000000UL)
#endif

#ifndef ESPNOW_SOFT_AP_PASSPHRASE
/** Passphrase (max. 64 chars) used for the SoftAP interface of the nodes */
#define ESPNOW_SOFT_AP_PASSPHRASE "ThisistheRIOTporttoESP"
#endif

#ifndef ESPNOW_CHANNEL
/** Channel used as broadcast medium by all ESP-NOW nodes together */
#define ESPNOW_CHANNEL 6
#endif

#ifndef ESPNOW_KEY
/**
 * @brief   Key used for the communication between ESP-NOW nodes
 *
 * The key has to be defined to enable encrypted communication between ESP-NOW
 * nodes. The key has to be of type *uint8_t [16]* and has to be exactly
 * 16 bytes long. If the key is NULL (the default) communication is not
 * encrypted.
 *
 * Please note: If encrypted communication is used, a maximum of 6 nodes can
 * communicate with each other, while in unencrypted mode, up to 20 nodes can
 * communicate.
 */
#define ESPNOW_KEY      NULL
#endif

#ifndef ESPNOW_PARAMS
#define ESPNOW_PARAMS   { .key = ESPNOW_KEY, \
                          .scan_period = ESPNOW_SCAN_PERIOD, \
                          .softap_pass = ESPNOW_SOFT_AP_PASSPHRASE, \
                          .channel = ESPNOW_CHANNEL }
#endif

/**@}*/

/**
 * @brief   struct holding all params needed for device initialization
 */
typedef struct
{
    uint8_t* key;         /**< key of type uint8_t [16] or NULL (no encryption) */
    uint32_t scan_period; /**< Period at which the node scans for other nodes */
    char*    softap_pass; /**< Passphrase used for the SoftAP interface */
    uint8_t  channel;     /**< Channel used for ESP-NOW nodes */

} espnow_params_t;

#ifdef __cplusplus
extern "C" {
#endif

static const espnow_params_t espnow_params = ESPNOW_PARAMS;

#ifdef __cplusplus
}
#endif

#endif /* MODULE_ESP_NET_ESPNOW || DOXYGEN */

#endif /* ESPNOW_PARAMS_H */
