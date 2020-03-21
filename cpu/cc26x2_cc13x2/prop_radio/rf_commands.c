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

#include "cc13x2_prop_rf_commands.h"

/* Overrides for CMD_PROP_RADIO_DIV_SETUP */
static uint32_t rf_prop_overrides[] =
{
    /* override_prop_common.xml
     * DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0x7 (DITHER_EN=0 and IPEAK=7).
     */
    (uint32_t)0x00F788D3,
    /* override_tc146.xml
     * Tx: Configure PA ramp time, PACTL2.RC=0x3 (in ADI0, set PACTL2[4:3]=0x3)
     */
    ADI_2HALFREG_OVERRIDE(0,16,0x8,0x8,17,0x1,0x1),
    /* Tx: Configure PA ramping, set wait time before turning off (0x1A ticks
     of 16/24 us = 17.3 us).
     */
    HW_REG_OVERRIDE(0x6028,0x001A),
    /* Rx: Set AGC reference level to 0x16 (default: 0x2E) */
    HW_REG_OVERRIDE(0x609C,0x0016),
    /* Rx: Set RSSI offset to adjust reported RSSI by -1 dB (default: -2),
     * trimmed for external bias and differential configuration */
    (uint32_t)0x000188A3,
    /* Rx: Set anti-aliasing filter bandwidth to 0x8 (in ADI0, set
     * IFAMPCTL3[7:4]=0x8) */
    ADI_HALFREG_OVERRIDE(0,61,0xF,0x8),
    /* TX power override
     * Tx: Set PA trim to max to maximize its output power (in ADI0,
     * set PACTL0=0xF8) */
    ADI_REG_OVERRIDE(0,12,0xF8),
    (uint32_t)0xFFFFFFFF
};

rfc_cmd_prop_radio_div_setup_t rf_cmd_prop_radio_div_setup =
{
    .commandNo = 0x3807,
    .status = 0x0000,
    .pNextOp = NULL, /* set by us */
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .modulation.modType = 0x1,
    .modulation.deviation = 0xC8,
    .modulation.deviationStepSz = 0x0,
    .symbolRate.preScale = 0xF,
    .symbolRate.rateWord = 0x20000,
    .symbolRate.decimMode = 0x0,
    .rxBw = 0x59,
    .preamConf.nPreamBytes = 0x7,
    .preamConf.preamMode = 0x0,
    .formatConf.nSwBits = 0x18,
    .formatConf.bBitReversal = 0x0,
    .formatConf.bMsbFirst = 0x1,
    .formatConf.fecMode = 0x0,
    .formatConf.whitenMode = 0x7,
    .config.frontEndMode = 0x0,
    .config.biasMode = 0x1,
    .config.analogCfgMode = 0x0,
    .config.bNoFsPowerUp = 0x0,
    .txPower = 0x013F,
    .pRegOverride = rf_prop_overrides,
    .centerFreq = 0x0393,
    .intFreq = 0x0999,
    .loDivider = 0x05
};

rfc_cmd_fs_t rf_cmd_fs =
{
    .commandNo = CMD_FS,
    .status = IDLE,
    .pNextOp = NULL, /* set by us */
    .startTime = 0,
    .startTrigger.triggerType = TRIG_NOW,
    .startTrigger.bEnaCmd = 0,
    .startTrigger.triggerNo = 0,
    .startTrigger.pastTrig = 0,
    .condition.rule = COND_ALWAYS,
    .condition.nSkip = 0,
    .frequency = 0, /* set by us */
    .fractFreq = 0, /* set by us */
    .synthConf.bTxMode = 0,
    .synthConf.refFreq = 0,
    .__dummy0 = 0,
    .__dummy1 = 0,
    .__dummy2 = 0,
    .__dummy3 = 0
};
