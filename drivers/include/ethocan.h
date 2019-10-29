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
 * @name    Escape octet definitions
 * @{
 */
#define ETHOCAN_OCTECT_END          (0xFF)   /**< magic octet indicating the end of frame */
#define ETHOCAN_OCTECT_ESC          (0xFE)   /**< magic octet escaping 0xFF in byte stream */
/** @} */

/**
 * @name    State definitions
 * @brief   The drivers internal state that is hold in ethocan_t.state
 * @{
 */
#define ETHOCAN_STATE_UNDEF         (0x00)   /**< initial state that will never be reentered */
#define ETHOCAN_STATE_BLOCKED       (0x01)   /**< the driver just listens to incoming frames and blocks outgress frames */
#define ETHOCAN_STATE_IDLE          (0x02)   /**< frames will be received or sent */
#define ETHOCAN_STATE_RECV          (0x03)   /**< currently receiving a frame */
#define ETHOCAN_STATE_SEND          (0x04)   /**< currently sending a frame */
#define ETHOCAN_STATE_INVALID       (0x05)   /**< invalid state used as boundary checking */
#define ETHOCAN_STATE_ANY           (0x0F)   /**< special state filter used internally to observe any state transition */
/** @} */

/**
 * @name    Signal definitions
 * @brief   A signal controls the state machine and may cause a state transistion
 * @{
 */
#define ETHOCAN_SIGNAL_INIT         (0x00)   /**< init the state machine */
#define ETHOCAN_SIGNAL_GPIO         (0x10)   /**< the sense GPIO detected a falling edge */
#define ETHOCAN_SIGNAL_UART         (0x20)   /**< an octet has been received */
#define ETHOCAN_SIGNAL_XTIMER       (0x30)   /**< the timer timed out */
#define ETHOCAN_SIGNAL_SEND         (0x40)   /**< enter send state */
#define ETHOCAN_SIGNAL_END          (0x50)   /**< leave send state */
/** @} */

/**
 * @name    Flag definitions
 * @brief   Hold in ethocan_t.flags
 * @{
 */
#define ETHOCAN_FLAG_RECV_BUF_DIRTY (0x01)   /**< the receive buffer contains a full unhandled frame */
#define ETHOCAN_FLAG_END_RECEIVED   (0x02)   /**< the end octet has been received */
#define ETHOCAN_FLAG_ESC_RECEIVED   (0x04)   /**< the esc octet has been received */
/** @} */

/**
 * @name    Opt definitions
 * @brief   Hold in ethocan_t.opts
 * @{
 */
#define ETHOCAN_OPT_PROMISCUOUS     (0x01)   /**< don't check the destination MAC - pass every frame to upper layers */
/** @} */

#ifndef ETHOCAN_TIMEOUT_USEC
#define ETHOCAN_TIMEOUT_USEC        (5000)   /**< timeout that brings the driver back into idle state if the remote side died within a transaction */
#endif

#define ETHOCAN_FRAME_CRC_LEN          (2)   /**< CRC16 is used */
#define ETHOCAN_FRAME_LEN (ETHERNET_FRAME_LEN + ETHOCAN_FRAME_CRC_LEN) /**< ethocan frame length */

/**
 * @brief   ethocan netdev device
 * @extends netdev_t
 */
typedef struct {
    netdev_t netdev;                        /**< extended netdev structure */
    uint8_t mac_addr[ETHERNET_ADDR_LEN];    /**< this device's MAC address */
    uint8_t opts;                           /**< driver options */
    uint8_t state;                          /**< current state of the driver's state machine */
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
#endif /* ETHOCAN_H */
/** @} */
