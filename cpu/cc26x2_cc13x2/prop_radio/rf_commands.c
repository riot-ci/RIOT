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

#include "cc13x2_prop_rf_internal.h"
#include "cc13x2_prop_rf_params.h"

#include <driverlib/rf_common_cmd.h>
#include <driverlib/rf_prop_mailbox.h>
#include <driverlib/rfc.h>

#define CMD_ALIGN __attribute__((aligned(4)))

#define CMD_CMD0 (0x0607) /* RF Core CMD0 */

static volatile CMD_ALIGN rfc_CMD_SYNC_START_RAT_t       _cmd_sync_start_rat;
static volatile CMD_ALIGN rfc_CMD_PROP_RADIO_DIV_SETUP_t _cmd_prop_radio_div_setup;
static volatile CMD_ALIGN rfc_CMD_FS_POWERDOWN_t         _cmd_fs_powerdown;
static volatile CMD_ALIGN rfc_CMD_SYNC_STOP_RAT_t        _cmd_sync_stop_rat;
static volatile CMD_ALIGN rfc_CMD_CLEAR_RX_t             _cmd_clear_rx;

static volatile CMD_ALIGN rfc_CMD_FS_t                   _cmd_fs;
static volatile CMD_ALIGN rfc_CMD_SET_TX_POWER_t         _cmd_set_tx_power;
static volatile CMD_ALIGN rfc_CMD_PROP_RX_ADV_t          _cmd_prop_rx_adv;
static volatile CMD_ALIGN rfc_CMD_PROP_TX_ADV_t          _cmd_prop_tx_adv;

static void memset_volatile(volatile void *s, char c, size_t n)
{
    volatile char *p = s;
    while (n-- > 0) {
        *p++ = c;
    }
}

uint32_t cc13x2_cmd_sync_start_rat(uint32_t next_cmd, uint32_t rat0)
{
    memset_volatile(&_cmd_sync_start_rat, 0, sizeof(_cmd_sync_start_rat));

    /* Command default values */
    _cmd_sync_start_rat.commandNo                = CMD_SYNC_START_RAT;
    _cmd_sync_start_rat.startTrigger.triggerType = TRIG_NOW;
    _cmd_sync_start_rat.condition.rule           = COND_ALWAYS;

    /* Radio timer offset */
    _cmd_sync_start_rat.rat0 = rat0;

    /* Check whether we want to run another command after this */
    if (next_cmd != 0) {
        /* Don't run the next command if this command didn't succeed! */
        _cmd_sync_start_rat.condition.rule = COND_STOP_ON_FALSE;
        _cmd_sync_start_rat.pNextOp = (rfc_radioOp_t *)next_cmd;
    }

    return (uint32_t)(&_cmd_sync_start_rat);
}

uint32_t cc13x2_cmd_prop_radio_div_setup(uint16_t tx_power)
{
    memset_volatile(&_cmd_prop_radio_div_setup, 0, sizeof(_cmd_prop_radio_div_setup));

    _cmd_prop_radio_div_setup.commandNo                  = CMD_PROP_RADIO_DIV_SETUP;
    _cmd_prop_radio_div_setup.startTrigger.triggerType   = TRIG_NOW;
    _cmd_prop_radio_div_setup.condition.rule             = COND_NEVER;

    /* Command default values */
    _cmd_prop_radio_div_setup.modulation.modType         = CC13X2_MODULATION_TYPE;
    _cmd_prop_radio_div_setup.modulation.deviation       = CC13X2_MODULATION_DEVIATION;
    _cmd_prop_radio_div_setup.modulation.deviationStepSz = CC13X2_MODULATION_DEVIATION_STEP_SIZE;
    _cmd_prop_radio_div_setup.symbolRate.preScale        = CC13X2_SYMBOL_RATE_PRESCALE;
    _cmd_prop_radio_div_setup.symbolRate.rateWord        = CC13X2_SYMBOL_RATE_RATEWORD;
    _cmd_prop_radio_div_setup.symbolRate.decimMode       = CC13X2_SYMBOL_RATE_DECIMMODE;
    _cmd_prop_radio_div_setup.rxBw                       = 0x52;
    _cmd_prop_radio_div_setup.preamConf.nPreamBytes      = IEEE802154_FSK_PREAMBLE_SIZE;
    _cmd_prop_radio_div_setup.preamConf.preamMode        = CC13X2_PREAMBLE_MODE;
    _cmd_prop_radio_div_setup.formatConf.nSwBits         = IEEE802154_2FSK_SFD_SIZE,
    _cmd_prop_radio_div_setup.formatConf.bMsbFirst       = 0x1;
    _cmd_prop_radio_div_setup.formatConf.whitenMode      = 0x7;
    _cmd_prop_radio_div_setup.intFreq                    = CC13X2_INTERMEDIATE_FREQUENCY;
    _cmd_prop_radio_div_setup.config.biasMode            = CC13X2_BIAS_MODE;

    /* Transmit power */
    _cmd_prop_radio_div_setup.txPower      = tx_power;
    _cmd_prop_radio_div_setup.pRegOverride = CC13X2_REGISTER_OVERRIDES;
    _cmd_prop_radio_div_setup.centerFreq   = CC13X2_CENTER_FREQ_SUB_GHZ;
    _cmd_prop_radio_div_setup.loDivider    = CC13X2_LO_DIVIDER_SUB_GHZ;

    return (uint32_t)(&_cmd_prop_radio_div_setup);
}

