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

#include <assert.h>

#include "net/ieee802154.h"
#include "net/netdev/ieee802154.h"

#ifndef CC13X2_CHANNEL_MIN_SUB_GHZ
#define CC13X2_CHANNEL_MIN_SUB_GHZ  (0U) /**< Minimum channel for Sub-GHz band */
#endif

#ifndef CC13X2_CHANNEL_MAX_SUB_GHZ
#define CC13X2_CHANNEL_MAX_SUB_GHZ  (198U) /**< Maximum channel for Sub-GHz band */
#endif

#ifndef CC13X2_FREQ_SPACING_SUB_GHZ
#define CC13X2_FREQ_SPACING_SUB_GHZ (200U) /**< Frequency spacing between channels */
#endif

#ifndef CC13X2_BASE_FREQ_SUB_GHZ
#define CC13X2_BASE_FREQ_SUB_GHZ    (902200U) /**< Base frequency for the Sub-GHz band */
#endif

#ifndef CC13X2_CENTER_FREQ_SUB_GHZ
#define CC13X2_CENTER_FREQ_SUB_GHZ  (0x0393U) /**< Center frequency for the Sub-GHz band */
#endif

#ifndef CC13X2_LO_DIVIDER_SUB_GHZ
#define CC13X2_LO_DIVIDER_SUB_GHZ   (0x05U) /**< LO divider for the Sub-GHz band */
#endif

#ifndef CC13X2_SYMBOL_RATE
/**
 * @brief   Symbol rate configuration
 *
 *          Default: 200 kbps
 */
#define CC13X2_SYMBOL_RATE { .preScale = 0xF, .rateWord = 0x20000, .decimMode = 0 }
#endif

/**
 * @brief   Modulation configuration
 *
 *          Default: 2-GFSK, deviation = 50,000 KHz
 */
#define CC13X2_MODULATION { .modType = 0x1, .deviation = 200, .deviationStepSz = 0 }

#ifndef IEEE802154_FSK_PREAMBLE_SIZE
#define IEEE802154_FSK_PREAMBLE_SIZE (4U) /**< Preamble length in octects */
#endif

/**
 * @brief   MR-FSK PHY SFD values for filtered 2FSK
 * @{
 */
#define IEEE802154_2FSK_UNCODED_SFD_0 (0x0000904EU) /**< SFD value for uncoded (PHR + PSDU) */
#define IEEE802154_2FSK_UNCODED_SFD_1 (0x00007A0EU) /**< SFD value for uncoded (PHR + PSDU) */
#define IEEE802154_2FSK_SFD_SIZE      (16U) /**< Size in bits of the SFD */
/** @} */

#define IEEE802154_PHR_SIZE (2) /**< Size in bits of the PHR */
#define IEEE802154_PHR_BITS (IEEE802154_PHR_SIZE * 8) /**< Size in bits of the PHR */
#define IEEE802154_PHR_FRAME_LENGTH_BITS (11U) /**< Size in bits of the Frame Length field */

#define CC13X2_RSSI_SIZE (1U)
#define CC13X2_CORR_SIZE (1U)

#define CC13X2_METADATA_SIZE (CC13X2_RSSI_SIZE + CC13X2_CORR_SIZE)

#define CCFG_UNKNOWN_EUI64 (0xFFU) /**< Unknown value for the EUI64 address on the CCFG */


/**
 * @brief This enum represents the state of a radio.
 *
 * Initially, a radio is in the Disabled state.
 *
 * The following are valid radio state transitions for the cc13x2:
 *
 *                                    (Radio ON)
 *  +----------+  Enable()  +-------+  Receive()   +---------+   Transmit()   +----------+
 *  |          |----------->|       |------------->|         |--------------->|          |
 *  | Disabled |            | Sleep |              | Receive |                | Transmit |
 *  |          |<-----------|       |<-------------|         |<---------------|          |
 *  +----------+  Disable() |       |   Sleep()    |         |                +----------+
 *                          |       | (Radio OFF)  +---------+
 *                          +-------+
 *
 * | State            | Description                                        |
 * |------------------|----------------------------------------------------|
 * | Disabled         | The rfcore powerdomain is off and the RFCPE is off |
 * | Sleep            | The RFCORE PD is on, and the RFCPE is in PROP mode |
 * | Receive          | The RFCPE is running a CMD_PROP_RX_ADV             |
 * | Transmit         | The RFCPE is running a transmit command string     |
 */
typedef enum {
    cc13x2_stateDisabled = 0, /**< Disabled, radio powered off */
    cc13x2_stateSleep, /**< Sleep state, awaiting for actions */
    cc13x2_stateReceive, /**< Receive state */
    cc13x2_stateTransmit, /**< Transmitting packets */
} cc13x2_PropPhyState_t;

extern volatile cc13x2_PropPhyState_t _cc13x2_prop_rf_state; /**< PHY state */

/**
 * @brief   Calculate the channel frequency for a given channel number.
 *
 * @pre (@p chan >= CC13X2_CHANNEL_MIN_SUB_GHZ) && (@p chan <= CC13X2_CHANNEL_MAX_SUB_GHZ)
 *
 * @param[in] chan Channel number.
 *
 * @return Channel frequency.
 */
static inline uint32_t cc13x2_prop_rf_channel_freq(const uint16_t chan)
{
    assert(chan <= CC13X2_CHANNEL_MAX_SUB_GHZ);

    const uint32_t chan0 = CC13X2_BASE_FREQ_SUB_GHZ;
    const uint32_t spacing = CC13X2_FREQ_SPACING_SUB_GHZ;
    const uint32_t chan_min = CC13X2_CHANNEL_MIN_SUB_GHZ;
    return chan0 + spacing * ((uint32_t)chan - chan_min);
}

/**
 * @brief   Get decimal and fractionary parts of a frequency
 *
 * @pre (@p dec != NULL) && (@p frac != NULL)
 *
 * @param[in] freq     The frequency.
 * @param[out] dec     The decimal part.
 * @param[out] frac    The fractionary part.
 */
static inline void cc13x2_prop_rf_freq_parts(const uint32_t freq, uint16_t *dec, uint16_t *frac)
{
    assert(dec != NULL);
    assert(frac != NULL);

    *dec = (uint16_t)(freq / 1000);
    *frac = (uint16_t)(((freq - ((*dec) * 1000)) * 0x10000) / 1000);
}

/**
 * @brief   Initialize the radio internal structures.
 *
 * @note    Must be called before @ref cc13x2_prop_rf_enable.
 */
void cc13x2_prop_rf_init(void);

/**
 * @brief   Power on the RF Core and send the setup command.
 *
 * @return  0 on success
 * @return  -1 on failure
 */
int_fast8_t cc13x2_prop_rf_enable(void);

/**
 * @brief   Power off the RF Core and shut down the Frequency Synthesizer.
 *
 * @return  0 on success
 * @return  -1 on failure
 */
int_fast8_t cc13x2_prop_rf_disable(void);

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
void cc13x2_prop_rf_set_chan(uint16_t channel);

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
 * @return  0 if nothing to read
 * @return  1 if there is data
 */
int cc13x2_prop_rf_recv_avail(void);

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

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC13X2_PROP_RF_INTERNAL_H */

/*@}*/
