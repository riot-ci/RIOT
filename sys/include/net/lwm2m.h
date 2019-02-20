/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *               2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup     net_lwm2m LWM2M
 * @ingroup      net
 * @brief        Lightweight Machine to Machnine (LWM2M) definitions
 * @{
 *
 * @file
 * @brief       LWM2M header definitions
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */


#ifndef NET_LWM2M_H
#define NET_LWM2M_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LWM2M default parameters
 * @{
 */

/**
 * @brief CoAP default port of the LWM2M server
 */
#ifndef LWM2M_STANDARD_PORT
#define LWM2M_STANDARD_PORT "5683"
#endif

/**
 * @brief CoAPS default port of the LWM2M server
 */
#ifndef LWM2M_DTLS_PORT
#define LWM2M_DTLS_PORT     "5684"
#endif

/**
 * @brief CoAP default port of the LWM2M bootstrap server
 */
#ifndef LWM2M_BSSERVER_PORT
#define LWM2M_BSSERVER_PORT "5685"
#endif

/**
 * @brief Default port for the local LWM2M instance
 */
#ifndef LWM2M_LOCAL_PORT
#define LWM2M_LOCAL_PORT    "5683"
#endif

/**
 * @brief Device name used to register at the LWM2M server
 */
#ifndef LWM2M_DEVICE_NAME
#define LWM2M_DEVICE_NAME "testRIOTDevice"
#endif

/**
 * @brief Lifetime of the device object on the LWM2M server
 */
#ifndef LWM2M_DEVICE_TTL
#define LWM2M_DEVICE_TTL 300
#endif

/**
 * @brief LwM2M server URI to register/bootstrap with
 * 
 * @note The host part of the URI MUST be a valid IPv6 address. Host names can
 * not be resolved at this time.
 */
#ifndef LWM2M_SERVER_URI
#define LWM2M_SERVER_URI "coap://[]"
#endif

/**
 * @brief Numeric ID of LWM2M_SERVER_URI
 */
#ifndef LWM2M_SERVER_ID
#define LWM2M_SERVER_ID 10
#endif

/**
 * @brief Alternate path to place LWM2M resources
 */
#ifndef LWM2M_ALT_PATH
#define LWM2M_ALT_PATH  NULL
#endif

/**
 * @brief Specifies if the server at @ref LWM2M_SERVER_URI is a bootstrap server
 * or not.
 * 
 * Valid values:
 * - 0: Server is not bootstrap
 * - 1: Server is bootstrap
 */
#ifndef LWM2M_SERVER_IS_BOOTSTRAP
#define LWM2M_SERVER_IS_BOOTSTRAP 0
#endif

#if defined(MODULE_TINYDTLS) || defined(doxygen)

/*
 * DTLS debug log level. One of the following:
 *    DTLS_LOG_EMERG
 *    DTLS_LOG_ALERT
 *    DTLS_LOG_CRIT
 *    DTLS_LOG_WARN
 *    DTLS_LOG_NOTICE
 *    DTLS_LOG_INFO
 *    DTLS_LOG_DEBUG
 */
#ifndef DTLS_LOG_LEVEL
#define DTLS_LOG_LEVEL DTLS_LOG_CRIT
#endif

/*
 * PSK ID of LWM2M_SERVER_URI.
 */
#define LWM2M_SERVER_PSK_ID "Client_identity"

/*
 * PSK KEY of LWM2M_SERVER_URI in hexadecimal.
 * Set to NULL to not encrypt traffic.
 */
#define LWM2M_SERVER_PSK_KEY "73656372657450534B"

#endif /* MODULE_TINYDTLS */

/**
 * @brief Device object manufacturer string
 */
#ifndef LWM2M_DEVICE_MANUFACTURER
#define LWM2M_DEVICE_MANUFACTURER "A RIOT maker"
#endif

/**
 * @brief Device object model.
 * 
 * @note Defaults to the board name
 */
#ifndef LWM2M_DEVICE_MODEL
#define LWM2M_DEVICE_MODEL RIOT_BOARD
#endif

/**
 * @brief Device object serial number
 */
#ifndef LWM2M_DEVICE_SERIAL
#define LWM2M_DEVICE_SERIAL "undefined"
#endif

/**
 * @brief Device object firmware version
 * 
 * @note Defaults to the running RIOT version
 */
#ifndef LWM2M_DEVICE_FW_VERSION
#define LWM2M_DEVICE_FW_VERSION RIOT_VERSION
#endif

/**
 * @brief Device object binding and queue mode
 * 
 * Valid values are:
 * - U: UDP
 * - UQ: UDP with Queue mode
 * - S: SMS
 * - SQ: SMS with Queue mode
 * - US: UDP and SMS
 * - UQS: UDP with Queue mode and SMS 
 */
#ifndef LWM2M_DEVICE_BINDINGS
#define LWM2M_DEVICE_BINDINGS "U"
#endif

/**
 * @brief Device object device type
 */
#ifndef LWM2M_DEVICE_TYPE
#define LWM2M_DEVICE_TYPE "RIOT device"
#endif

/**
 * @brief Device object hardware version
 * 
 * @note Defaults to the board name
 */
#ifndef LWM2M_DEVICE_HW_VERSION
#define LWM2M_DEVICE_HW_VERSION RIOT_BOARD
#endif

/**
 * @brief Device object software version
 * 
 * @note Defaults to the running RIOT version
 */
#ifndef LWM2M_DEVICE_SW_VERSION
#define LWM2M_DEVICE_SW_VERSION RIOT_VERSION
#endif
/** @} */

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NET_LWM2M_H */
