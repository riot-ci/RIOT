/*
 *  Copyright (c) 2018, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

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

#include "rf_conf.h"

#include "cc13x2_prop_rf_internal.h"
#include "cc26x2_cc13x2_rfc.h"
#include "cc26xx_cc13xx_ints.h"

#include <stdio.h>
#include <assert.h>

#include "random.h"
#include "iolist.h"

#include <driverlib/chipinfo.h>
#include <driverlib/gpio.h>
#include <driverlib/ioc.h>
#include <driverlib/osc.h>
#include <driverlib/prcm.h>
#include <driverlib/rf_common_cmd.h>
#include <driverlib/rf_data_entry.h>
#include <driverlib/rf_prop_mailbox.h>
#include <driverlib/rf_mailbox.h>
#include <driverlib/rfc.h>
#include <inc/hw_ccfg.h>
#include <inc/hw_fcfg1.h>
#include <inc/hw_memmap.h>
#include <inc/hw_prcm.h>

/* phy state as defined by openthread */
volatile cc13x2_PropPhyState_t _cc13x2_prop_rf_state;

/* set to max transmit power by default */
static output_config_t const *_current_tx_power = &(output_power_table[0]);

static uint32_t sPropOverrides[]  =
{
    (uint32_t)0x00F788D3,
    ADI_2HALFREG_OVERRIDE(0,16,0x8,0x8,17,0x1,0x1),
    HW_REG_OVERRIDE(0x609C,0x001A),
    (uint32_t)0x000188A3,
    ADI_HALFREG_OVERRIDE(0,61,0xF,0xD),
    HW_REG_OVERRIDE(0x6028,0x001A),
    (uint32_t)0xFFFFFFFF,
};

/*
 * Number of retry counts left to the currently transmitting frame.
 *
 * Initialized when a frame is passed to be sent over the air, and decremented
 * by the radio ISR every time the transmit command string fails to receive a
 * corresponding ack.
 */
static volatile unsigned int sTransmitRetryCount = 0;

/*
 * Offset of the radio timer from the rtc.
 *
 * Used when we start and stop the RAT on enabling and disabling of the rf
 * core.
 */
static uint32_t _rat_offset = 0;

static volatile __attribute__((aligned(4))) rfc_CMD_SYNC_START_RAT_t _cmd_start_rat;
static volatile __attribute__((aligned(4))) rfc_CMD_PROP_RADIO_DIV_SETUP_t _cmd_radio_setup;
static volatile __attribute__((aligned(4))) rfc_CMD_FS_t _cmd_fs;

static volatile __attribute__((aligned(4))) rfc_CMD_FS_POWERDOWN_t _cmd_fs_powerdown;
static volatile __attribute__((aligned(4))) rfc_CMD_SYNC_STOP_RAT_t _cmd_stop_rat;

static volatile __attribute__((aligned(4))) rfc_CMD_CLEAR_RX_t _cmd_clear_rx;

static volatile __attribute__((aligned(4))) rfc_CMD_PROP_RX_ADV_t _cmd_receive;
static volatile __attribute__((aligned(4))) rfc_CMD_PROP_TX_ADV_t _cmd_transmit;

static volatile uint16_t _channel;

static __attribute__((aligned(4))) rfc_propRxOutput_t _rf_stats;

/*
 * Two receive buffers entries with room for 1 max IEEE802.15.4 frame in each
 *
 * These will be setup in a circular buffer configuration by /ref _rx_data_queue.
 */
#define RX_BUF_SIZE 144
static __attribute__((aligned(4))) uint8_t _rx_buf0[RX_BUF_SIZE];
static __attribute__((aligned(4))) uint8_t _rx_buf1[RX_BUF_SIZE];
static __attribute__((aligned(4))) uint8_t _rx_buf2[RX_BUF_SIZE];
static __attribute__((aligned(4))) uint8_t _rx_buf3[RX_BUF_SIZE];

#define TX_BUF_SIZE 144
static uint8_t _tx_buf[TX_BUF_SIZE] __attribute__((aligned(4)));

volatile unsigned _irq_handler_flags;

/*
 * The RX Data Queue used by @ref _cmd_receive.
 */