uint32_t cc13x2_cmd_fs_powerdown(uint32_t next_cmd)
{
    memset_volatile(&_cmd_fs_powerdown, 0, sizeof(_cmd_fs_powerdown));

    /* Command default values */
    _cmd_fs_powerdown.commandNo                = CMD_FS_POWERDOWN;
    _cmd_fs_powerdown.startTrigger.triggerType = TRIG_NOW;
    _cmd_fs_powerdown.condition.rule           = COND_NEVER;

    /* Check whether we want to run another command after this */
    if (next_cmd != 0) {
        /* Always run the next command, we don't care if CMD_FS_POWERDOWN
         * didn't succeed */
        _cmd_fs_powerdown.condition.rule = COND_ALWAYS;
        _cmd_fs_powerdown.pNextOp = (rfc_radioOp_t *)next_cmd;
    }

    return (uint32_t)(&_cmd_fs_powerdown);
}

uint32_t cc13x2_cmd_sync_stop_rat(void)
{
    memset_volatile(&_cmd_sync_stop_rat, 0, sizeof(_cmd_sync_stop_rat));

    /* Command default values */
    _cmd_sync_stop_rat.commandNo                = CMD_SYNC_STOP_RAT;
    _cmd_sync_stop_rat.startTrigger.triggerType = TRIG_NOW;
    _cmd_sync_stop_rat.condition.rule           = COND_NEVER;

    return (uint32_t)(&_cmd_sync_stop_rat);
}

uint32_t cc13x2_cmd_sync_stop_rat_get_rat0(void)
{
    return _cmd_sync_stop_rat.rat0;
}

uint32_t cc13x2_cmd_clear_rx(dataQueue_t *queue)
{
    /* No memset_volatile because we're setting all fields */
    _cmd_clear_rx.commandNo = CMD_CLEAR_RX;
    _cmd_clear_rx.__dummy0  = 0;
    _cmd_clear_rx.pQueue    = queue;

    return (uint32_t)(&_cmd_clear_rx);
}

uint32_t cc13x2_cmd_fs(uint32_t next_cmd, uint16_t freq, uint16_t frac)
{
    memset_volatile(&_cmd_fs, 0, sizeof(_cmd_fs));

    /* Default command values */
    _cmd_fs.commandNo                = CMD_FS;
    _cmd_fs.status                   = IDLE;
    _cmd_fs.startTrigger.triggerType = TRIG_NOW;
    _cmd_fs.condition.rule           = COND_NEVER;

    /* Set frequency parameters */
    _cmd_fs.frequency = freq;
    _cmd_fs.fractFreq = frac;

    if (next_cmd != 0) {
        /* Don't run next command if we fail */
        _cmd_fs.condition.rule = COND_STOP_ON_FALSE;
        _cmd_fs.pNextOp = (rfc_radioOp_t *)next_cmd;
    }

    return (uint32_t)(&_cmd_fs);
}

uint32_t cc13x2_cmd_set_tx_power(uint16_t tx_power)
{
    /* Not memset_volatile here as these are the only two fields */
    _cmd_set_tx_power.commandNo = CMD_SET_TX_POWER;
    _cmd_set_tx_power.txPower   = tx_power;

    return (uint32_t)(&_cmd_set_tx_power);
}

