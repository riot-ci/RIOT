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

#ifndef CC13X2_PROP_RF_PARAMS_H
#define CC13X2_PROP_RF_PARAMS_H

#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    CC1312 register overrides for MR-FSK PHY 50 kbps
 */
extern uint32_t cc1312_register_overrides_mr_fsk[];

#ifndef CC13X2_REGISTER_OVERRIDES
/**
 * @brief    CC13X2 register overrides
 */
#define CC13X2_REGISTER_OVERRIDES (cc1312_register_overrides_mr_fsk)
#endif

#define CC13X2_MAX_PACKET_SIZE      (2047) /**< Max. packet size */

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

#ifndef CC13X2_INTERMEDIATE_FREQUENCY
/**
 * @brief   Intermediate frequency for RX.
 *
 * @note    This is stored as a number in the Q4.12 format.
 */
#define CC13X2_INTERMEDIATE_FREQUENCY (0x8000)
#endif

#ifndef CC13X2_BIAS_MODE
/**
 * @brief   Bias mode
 *
 *          Default: external.
 */
#define CC13X2_BIAS_MODE (0x1)
#endif

/**
 * @brief   Symbol rate configuration
 *
 *          Default: 200 kbps
 * @{
 */
#define CC13X2_SYMBOL_RATE_PRESCALE  (0xF)
#define CC13X2_SYMBOL_RATE_RATEWORD  (0x8000)
#define CC13X2_SYMBOL_RATE_DECIMMODE (0)
/** @} */

/**
 * @brief   Modulation configuration
 * @{
 */
/* @brief   Modulation type
 *
 *          0h = FSK
 *          1h = GFSK
 *          2h = OOK
 *          Others = Reserved
 */
#define CC13X2_MODULATION_TYPE                (0x1)
/**
 * @brief   Modulation deviation in number of steps
 */
#define CC13X2_MODULATION_DEVIATION           (0x64)
/**
 * @brief   Deviation step size
 *
 *          0h = 250 Hz
 *          1h = 1000 Hz
 *          2h = 15.625 Hz
 *          3h = 62.5 Hz
 */
#define CC13X2_MODULATION_DEVIATION_STEP_SIZE (0x0)
/** @} */

#ifndef IEEE802154_FSK_PREAMBLE_SIZE
#define IEEE802154_FSK_PREAMBLE_SIZE (4U) /**< Preamble length in octects */
#endif

#ifndef CC13X2_PREAMBLE_MODE
/**
 * @brief   Preamble mode
 *
 *          0h = Send 0 as first preamble bit
 *          1h = Send 1 as first preamble bit
 *          2h = Send same first bit in preamble and sync word
 *          3h = Send different first bit in preamble and sync word
 */
#define CC13X2_PREAMBLE_MODE (0x0)
#endif

/**
 * @brief   MR-FSK PHY SFD values for filtered 2FSK
 * @{
 */
#define IEEE802154_2FSK_UNCODED_SFD_0 (0x0000904EU) /**< SFD value for uncoded (PHR + PSDU) */
#define IEEE802154_2FSK_UNCODED_SFD_1 (0x00007A0EU) /**< SFD value for uncoded (PHR + PSDU) */
#define IEEE802154_2FSK_SFD_SIZE      (16U) /**< Size in bits of the SFD */
/** @} */

#define IEEE802154_PHR_SIZE              (2) /**< Size in bits of the PHR */
#define IEEE802154_PHR_BITS              (IEEE802154_PHR_SIZE * 8) /**< Size in bits of the PHR */
#define IEEE802154_PHR_FRAME_LENGTH_BITS (11U) /**< Size in bits of the Frame Length field */

#define CC13X2_RSSI_SIZE (1U) /**< RSSI size in bytes */
#define CC13X2_CORR_SIZE (1U) /**< LQI size in bytes */

#define CC13X2_METADATA_SIZE (CC13X2_RSSI_SIZE + CC13X2_CORR_SIZE)

#define CCFG_UNKNOWN_EUI64 (0xFFU) /**< Unknown value for the EUI64 address on the CCFG */

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

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC13X2_PROP_RF_PARAMS_H */

/*@}*/