static __attribute__((aligned(4))) dataQueue_t _rx_data_queue = {0};

/*
 * OpenThread data primitives
 */
static uint_fast8_t _transmit_error;

/* network stack ISR handlers */
static void (*_irq_handler)(void*);
static void *_irq_handler_arg;

/**
 * Initialize the RX/TX buffers.
 *
 * Zeros out the receive and transmit buffers and sets up the data structures
 * of the receive queue.
 */
static void cc13x2_prop_rf_init_bufs(void)
{
    rfc_dataEntry_t *entry;
    memset(_rx_buf0, 0, sizeof(_rx_buf0));
    memset(_rx_buf1, 0, sizeof(_rx_buf1));
    memset(_rx_buf2, 0, sizeof(_rx_buf2));
    memset(_rx_buf3, 0, sizeof(_rx_buf3));

    entry               = (rfc_dataEntry_t *)_rx_buf0;
    entry->pNextEntry   = _rx_buf1;
    entry->config.lenSz = sizeof(uint16_t);
    entry->length       = sizeof(_rx_buf0) - sizeof(rfc_dataEntry_t);

    entry               = (rfc_dataEntry_t *)_rx_buf1;
    entry->pNextEntry   = _rx_buf2;
    entry->config.lenSz = sizeof(uint16_t);
    entry->length       = sizeof(_rx_buf1) - sizeof(rfc_dataEntry_t);

    entry               = (rfc_dataEntry_t *)_rx_buf2;
    entry->pNextEntry   = _rx_buf3;
    entry->config.lenSz = sizeof(uint16_t);
    entry->length       = sizeof(_rx_buf2) - sizeof(rfc_dataEntry_t);

    entry               = (rfc_dataEntry_t *)_rx_buf3;
    entry->pNextEntry   = _rx_buf0;
    entry->config.lenSz = sizeof(uint16_t);
    entry->length       = sizeof(_rx_buf3) - sizeof(rfc_dataEntry_t);
}

/**
 * Initialize the RX command structure.
 *
 * Sets the default values for the receive command structure.
 */
static void cc13x2_rf_core_init_rx_params(void)
{
    static const rfc_CMD_PROP_RX_ADV_t cReceiveCmd =
    {
        .commandNo                  = CMD_PROP_RX_ADV,
        .status                     = IDLE,
        .pNextOp                    = NULL,
        .startTime                  = 0u,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  =
        {
            .rule                   = COND_NEVER,
        },
        .pktConf                    =
        {
            .bFsOff                 = 0,
            .bRepeatOk              = 1,
            .bRepeatNok             = 1,
            .bUseCrc                = 1,
            .bCrcIncSw              = 0,
            .bCrcIncHdr             = 0,
            .endType                = 0,
            .filterOp               = 1,
        },
        .rxConf                     =
        {
            .bAutoFlushIgnored      = 1,
            .bAutoFlushCrcErr       = 1,
            .bIncludeHdr            = 0,
            .bIncludeCrc            = 0,
            .bAppendRssi            = 1,
            .bAppendTimestamp       = 0,
            .bAppendStatus          = 1,
        },
        .syncWord0                  = 0x0055904E,
        .syncWord1                  = 0x00000000,
        .maxPktLen                  = 0,
        .hdrConf                    =
        {
            .numHdrBits             = 0x10,
            .lenPos                 = 0,
            .numLenBits             = 0x0B,
        },
        .addrConf                   =
        {
            .addrType               = 0,
            .addrSize               = 0,
            .addrPos                = 0,
            .numAddr                = 0,
        },
        .lenOffset                  = 0xFC,
        .endTrigger                 =
        {
            .triggerType            = TRIG_NEVER,
            .bEnaCmd                = 0x0,
            .triggerNo              = 0x0,
            .pastTrig               = 0x0,
        },
        .endTime                    = 0x00000000,
        .pAddr                      = 0,
        .pQueue                     = 0,
        .pOutput                    = 0
    };

    _cmd_receive = cReceiveCmd;

    _cmd_receive.pQueue  = &_rx_data_queue;
    _cmd_receive.pOutput = (uint8_t *)&_rf_stats;
}

