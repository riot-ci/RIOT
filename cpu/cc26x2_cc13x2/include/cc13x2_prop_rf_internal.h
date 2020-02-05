/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC13x2 IEEE 802.15.4 netdev driver
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC13X2_PROP_RF_INTERNAL_H
#define CC13X2_PROP_RF_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "net/ieee802154.h"
#include "net/netdev/ieee802154.h"

/**
 * @brief This enum represents the state of a radio.
 *
 * Initially, a radio is in the Disabled state.
 *
 * The following are valid radio state transitions for the cc13x2:
 *
 *                                    (Radio ON)
 *  +----------+      On()  +-------+  Receive()   +---------+   Transmit()   +----------+
 *  |          |----------->|       |------------->|         |--------------->|          |
 *  |   Off    |            | Sleep |              | Receive |                | Transmit |
 *  |          |<-----------|       |<-------------|         |<---------------|          |
 *  +----------+      Off() |       |   Sleep()    |         |                +----------+
 *                          |       | (Radio OFF)  +---------+
 *                          +-------+
 *
 * | State            | Description                                        |
 * |------------------|----------------------------------------------------|
 * | Off              | The rfcore powerdomain is off and the RFCPE is off |
 * | Sleep            | The RFCORE PD is on, and the RFCPE is in PROP mode |
 * | Receive          | The RFCPE is running a CMD_PROP_RX_ADV             |
 * | Transmit         | The RFCPE is running a transmit command string     |
 */
typedef enum {
    FSM_STATE_OFF = 0, /**< Disabled, radio powered off */
    FSM_STATE_SLEEP, /**< Sleep state, awaiting for actions */
    FSM_STATE_RX, /**< Receive state */
    FSM_STATE_TX, /**< Transmitting packets */
} cc13x2_prop_rf_state_t;

/**
 * @brief   Initialize the radio internal structures.
 *
 * @note    Must be called before @ref cc13x2_prop_rf_power_on.
 */
void cc13x2_prop_rf_init(void);

/**
 * @brief   Power on the RF Core and send the setup command.
 *
 * @return  0 on success
 * @return  -1 on failure
 */
int_fast8_t cc13x2_prop_rf_power_on(void);

/**
 * @brief   Power off the RF Core and shut down the Frequency Synthesizer.
 */
void cc13x2_prop_rf_power_off(void);

/**
 * @brief   Reset the RF Core state.
 *
 * @return  0 on success
 * @return  -1 on failure
 */
int_fast8_t cc13x2_prop_rf_reset(void);

/**
 * @brief   Get the current TX power.
 *
 * @return  The TX power.
 */
int8_t cc13x2_prop_rf_get_txpower(void);

/**
 * @brief   Set the TX power.
 *
 * @param[in] power The new transmit power in dBm.
 *
 * @return  0 on success
 * @return  -1 on failure
 */
int_fast8_t cc13x2_prop_rf_set_txpower(int8_t power);

/**
 * @brief   Get the RSSI of the last received packet.
 *
 * @return  The RSSI
 */
int8_t cc13x2_prop_rf_get_rssi(void);

/**
 * @brief   Put the RF driver in the "Receive" state.
 *
 *          - If we're on "Sleep" the state is updated to Receive.
 *          - If we're on "Receive" the state is not changed.
 *          - If we're on the "Transmit" state we abort the transmission and
 *            update the state to "Receive".
 *
 * @return  0 on success
 * @return  -1 on failure
 */
int_fast8_t cc13x2_prop_rf_rx_start(void);

/**
 * @brief   Stop receving and move the state to "Sleep".
 *
 *          - If the state isn't "Receive" nothing happens.
 *
 * @return  0 on success
 * @return  -1 on failure
 */
int_fast8_t cc13x2_prop_rf_rx_stop(void);

/**
 * @brief   Get current channel.
 *
 * @return  The current channel
 */
uint8_t cc13x2_prop_rf_get_chan(void);

/**
 * @brief   Set the channel to use.
 *
 * @param[in] channel The channel to use.
 */
void cc13x2_prop_rf_set_chan(uint16_t channel, bool force);

/**
 * @brief   Get the EUI64 from the device.
 */
void cc13x2_prop_rf_get_ieee_eui64(uint8_t *addr);

/**
 * @brief   Set the IRQ handler.
 *
 * @param[in] handler The handler function pointer.
 * @param[in] arg     The argument to pass to the handler.
 */
void cc13x2_prop_rf_irq_set_handler(void(*handler)(void *), void* arg);

/**
 * @brief   Read the contents of the next readily available buffer.
 *
 * @param[out] buf     The buffer to put the readed contents.
 * @param[in]  len     The length of the buffer.
 * @param[out] rx_info The RX info.
 *
 * @return  The number of bytes readed.
 */
int cc13x2_prop_rf_recv(void *buf, size_t len, netdev_ieee802154_rx_info_t *rx_info);

/**
 * @brief   Check if there is data available to read.
 *
 * @retval  false if nothing to read
 * @retval  true if there is data
 */
bool cc13x2_prop_rf_recv_avail(void);

/**
 * @brief   Transmit data
 *
 * @return -EOVERFLOW if the sum of the length of the iolist is bigger than
 *          the TX buffer size.
 * @return -EAGAIN the driver is busy, try again.
 */
int cc13x2_prop_rf_send(const iolist_t *iolist);

/**
 * @brief   is IRQ enabled?
 *
 * @param[in] irq The IRQ to check
 *
 * @return 1 is enabled
 * @return 0 is not enabled
 */
unsigned cc13x2_prop_rf_irq_is_enabled(unsigned irq);

/**
 * @brief   Enable a IRQ
 *
 * @param[in] irq The IRQ to enable
 */
void cc13x2_prop_rf_irq_enable(unsigned irq);

/**
 * @brief  Disable a IRQ
 *
 * @param[in] irq The IRQ to disable
 */
void cc13x2_prop_rf_irq_disable(unsigned irq);

/**
 * @brief   Get flags of the TX/RX state
 *
 * @note    The flags are cleared after calling this function.
 *
 * @return  The flags
 */
unsigned cc13x2_prop_rf_get_flags(void);

/**
 * @brief   Get the state of the RF Core.
 *
 * @return  The state.
 */
cc13x2_prop_rf_state_t cc13x2_prop_rf_get_state(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC13X2_PROP_RF_INTERNAL_H */

/*@}*/
