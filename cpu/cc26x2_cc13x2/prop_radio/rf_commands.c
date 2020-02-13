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

#include <driverlib/rf_common_cmd.h>
#include <driverlib/rf_prop_mailbox.h>
#include <driverlib/rfc.h>

#define CMD_ALIGN __attribute__((aligned(4)))

static volatile CMD_ALIGN rfc_CMD_SYNC_START_RAT_t       _cmd_sync_start_rat;
static volatile CMD_ALIGN rfc_CMD_PROP_RADIO_DIV_SETUP_t _cmd_radio_setup;
static volatile CMD_ALIGN rfc_CMD_FS_POWERDOWN_t         _cmd_fs_powerdown;
static volatile CMD_ALIGN rfc_CMD_SYNC_STOP_RAT_t        _cmd_sync_stop_rat;
static volatile CMD_ALIGN rfc_CMD_CLEAR_RX_t             _cmd_clear_rx;

/*static volatile CMD_ALIGN rfc_CMD_FS_t                   _cmd_fs;*/
/*static volatile CMD_ALIGN rfc_CMD_PROP_RX_ADV_t          _cmd_receive;*/
/*static volatile CMD_ALIGN rfc_CMD_PROP_TX_ADV_t          _cmd_transmit;*/

static void memset_volatile(volatile void *s, char c, size_t n)
{
    volatile char *p = s;
    while (n-- > 0) {
        *p++ = c;
    }
}

uint16_t cc13x2_cmd_get_status(uint32_t cmd)
{
    assert(cmd != 0 && (cmd & 0x3) == 0);

    volatile rfc_radioOp_t *radio_cmd = (volatile rfc_radioOp_t *)cmd;
    return radio_cmd->status;
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

uint32_t cc13x2_cmd_prop_radio_div_setup(uint16_t tx_power, uint32_t *reg_override)
{
    memset_volatile(&_cmd_radio_setup, 0, sizeof(_cmd_radio_setup));

    _cmd_radio_setup.commandNo                  = CMD_PROP_RADIO_DIV_SETUP;
    _cmd_radio_setup.startTrigger.triggerType   = TRIG_NOW;
    _cmd_radio_setup.condition.rule             = COND_NEVER;

    /* Command default values */
    _cmd_radio_setup.modulation.modType         = CC13X2_MODULATION_TYPE;
    _cmd_radio_setup.modulation.deviation       = CC13X2_MODULATION_DEVIATION;
    _cmd_radio_setup.modulation.deviationStepSz = CC13X2_MODULATION_DEVIATION_STEP_SIZE;
    _cmd_radio_setup.symbolRate.preScale        = CC13X2_SYMBOL_RATE_PRESCALE;
    _cmd_radio_setup.symbolRate.rateWord        = CC13X2_SYMBOL_RATE_RATEWORD;
    _cmd_radio_setup.symbolRate.decimMode       = CC13X2_SYMBOL_RATE_DECIMMODE;
    _cmd_radio_setup.rxBw                       = 0x52;
    _cmd_radio_setup.preamConf.nPreamBytes      = IEEE802154_FSK_PREAMBLE_SIZE;
    _cmd_radio_setup.formatConf.nSwBits         = IEEE802154_2FSK_SFD_SIZE,
    _cmd_radio_setup.formatConf.bMsbFirst       = 0x1;
    _cmd_radio_setup.formatConf.whitenMode      = 0x7;
    _cmd_radio_setup.intFreq                    = 0x8000;

    /* Transmit power */
    _cmd_radio_setup.txPower      = tx_power;
    _cmd_radio_setup.pRegOverride = reg_override;
    _cmd_radio_setup.centerFreq   = CC13X2_CENTER_FREQ_SUB_GHZ;
    _cmd_radio_setup.loDivider    = CC13X2_LO_DIVIDER_SUB_GHZ;

    return (uint32_t)(&_cmd_radio_setup);
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