/**
 * Sends the immediate clear rx queue command to the radio core.
 *
 * Uses the radio core to mark all of the entries in the receive queue as
 * pending. This is used instead of clearing the entries manually to avoid race
 * conditions between the main processor and the radio core.
 *
 * @param [in] aQueue A pointer to the receive queue to be cleared.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t cc13x2_prop_rf_clear_rx_queue(dataQueue_t *aQueue)
{
    /* memset skipped because _cmd_clear_rx has only 2 members and padding */
    _cmd_clear_rx.commandNo = CMD_CLEAR_RX;
    _cmd_clear_rx.pQueue    = aQueue;

    return (RFCDoorbellSendTo((uint32_t)&_cmd_clear_rx) & 0xFF);
}

/**
 * Sends the tx command to the radio core.
 *
 * Sends the packet to the radio core to be sent asynchronously.
 *
 * @note @ref aPsdu *must* be 4 byte aligned and not include the FCS.
 *
 * @param [in] aPsdu A pointer to the data to be sent.
 * @param [in] aLen  The length in bytes of data pointed to by PSDU.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t cc13x2_prop_rf_send_tx_cmd(uint8_t *aPsdu, uint8_t aLen)
{
    static const rfc_CMD_PROP_TX_ADV_t cTransmitCmd =
    {
        .commandNo                  = CMD_PROP_TX_ADV,
        .status                     = IDLE,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_NEVER,
        },
        .pNextOp                    = NULL,
        .pktConf                    =
        {
            .bFsOff                 = 0,
            .bUseCrc                = 1,
            .bCrcIncSw              = 0,
            .bCrcIncHdr             = 0,
        },
        .numHdrBits                 = 0x10,
        .startConf                  =
        {
            .bExtTxTrig             = 0,
            .inputMode              = 0,
            .source                 = 0,
        },
        .preTrigger                 =
        {
            .triggerType            = TRIG_REL_START,
            .bEnaCmd                = 0,
            .triggerNo              = 0,
            .pastTrig               = 1,
        },
        .preTime                    = 0x00000000,
        .syncWord                   = 0x0055904E,
    };

    /* reset retry count */
    sTransmitRetryCount = 0;

    _cmd_transmit = cTransmitCmd;
    /* no need to look for an ack if the tx operation was stopped */
    _cmd_transmit.pktLen = aLen;
    _cmd_transmit.pPkt = aPsdu;

    if (aPsdu[0] & IEEE802154_FCF_ACK_REQ)
    {
        /* XXX: ack? */
    }

    return (RFCDoorbellSendTo((uint32_t)&_cmd_transmit) & 0xFF);
}

/**
 * Sends the rx command to the radio core.
 *
 * Sends the pre-built receive command to the radio core. This sets up the
 * radio to receive packets according to the settings in the global rx command.
 *
 * @note This function does not alter any of the parameters of the rx command.
 * It is only concerned with sending the command to the radio core. See @ref
 * rfSetPanId for an example of how the rx settings are set changed.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t cc13x2_prop_rf_send_rx_cmd(void)
{
    _cmd_receive.status = IDLE;
    return (RFCDoorbellSendTo((uint32_t)&_cmd_receive) & 0xFF);
}

static uint_fast8_t cc13x2_prop_rf_send_fs_cmd(uint16_t frequency, uint16_t fractFreq)
{
    static const rfc_CMD_FS_t cFsCmd =
    {
        .commandNo                  = CMD_FS,
        .status                     = IDLE,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_NEVER,
        },
        .frequency                  = 0x0393,
        .fractFreq                  = 0,
        .synthConf                  =
        {
            .bTxMode                = 0,
            .refFreq                = 0,
        },
    };
    _cmd_fs = cFsCmd;

    _cmd_fs.frequency = frequency;
    _cmd_fs.fractFreq = fractFreq;

    return (RFCDoorbellSendTo((uint32_t)&_cmd_fs) & 0xFF);
}

/**
 * Sends the setup command string to the radio core.
 *
 * Enables the clock line from the RTC to the RF core RAT. Enables the RAT
 * timer and sets up the radio in IEEE mode.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command was received.
 */
