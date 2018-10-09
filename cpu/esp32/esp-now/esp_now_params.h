/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_now
 * @{
 *
 * @file
 * @brief       Parameters for the netdev interface for ESP-NOW WiFi P2P
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESP_NOW_PARAMS_H
#define ESP_NOW_PARAMS_H

#if defined(MODULE_ESP_NOW) || defined(DOXYGEN)

/**
 * @name    Set default configuration parameters for the ESP-NOW netdev driver
 * @{
 */
#ifndef ESP_NOW_STACKSIZE
/** The size of the stack used for the ESP-NOW netdev driver thread */
#define ESP_NOW_STACKSIZE       THREAD_STACKSIZE_DEFAULT
#endif

#ifndef ESP_NOW_PRIO
/** The priority of the ESP-NOW netdev driver thread */
#define ESP_NOW_PRIO            GNRC_NETIF_PRIO
#endif

#ifndef ESP_NOW_SCAN_PERIOD
/** Period in us at which the node scans for other nodes in its range */
#define ESP_NOW_SCAN_PERIOD     (10000000UL)
#endif

#ifndef ESP_NOW_SOFT_AP_PASS
/** Passphrase (max. 64 chars) used for the SoftAP interface of the nodes */
#define ESP_NOW_SOFT_AP_PASS    "ThisistheRIOTporttoESP"
#endif

#ifndef ESP_NOW_CHANNEL
/** Channel used as broadcast medium by all ESP-NOW nodes together */
#define ESP_NOW_CHANNEL         (6)
#endif

#ifndef ESP_NOW_KEY
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
#define ESP_NOW_KEY             (NULL)
#endif

#ifndef ESP_NOW_PARAMS
#define ESP_NOW_PARAMS   { .key = ESP_NOW_KEY, \
                           .scan_period = ESP_NOW_SCAN_PERIOD, \
                           .softap_pass = ESP_NOW_SOFT_AP_PASS, \
                           .channel = ESP_NOW_CHANNEL }
#endif

/**
 * @brief   struct holding all params needed for device initialization
 */
typedef struct
{
    uint8_t* key;         /**< key of type uint8_t [16] or NULL (no encryption) */
    uint32_t scan_period; /**< Period at which the node scans for other nodes */
    char*    softap_pass; /**< Passphrase used for the SoftAP interface */
    uint8_t  channel;     /**< Channel used for ESP-NOW nodes */

} esp_now_params_t;

#ifdef __cplusplus
extern "C" {
#endif

static const esp_now_params_t esp_now_params = ESP_NOW_PARAMS;

#ifdef __cplusplus
}
#endif

#endif /* MODULE_ESP_NOW || DOXYGEN */

#endif /* ESP_NOW_PARAMS_H */
/**@}*/