uint32_t cc13x2_cmd_prop_rx_adv(dataQueue_t *queue, void *output)
{
    memset_volatile(&_cmd_prop_rx_adv, 0, sizeof(_cmd_prop_rx_adv));

    /* Command default values */
    _cmd_prop_rx_adv.commandNo                = CMD_PROP_RX_ADV;
    _cmd_prop_rx_adv.status                   = IDLE;
    _cmd_prop_rx_adv.startTrigger.triggerType = TRIG_NOW;
    _cmd_prop_rx_adv.condition.rule           = COND_NEVER;

    /* Use CRC */
    _cmd_prop_rx_adv.pktConf.bUseCrc          = 0;

    /* Flush ignored packets and packets with CRC errors from the receive
     * queue */
    _cmd_prop_rx_adv.rxConf.bAutoFlushIgnored = 1;
    _cmd_prop_rx_adv.rxConf.bAutoFlushCrcErr  = 1;

    /* Append RSSI and Status (Link Quality Info) at the end of the packet
     * in the receive buffer */
    _cmd_prop_rx_adv.rxConf.bAppendRssi       = 1;
    _cmd_prop_rx_adv.rxConf.bAppendStatus     = 1;

    /* Syncrhonization word - Start-of-Frame Delimiter value */
    _cmd_prop_rx_adv.syncWord0                = IEEE802154_2FSK_UNCODED_SFD_0;

    /* Maximum packet size that can be transmitted */
    _cmd_prop_rx_adv.maxPktLen                = CC13X2_MAX_PACKET_SIZE;

    /* Header configuration - PHY Header size in bits, and the size of the
     * length field in bits */
    _cmd_prop_rx_adv.hdrConf.numHdrBits       = IEEE802154_PHR_BITS;
    _cmd_prop_rx_adv.hdrConf.numLenBits       = IEEE802154_PHR_FRAME_LENGTH_BITS;

    /* XXX: should this command end and enter a FSM_STATE_SLEEP state? */
    /* Don't end this command */
    _cmd_prop_rx_adv.endTrigger.triggerType   = TRIG_NEVER;

    /* The RX data queue */
    _cmd_prop_rx_adv.pQueue = queue;

    /* The RX statistics information */
    _cmd_prop_rx_adv.pOutput = output;

    return (uint32_t)(&_cmd_prop_rx_adv);
}

uint32_t cc13x2_cmd_prop_tx_adv(void *packet, uint16_t packet_len)
{
    memset_volatile(&_cmd_prop_tx_adv, 0, sizeof(_cmd_prop_rx_adv));

    /* Default command values */
    _cmd_prop_tx_adv.commandNo                = CMD_PROP_TX_ADV;
    _cmd_prop_tx_adv.status                   = IDLE;
    _cmd_prop_tx_adv.startTrigger.triggerType = TRIG_NOW;
    _cmd_prop_tx_adv.condition.rule           = COND_NEVER;

    /* Use CRC */
    _cmd_prop_tx_adv.pktConf.bUseCrc          = 1;

    /* MR-FSK PHY Header size in bits */
    _cmd_prop_tx_adv.numHdrBits               = IEEE802154_PHR_BITS;

    _cmd_prop_tx_adv.preTrigger.triggerType   = TRIG_REL_START;
    _cmd_prop_tx_adv.preTrigger.pastTrig      = 1;

    /* Synchronization word - Start-of-Frame Delimiter (SFD) */
    _cmd_prop_tx_adv.syncWord                 = IEEE802154_2FSK_UNCODED_SFD_0;

    /* Packet data */
    _cmd_prop_tx_adv.pktLen = packet_len;
    _cmd_prop_tx_adv.pPkt   = packet;

    return (uint32_t)(&_cmd_prop_tx_adv);
}

uint32_t cc13x2_cmd_abort(void)
{
    return CMDR_DIR_CMD(CMD_ABORT);
}

uint32_t cc13x2_cmd_ping(void)
{
    return CMDR_DIR_CMD(CMD_PING);
}

uint32_t cc13x2_cmd_cmd0(uint16_t clk_en)
{
    return CMDR_DIR_CMD_2BYTE(CMD_CMD0, clk_en);
}

uint8_t cc13x2_dbell_execute(uint32_t cmd)
{
    return RFCDoorbellSendTo(cmd) & 0xFF;
}

uint16_t cc13x2_cmd_prop_radio_div_setup_status(void)
{
    return _cmd_prop_radio_div_setup.status;
}

uint16_t cc13x2_cmd_sync_stop_rat_get_status(void)
{
    return _cmd_sync_stop_rat.status;
}

uint16_t cc13x2_cmd_prop_rx_adv_get_status(void)
{
    return _cmd_prop_rx_adv.status;
}

uint16_t cc13x2_cmd_prop_tx_adv_get_status(void)
{
    return _cmd_prop_tx_adv.status;
}
