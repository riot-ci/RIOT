/*
 * Copyright (C) 2019 Juergen Fitschen <me@jue.yt>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_ethocan Ethernet-over-CAN driver
 * @ingroup     drivers_netdev
 * @brief       Driver for the Ethernet-over-CAN module
 * @{
 *
 * @file
 * @brief       Driver for the Ethernet-over-CAN module
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#ifndef ETHOCAN_H
#define ETHOCAN_H

#include "periph/uart.h"
#include "periph/gpio.h"
#include "net/netdev.h"
#include "net/ethernet.h"
#include "mutex.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Escape char definitions
 * @{
 */
#define ETHOCAN_OCTECT_END                  (0xFF)
#define ETHOCAN_OCTECT_ESC                  (0xFE)
/** @} */

/**
 * @name    State definitions
 * @{
 */
#define ETHOCAN_STATE_UNDEF   (0)
#define ETHOCAN_STATE_BLOCKED (1)
#define ETHOCAN_STATE_IDLE    (2)
#define ETHOCAN_STATE_RECV    (3)
#define ETHOCAN_STATE_SEND    (4)
#define ETHOCAN_STATE_ANY     (255)
/** @} */

/**
 * @name    Signal definitions
 * @brief   A signal controls the state machine and may cause a state transistion.
 * @{
 */
#define ETHOCAN_SIGNAL_INIT      (0)
#define ETHOCAN_SIGNAL_GPIO      (1)
#define ETHOCAN_SIGNAL_UART      (2)
#define ETHOCAN_SIGNAL_XTIMER    (3)
#define ETHOCAN_SIGNAL_SEND      (4)
#define ETHOCAN_SIGNAL_END       (5)
/** @} */

/**
 * @name    Flag definitions
 * @{
 */
#define ETHOCAN_FLAG_RECV_BUF_DIRTY    (0b00000001)
#define ETHOCAN_FLAG_END_RECEIVED      (0b00000010)
#define ETHOCAN_FLAG_ESC_RECEIVED      (0b00000100)
/** @} */

#ifndef ETHOCAN_TIMEOUT_USEC
#define ETHOCAN_TIMEOUT_USEC (5000)
#endif

#define ETHOCAN_FRAME_LEN (ETHERNET_FRAME_LEN + 2)

/**
 * @brief   ethocan netdev device
 * @extends netdev_t
 */
typedef struct {
    netdev_t netdev;                        /**< extended netdev structure */
    uint8_t mac_addr[ETHERNET_ADDR_LEN];    /**< this device's MAC address */
    uint8_t state;                          /**< hold the current state of the driver's state machine */
    mutex_t state_mtx;                      /**< is unlocked everytime a state is (re)entered */
    uint8_t flags;                          /**< several flags */
    uint8_t recv_buf[ETHOCAN_FRAME_LEN];    /**< receive buffer for incoming frames */
    size_t recv_buf_ptr;                    /**< index of the next empty octet of the recveive buffer */
    uart_t uart;                            /**< UART device the to use */
    uint8_t uart_octect;                    /**< the last received octet */
    gpio_t sense_pin;                       /**< gpio to sense for start bits on the UART's rx line */
    xtimer_t timeout;                       /**< timeout timer ensuring always to get back to IDLE state */
    uint32_t timeout_ticks;                 /**< default amount of timeout ticks */
} ethocan_t;

/**
 * @brief   Struct containing the needed configuration
 */
typedef struct {
    uart_t uart;                            /**< UART device the to use */
    gpio_t sense_pin;                       /**< gpio to sense for start bits on the UART's rx line */
    uint32_t baudrate;                      /**< baudrate to UART device */
} ethocan_params_t;

/**
 * @brief   Setup an ethocan based device state.
 * @param[out]  dev         handle of the device to initialize
 * @param[in]   params      parameters for device initialization
 */
void ethocan_setup(ethocan_t *dev, const ethocan_params_t *params);

#ifdef __cplusplus
}
#endif
#endif