static uint_fast16_t cc13x2_prop_rf_send_enable_cmd(void)
{
    uint8_t       doorbellRet;
    bool          ints_disabled;
    uint_fast16_t ret;

    static const rfc_CMD_SYNC_START_RAT_t cStartRatCmd =
    {
        .commandNo                  = CMD_SYNC_START_RAT,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_STOP_ON_FALSE,
        },
    };
    static const rfc_CMD_PROP_RADIO_DIV_SETUP_t cRadioSetupCmd =
    {
        .commandNo                  = CMD_PROP_RADIO_DIV_SETUP,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_NEVER,
        },
        .modulation                 =
        {
            .modType                = 0x1,
            .deviation              = 0x64,
            .deviationStepSz        = 0x0,
        },
        .symbolRate                 =
        {
            .preScale               = 0xF,
            .rateWord               = 0x8000,
            .decimMode              = 0x0,
        },
        .rxBw                       = 0x52,
        .preamConf                  =
        {
            .nPreamBytes            = 0x7,
            .preamMode              = 0x0,
        },
        .formatConf                 =
        {
            .nSwBits                = 0x18,
            .bBitReversal           = 0x0,
            .bMsbFirst              = 0x1,
            .fecMode                = 0x0,
            .whitenMode             = 0x7,
        },
        .config =
        {
            .frontEndMode           = 0x0,
            .biasMode               = 0x0,
            .analogCfgMode          = 0x0,
            .bNoFsPowerUp           = 0x0,
        },
        .txPower                    = 0xB224,
        .pRegOverride               = 0,
        .centerFreq                 = 0x0393,
        .intFreq                    = 0x8000,
        .loDivider                  = 0x05,
    };

    /* turn on the clock line to the radio core */
    HWREGBITW(AON_RTC_BASE + AON_RTC_O_CTL, AON_RTC_CTL_RTC_UPD_EN_BITN) = 1;

    /* initialize the rat start command */
    _cmd_start_rat         = cStartRatCmd;
    _cmd_start_rat.pNextOp = (rfc_radioOp_t *)&_cmd_radio_setup;
    _cmd_start_rat.rat0    = _rat_offset;

    /* initialize radio setup command */
    _cmd_radio_setup = cRadioSetupCmd;
    /* initially set the radio tx power to the max */
    _cmd_radio_setup.txPower             = _current_tx_power->value;
    _cmd_radio_setup.pRegOverride        = sPropOverrides;
    _cmd_radio_setup.config.frontEndMode = 0; /* differential */
    _cmd_radio_setup.config.biasMode     = 0; /* internal bias */
    _cmd_radio_setup.centerFreq          = CC13X2_CENTER_FREQ_SUB_GHZ;
    _cmd_radio_setup.loDivider           = CC13X2_LO_DIVIDER_SUB_GHZ;

    ints_disabled = IntMasterDisable();

    doorbellRet = (RFCDoorbellSendTo((uint32_t)&_cmd_start_rat) & 0xFF);
    if (CMDSTA_Done != doorbellRet) {
        ret = doorbellRet;
        goto exit;
    }

    /* synchronously wait for the CM0 to stop executing */
    while ((HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_LAST_COMMAND_DONE) == 0x00)
        ;

    ret = _cmd_radio_setup.status;

exit:

    if (!ints_disabled)
    {
        IntMasterEnable();
    }

    return ret;
}

/**
 * Sends the shutdown command string to the radio core.
 *
 * Powers down the frequency synthesizer and stops the RAT.
 *
 * @note synchronously waits until the command string completes.
 *
 * @return The status of the RAT stop command.
 * @retval DONE_OK The command string executed properly.
 */
