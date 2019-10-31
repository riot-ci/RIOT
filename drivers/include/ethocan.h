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
 * @brief       Driver for connecting RIOT devices using a single bus wire
 *
 * About
 * =====
 *
 * This driver enables RIOT nodes to communicate by Ethernet over (electrical)
 * CAN. This enables them to interact in an easy and cheap manner using a single
 * bus wire with very low hardware requirements: The used microcontrollers just
 * need to feature at least one UART and one GPIO that is able to raise
 * interrupts.
 *
 * Wiring
 * ======
 *
 * ![Ethocan wiring](ethocan-wiring.svg)
 *
 * For bus access, you need a CAN transceiver. Every transceiver operating with
 * the right voltage levels should do. (If you are on a 3.3V MCU, you could use
 * an IC such as the SN65HVD233.)
 *
 * Basically, UART TX and RX are connected to respective pins of the
 * transceiver. In addition , the RX pin is also connected to the sense GPIO.
 * It is used to detect bus allocation.
 *
 * How it works
 * ============
 *
 * Some technical details for those interested: The Ethernet frames are sent
 * onto the CAN bus using `uart_write()` while observing the received echo from
 * the bus. This way collisions are detected (received echo != transmitted
 * octet) and retransmissions are scheduled. The frames are appended with a
 * CRC16 to protect the system from transmission errors.
 *
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
#include "bitarithm.h"
#include "mutex.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Escape octet definitions
 * @{
 */
#define ETHOCAN_OCTECT_END          (0xFF)   /**< Magic octet indicating the end of frame */
#define ETHOCAN_OCTECT_ESC          (0xFE)   /**< Magic octet escaping 0xFF in byte stream */
/** @} */

/**
 * @name    State definitions
 * @brief   The drivers internal state that is hold in ethocan_t.state
 * @{
 */
#define ETHOCAN_STATE_UNDEF         (0x00)   /**< Initial state that will never be reentered */
#define ETHOCAN_STATE_BLOCKED       (0x01)   /**< The driver just listens to incoming frames and blocks outgress frames */
#define ETHOCAN_STATE_IDLE          (0x02)   /**< Frames will be received or sent */
#define ETHOCAN_STATE_RECV          (0x03)   /**< Currently receiving a frame */
#define ETHOCAN_STATE_SEND          (0x04)   /**< Currently sending a frame */
#define ETHOCAN_STATE_INVALID       (0x05)   /**< Invalid state used as boundary checking */
#define ETHOCAN_STATE_ANY           (0x0F)   /**< Special state filter used internally to observe any state transition */
/** @} */

/**
 * @name    Signal definitions
 * @brief   A signal controls the state machine and may cause a state transistion
 * @{
 */
#define ETHOCAN_SIGNAL_INIT         (0x00)   /**< Init the state machine */
#define ETHOCAN_SIGNAL_GPIO         (0x10)   /**< Sense GPIO detected a falling edge */
#define ETHOCAN_SIGNAL_UART         (0x20)   /**< Octet has been received */
#define ETHOCAN_SIGNAL_XTIMER       (0x30)   /**< Timer timed out */
#define ETHOCAN_SIGNAL_SEND         (0x40)   /**< Enter send state */
#define ETHOCAN_SIGNAL_END          (0x50)   /**< Leave send state */
/** @} */

/**
 * @name    Flag definitions
 * @brief   Hold in ethocan_t.flags
 * @{
 */
#define ETHOCAN_FLAG_RECV_BUF_DIRTY (BIT0)   /**< Receive buffer contains a complete unhandled frame */
#define ETHOCAN_FLAG_END_RECEIVED   (BIT1)   /**< END octet has been received */
#define ETHOCAN_FLAG_ESC_RECEIVED   (BIT2)   /**< ESC octet has been received */
/** @} */

/**
 * @name    Opt definitions
 * @brief   Hold in ethocan_t.opts
 * @{
 */
#define ETHOCAN_OPT_PROMISCUOUS     (BIT0)   /**< Don't check the destination MAC - pass every frame to upper layers */
/** @} */

#ifndef ETHOCAN_TIMEOUT_USEC
#define ETHOCAN_TIMEOUT_USEC        (5000)   /**< Timeout that brings the driver back into idle state if the remote side died within a transaction */
#endif

#define ETHOCAN_FRAME_CRC_LEN          (2)   /**< CRC16 is used */
#define ETHOCAN_FRAME_LEN (ETHERNET_FRAME_LEN + ETHOCAN_FRAME_CRC_LEN) /**< ethocan frame length */

/**
 * @brief   ethocan netdev device
 * @extends netdev_t
 */
typedef struct {
    netdev_t netdev;                        /**< Extended netdev structure */
    uint8_t mac_addr[ETHERNET_ADDR_LEN];    /**< This device's MAC address */
    uint8_t opts;                           /**< Driver options */
    uint8_t state;                          /**< Current state of the driver's state machine */
    mutex_t state_mtx;                      /**< Is unlocked every time a state is (re)entered */
    uint8_t flags;                          /**< Several flags */
    uint8_t recv_buf[ETHOCAN_FRAME_LEN];    /**< Receive buffer for incoming frames */
    size_t recv_buf_ptr;                    /**< Index of the next empty octet of the recveive buffer */
    uart_t uart;                            /**< UART device to use */
    uint8_t uart_octect;                    /**< Last received octet */
    gpio_t sense_pin;                       /**< GPIO to sense for start bits on the UART's rx line */
    xtimer_t timeout;                       /**< Timeout timer ensuring always to get back to IDLE state */
    uint32_t timeout_ticks;                 /**< Default amount of timeout ticks */
} ethocan_t;

/**
 * @brief   Struct containing the required configuration
 */
typedef struct {
    uart_t uart;                            /**< UART device to use */
    gpio_t sense_pin;                       /**< GPIO to sense for start bits on the UART's rx line */
    uint32_t baudrate;                      /**< Baudrate to UART device */
} ethocan_params_t;

/**
 * @brief   Setup an ethocan based device state.
 * @param[out]  dev         Handle of the device to initialize
 * @param[in]   params      Parameters for device initialization
 */
void ethocan_setup(ethocan_t *dev, const ethocan_params_t *params);

#ifdef __cplusplus
}
#endif
#endif /* ETHOCAN_H */
/** @} */
