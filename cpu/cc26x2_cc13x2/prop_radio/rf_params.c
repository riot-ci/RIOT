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
 * @}
 */

#include "cc13x2_prop_rf_params.h"

#include <driverlib/rf_mailbox.h>

uint32_t cc1312_register_overrides_mr_fsk[] = {
    /* Override name: override_prop_common.xml
     * DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0x7 (DITHER_EN=0 and IPEAK=7).
     */
    (uint32_t)0x00F788D3,
    /* Override name: override_tc106.xml
     * Tx: Configure PA ramp time, PACTL2.RC=0x3 (in ADI0, set PACTL2[4:3]=0x3)
     */
    ADI_2HALFREG_OVERRIDE(0, 16, 0x8, 0x8, 17, 0x1, 0x1),
    /* Override name: unknown
     * Rx: Set AGC reference level to 0x1A (default: 0x2E) */
    HW_REG_OVERRIDE(0x609C, 0x001A),
    /* Override name: unknown
     * Rx: Set RSSI offset to adjust reported RSSI by -1 dB (default: -2),
     * trimmed for external bias and differential configuration */
    (uint32_t)0x000188A3,
    /* Override name: unknown
     * Rx: Set anti-aliasing filter bandwidth to 0xD (in ADI0, set
     * IFAMPCTL3[7:4]=0xD)
     */
    ADI_HALFREG_OVERRIDE(0, 61, 0xF, 0xD),
    /* Override name: unknown
     * Tx: Set wait time before turning off ramp to 0x1A (default: 0x1F) */
    HW_REG_OVERRIDE(0x6028, 0x001A),
    /* Override name: unknown
     * TX power override
     * Tx: Set PA trim to max to maximize its output power (in ADI0, set
     * PACTL0=0xF8) */
    ADI_REG_OVERRIDE(0, 12, 0xF8),
    (uint32_t)0xFFFFFFFF
};