static uint_fast16_t cc13x2_prop_rf_send_disable_cmd(void)
{
    uint8_t       doorbellRet;
    bool          ints_disabled;
    uint_fast16_t ret;

    static const rfc_CMD_FS_POWERDOWN_t cFsPowerdownCmd =
    {
        .commandNo                  = CMD_FS_POWERDOWN,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_ALWAYS,
        },
    };
    static const rfc_CMD_SYNC_STOP_RAT_t cStopRatCmd =
    {
        .commandNo                  = CMD_SYNC_STOP_RAT,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_NEVER,
        },
    };

    HWREGBITW(AON_RTC_BASE + AON_RTC_O_CTL, AON_RTC_CTL_RTC_UPD_EN_BITN) = 1;

    /* initialize the command to power down the frequency synth */
    _cmd_fs_powerdown         = cFsPowerdownCmd;
    _cmd_fs_powerdown.pNextOp = (rfc_radioOp_t *)&_cmd_stop_rat;

    _cmd_stop_rat = cStopRatCmd;

    ints_disabled = IntMasterDisable();

    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~IRQ_LAST_COMMAND_DONE;

    doorbellRet = (RFCDoorbellSendTo((uint32_t)&_cmd_fs_powerdown) & 0xFF);
    if (CMDSTA_Done != doorbellRet) {
        ret = doorbellRet;
        goto exit;
    };

    /* synchronously wait for the CM0 to stop */
    while ((HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_LAST_COMMAND_DONE) == 0x00)
        ;

    ret = _cmd_stop_rat.status;

    if (_cmd_stop_rat.status == DONE_OK)
    {
        _rat_offset = _cmd_stop_rat.rat0;
    }

exit:

    if (!ints_disabled)
    {
        IntMasterEnable();
    }

    return ret;
}

/**
 * Error interrupt handler.
 */
void _isr_rfc_cpe1(void)
{
    /* Clear INTERNAL_ERROR interrupt flag */
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = 0x7FFFFFFF;
}

/**
 * Command done handler.
 */
void _isr_rfc_cpe0(void)
{
    if (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_LAST_COMMAND_DONE)
    {
        HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~IRQ_LAST_COMMAND_DONE;

        if (_cc13x2_prop_rf_state == cc13x2_stateReceive && _cmd_receive.status != ACTIVE && _cmd_receive.status != PROP_DONE_RXTIMEOUT)
        {
            /* the rx command was probably aborted to change the channel */
            _cc13x2_prop_rf_state = cc13x2_stateSleep;
        }
    }

    if (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_LAST_FG_COMMAND_DONE)
    {
        HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~IRQ_LAST_FG_COMMAND_DONE;

        if (_cc13x2_prop_rf_state == cc13x2_stateTransmit)
        {
            if (_cmd_transmit.pPkt[0] & IEEE802154_FCF_ACK_REQ)
            {
                /* XXX: ack? */
            }
            else
            {
                /* The TX command was either stopped or we are not looking for
                 * an ack */
                switch (_cmd_transmit.status)
                {
                case PROP_DONE_OK:
                    _transmit_error = 0;
                    break;

                case PROP_ERROR_NO_SETUP:
                case PROP_ERROR_NO_FS:
                    _transmit_error = -1;
                    break;

                case PROP_ERROR_TXUNF:
                    _transmit_error = -1;
                    break;

                default:
                    _transmit_error = -1;
                    break;
                }
            }
        }
    }

    if (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_RX_OK) {
        _irq_handler_flags |= 1;
        HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~IRQ_RX_OK;
    }

    if (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_TX_DONE) {
        _irq_handler_flags |= 2;
        _cc13x2_prop_rf_state = cc13x2_stateReceive;
        HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~IRQ_TX_DONE;
    }

    if (_irq_handler_flags) {
        if (_irq_handler) {
            _irq_handler(_irq_handler_arg);
        }
    }
}

void cc13x2_prop_rf_init(void)
{
    cc26xx_cc13xx_set_isr_rfc_cpe0_handler(_isr_rfc_cpe0);
    cc26xx_cc13xx_set_isr_rfc_cpe1_handler(_isr_rfc_cpe1);

    /* Populate the RX parameters data structure with default values */
    cc13x2_rf_core_init_rx_params();

    _cc13x2_prop_rf_state = cc13x2_stateDisabled;
}

int_fast8_t cc13x2_prop_rf_enable(void)
{
    int_fast8_t error = -1;

    if (_cc13x2_prop_rf_state == cc13x2_stateSleep)
    {
        error = 0;
    }
    else if (_cc13x2_prop_rf_state == cc13x2_stateDisabled)
    {

        /* Set of RF Core data queue. Circular buffer, no last entry */
        _rx_data_queue.pCurrEntry = _rx_buf0;
        _rx_data_queue.pLastEntry = NULL;

        cc13x2_prop_rf_init_bufs();

        if (cc26x2_cc13x2_rf_power_on() != CMDSTA_Done) {
            error = -1;
            goto exit;
        }

        if (cc13x2_prop_rf_send_enable_cmd() != PROP_DONE_OK) {
            error = -1;
            goto exit;
        }

        _cc13x2_prop_rf_state = cc13x2_stateSleep;
        error  = 0;
    }

exit:

    if (error == -1)
    {
        cc26x2_cc13x2_rf_power_off();
        _cc13x2_prop_rf_state = cc13x2_stateDisabled;
    }

    return error;
}

int_fast8_t cc13x2_prop_rf_disable(void)
{
    int_fast8_t error = -1;

    if (_cc13x2_prop_rf_state == cc13x2_stateDisabled)
    {
        error = 0;
    }
    else if (_cc13x2_prop_rf_state == cc13x2_stateSleep)
    {
        cc13x2_prop_rf_send_disable_cmd();
        /* we don't want to fail if this command string doesn't work, just turn
         * off the whole core
         */
        cc26x2_cc13x2_rf_power_off();
        _cc13x2_prop_rf_state = cc13x2_stateDisabled;
        error  = 0;
    }

    return error;
}

int_fast8_t cc13x2_prop_rf_reset(void)
{
    int_fast8_t error = 0;
    if (_cc13x2_prop_rf_state == cc13x2_stateDisabled) {
        if (cc13x2_prop_rf_enable() == -1) {
            error = -1;
            goto exit;
        }

        if (cc13x2_prop_rf_rx_start() == -1) {
            error = -1;
            goto exit;
        }
    }
    else if (_cc13x2_prop_rf_state == cc13x2_stateSleep ||
             _cc13x2_prop_rf_state == cc13x2_stateTransmit) {
        /* If we're in Sleep mode, start receiving, if we're on the Transmit
         * mode trigger the start of the Receive state, this will abort any
         * transmission being done. */
        if (cc13x2_prop_rf_rx_start() == -1) {
            error = -1;
            goto exit;
        }
    }
    else if (_cc13x2_prop_rf_state == cc13x2_stateReceive) {
        error = 0;
        goto exit;
    }

exit:
    return error;
}

int8_t cc13x2_prop_rf_get_txpower(void)
{
    return _current_tx_power->dbm;
}

int_fast8_t cc13x2_prop_rf_set_txpower(int8_t power)
{
    unsigned int i;
    output_config_t const *powercfg = &(output_power_table[0]);

    for (i = 1; i < OUTPUT_CONFIG_COUNT; i++)
    {
        if (output_power_table[i].dbm >= power)
        {
            powercfg = &(output_power_table[i]);
        }
        else
        {
            break;
        }
    }

    _current_tx_power = powercfg;

    return 0;
}

int8_t cc13x2_prop_rf_get_rssi(void)
{
    return _rf_stats.lastRssi;
}

int_fast8_t cc13x2_prop_rf_rx_start(void)
{
    int_fast8_t error = -1;

    if (_cc13x2_prop_rf_state == cc13x2_stateSleep)
    {
        _cc13x2_prop_rf_state = cc13x2_stateReceive;

        /* initialize the receive command
         * XXX: no memset here because we assume init has been called and we
         *      may have changed some values in the rx command
         */
        if (!(cc13x2_prop_rf_send_rx_cmd() == CMDSTA_Done)) {
            error = -1;
            goto exit;
        }
        error = 0;
    }
    else if (_cc13x2_prop_rf_state == cc13x2_stateReceive)
    {
        if (_cmd_receive.status == ACTIVE)
        {
            /* we are already receiving */
            _cc13x2_prop_rf_state = cc13x2_stateReceive;
            error  = -1;
        }
        else
        {
            /* we have either not fallen back into our receive command or
             * we are running on the wrong channel. Either way assume the
             * caller correctly called us and abort all running commands.
             */
            if (!(cc26x2_cc13x2_rf_execute_abort_cmd() == CMDSTA_Done)) {
                error = -1;
                goto exit;
            }

            /* any frames in the queue will be for the old channel */
            if (!(cc13x2_prop_rf_clear_rx_queue(&_rx_data_queue) == CMDSTA_Done)) {
                error = -1;
                goto exit;
            }

            if (!(cc13x2_prop_rf_send_rx_cmd() == CMDSTA_Done)) {
                error = -1;
                goto exit;
            }

            _cc13x2_prop_rf_state = cc13x2_stateReceive;
             error  = 0;
        }
    }

exit:
    return error;
}

int_fast8_t cc13x2_prop_rf_rx_stop(void)
{
    int_fast8_t error = 0;

    if (_cc13x2_prop_rf_state == cc13x2_stateReceive)
    {
        if (!(cc26x2_cc13x2_rf_execute_abort_cmd() == CMDSTA_Done)) {
            error = -1;
            goto exit;
        }

        _cc13x2_prop_rf_state = cc13x2_stateSleep;
        error  = 0;
    }

exit:
    return error;
}

uint8_t cc13x2_prop_rf_get_chan(void)
{
    return _channel;
}

void cc13x2_prop_rf_set_chan(uint16_t channel)
{
    if (_channel == channel) {
        return;
    }

    int rx_was_active = (_cc13x2_prop_rf_state == cc13x2_stateReceive);

    if (rx_was_active) {
        cc13x2_prop_rf_rx_stop();
    }

    const uint32_t new_freq = cc13x2_prop_rf_channel_freq(channel);

    uint16_t freq;
    uint16_t frac;
    cc13x2_prop_rf_freq_parts(new_freq, &freq, &frac);

    if (cc13x2_prop_rf_send_fs_cmd(freq, frac) != CMDSTA_Done) {
        /* TODO: error */
    }

    _channel = channel;

    if (rx_was_active) {
        cc13x2_prop_rf_rx_start();
    }
    /* TODO: handle cc13x2_prop_rf_state == cc13x2_stateTransmit */

    return;
}

void cc13x2_prop_rf_get_ieee_eui64(uint8_t *aIeeeEui64)
{
    uint8_t *    eui64;
    unsigned int i;

    /*
     * The IEEE MAC address can be stored two places. We check the Customer
     * Configuration was not set before defaulting to the Factory
     * Configuration.
     */
    eui64 = (uint8_t *)(CCFG_BASE + CCFG_O_IEEE_MAC_0);

    for (i = 0; i < IEEE802154_LONG_ADDRESS_LEN; i++)
    {
        if (eui64[i] != CCFG_UNKNOWN_EUI64)
        {
            break;
        }
    }

    if (i >= IEEE802154_LONG_ADDRESS_LEN)
    {
        /* The ccfg address was all 0xFF, switch to the fcfg */
        eui64 = (uint8_t *)(FCFG1_BASE + FCFG1_O_MAC_15_4_0);
    }

    /*
     * The IEEE MAC address is stored in network byte order.  The caller seems
     * to want the address stored in little endian format, which is backwards
     * of the conventions setup by @ref rfSetExtendedAddress.
     * rfSetExtendedAddress assumes that the address being passed to
     * it is in network byte order, so the caller of
     * rfSetExtendedAddress must swap the endianness before calling.
     *
     * It may be easier to have the caller of this function store the IEEE
     * address in network byte order.
     */
    for (i = 0; i < IEEE802154_LONG_ADDRESS_LEN; i++)
    {
        aIeeeEui64[i] = eui64[(IEEE802154_LONG_ADDRESS_LEN - 1) - i];
    }
}

void cc13x2_prop_rf_irq_set_handler(void(*handler)(void *), void * arg)
{
    _irq_handler = handler;
    _irq_handler_arg = arg;
}

int cc13x2_prop_rf_recv(void *buf, size_t len, netdev_ieee802154_rx_info_t *rx_info)
{
    uint_fast8_t available = 0;
    rfc_dataEntryGeneral_t *start_entry = (rfc_dataEntryGeneral_t *)_rx_data_queue.pCurrEntry;
    rfc_dataEntryGeneral_t *cur_entry = start_entry;

    /* loop through receive queue */
    do {
        if (cur_entry->status == DATA_ENTRY_FINISHED ||
            cur_entry->status == DATA_ENTRY_BUSY) {
            available = 1;
            break;
        }

        cur_entry = (rfc_dataEntryGeneral_t *)(cur_entry->pNextEntry);
    } while (cur_entry != start_entry);

    if (!available) {
        return 0;
    }

    /* wait for entry to become finished */
    /* TODO: timeout */
    while (cur_entry->status == DATA_ENTRY_BUSY) {}

    /*
     * First 2 bytes in the data entry are the length.
     * Our data entry consists of:
     *
     * +----------------+---------+------+-----------+--------+
     * | Payload Length | Payload | RSSI | Timestamp | Status |
     * +----------------+---------+------+-----------+--------+
     *                    |          |         |         |
     *                    |          |         |         1 byte
     *                    |          |         4 bytes
     *                    |          1 byte (signed)
     *                    Payload Length
     *
     * This length includes all of those.
     */
    uint8_t *payload = &(cur_entry->data);
    uint16_t payload_len = *((uint16_t *)payload);

    if(len <= sizeof(rfc_dataEntry_t)) {
        puts("cc13x2_prop_rf_recv: too short!");
        cur_entry->status = DATA_ENTRY_PENDING;
        return 0;
    }

    int8_t rssi = (int8_t)payload[payload_len];
    uint8_t lqi = payload[payload_len + 5];

    if (!buf) {
        if (len == 0) {
            return payload_len;
        }
        else {
            cur_entry->status = DATA_ENTRY_PENDING;
            return 0;
        }
    }

    /* Check if payload is of standard size. */
    if ((payload_len - 2) > 144) {
        puts("cc13x2_prop_rf_recv: packet too big!");
        cur_entry->status = DATA_ENTRY_PENDING;
        return 0;
    }

    if (payload_len > len) {
        return -ENOSPC;
    }

    /* Subtract checksum bytes. */
    payload_len -= 2;

    /* Copy packet data. */
    memcpy(buf, &(payload[2]), payload_len);

    /* Save RX information. */
    if (rx_info) {
        rx_info->rssi = rssi;
        rx_info->lqi = lqi;
    }

    return payload_len;
}

int cc13x2_prop_rf_recv_avail(void)
{
    rfc_dataEntryGeneral_t *curEntry, *startEntry;

    startEntry = (rfc_dataEntryGeneral_t *)_rx_data_queue.pCurrEntry;
    curEntry   = startEntry;

    /* loop through receive queue */
    do {
        if (curEntry->status == DATA_ENTRY_FINISHED) {
            return 1;
        }

        curEntry = (rfc_dataEntryGeneral_t *)(curEntry->pNextEntry);
    } while (curEntry != startEntry);
    return 0;
}

int cc13x2_prop_rf_send(const iolist_t *iolist)
{
    unsigned error = -EAGAIN;

    if (_cc13x2_prop_rf_state == cc13x2_stateReceive)
    {
        size_t len = 0;
        uint8_t *bufpos = _tx_buf;

        for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
            len += iol->iol_len;
            if (len > TX_BUF_SIZE) {
                error = -EOVERFLOW;
                goto exit;
            }

            memcpy(bufpos, iol->iol_base, iol->iol_len);
            bufpos += iol->iol_len;
        }

        _cc13x2_prop_rf_state = cc13x2_stateTransmit;

        if (!(cc13x2_prop_rf_send_tx_cmd(_tx_buf, len) == CMDSTA_Done)) {
            error = -1;
            goto exit;
        }

        return len;
    }

exit:
    return error;
}

unsigned cc13x2_prop_rf_irq_is_enabled(unsigned irq)
{
    return (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) & irq) != 0;
}

void cc13x2_prop_rf_irq_enable(unsigned irq)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~irq;
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) |= irq;
}

void cc13x2_prop_rf_irq_disable(unsigned irq)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) &= ~irq;
}

unsigned cc13x2_prop_rf_get_flags(void)
{
    unsigned flags = _irq_handler_flags;
    _irq_handler_flags = 0;
    return flags;
}
