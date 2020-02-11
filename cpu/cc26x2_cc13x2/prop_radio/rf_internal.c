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
#include "cc13x2_prop_rf_commands.h"
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
#include <rf_patches/rf_patch_cpe_prop.h>


#define ENABLE_DEBUG (1)

#if ENABLE_DEBUG == 1
#undef FLASH_BASE
#undef GPIO_BASE
#undef GPT0_BASE
#undef GPT1_BASE
#undef GPT2_BASE
#undef GPT3_BASE
#undef AON_IOC_BASE
#undef UART0_BASE
#undef UART1_BASE

#include "debug.h"
#endif

#define RF_CMD0 (0x0607) /* RF Core CMD0 */

static volatile cc13x2_prop_rf_state_t _state;

/* set to max transmit power by default */
static output_config_t const *_current_tx_power = &(output_power_table[0]);

static uint32_t _prop_overrides[] =
{
    (uint32_t)0x00F788D3,
    ADI_2HALFREG_OVERRIDE(0, 16, 0x8, 0x8, 17, 0x1, 0x1),
    HW_REG_OVERRIDE(0x609C, 0x001A),
    (uint32_t)0x000188A3,
    ADI_HALFREG_OVERRIDE(0, 61, 0xF, 0xD),
    HW_REG_OVERRIDE(0x6028, 0x001A),
    (uint32_t)0xFFFFFFFF,
};

/*
 * Offset of the radio timer from the rtc.
 *
 * Used when we start and stop the RAT on enabling and disabling of the rf
 * core.
 */
static uint32_t _rat_offset = 0;

static volatile __attribute__((aligned(4))) rfc_CMD_FS_t _cmd_fs;

static volatile __attribute__((aligned(4))) rfc_CMD_PROP_RX_ADV_t _cmd_receive;
static volatile __attribute__((aligned(4))) rfc_CMD_PROP_TX_ADV_t _cmd_transmit;

static volatile uint16_t _channel;

static __attribute__((aligned(4))) rfc_propRxOutput_t _rf_stats;

#define ALIGN_TO_4(size) (((size) + 3) & ~3)

#define CC13X2_LENGTH_OFFSET (sizeof(rfc_dataEntry_t))
#define CC13X2_DATA_OFFSET   (CC13X2_LENGTH_OFFSET + IEEE802154_PHR_SIZE)

#define CC13X2_MAX_PACKET_SIZE  (2047)
#define CC13X2_MAX_PAYLOAD_SIZE (125)

#define BUF_SIZE                         \
    ALIGN_TO_4(CC13X2_DATA_OFFSET      + \
               CC13X2_MAX_PAYLOAD_SIZE + \
               CC13X2_METADATA_SIZE)

/*
 * Two receive buffers entries with room for 1 max IEEE802.15.4 frame in each
 *
 * These will be setup in a circular buffer configuration by /ref _rx_data_queue.
 */
static __attribute__((aligned(4))) uint8_t _rx_buf0[BUF_SIZE];
static __attribute__((aligned(4))) uint8_t _rx_buf1[BUF_SIZE];
static __attribute__((aligned(4))) uint8_t _rx_buf2[BUF_SIZE];
static __attribute__((aligned(4))) uint8_t _rx_buf3[BUF_SIZE];

static uint8_t _tx_buf[BUF_SIZE] __attribute__((aligned(4)));

volatile unsigned _irq_handler_flags;

/*
 * The RX Data Queue used by @ref _cmd_receive.
 */
static __attribute__((aligned(4))) dataQueue_t _rx_data_queue = { 0 };

/* network stack ISR handlers */
static void (*_irq_handler)(void *);
static void *_irq_handler_arg;

/**
 * @brief   Clear an interrupt flag from the RFCPEIFG register.
 *
 * @param [in] irqn The interrupt.
 */
static void clear_interrupt_flag(uint32_t irqn)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~irqn;
}

/**
 * @brief   Checks if an interrupt flag is present.
 *
 * @param [in] irqn The interrupt.
 */
static bool is_interrupt_flag_present(uint32_t irqn)
{
    return (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & irqn) == irqn;
}

/**
 * @brief   Initialize the RX/TX buffers.
 *
 *          Zeros out the receive and transmit buffers and sets up the data
 *          structures of the receive queue.
 */
static void cc13x2_prop_rf_init_bufs(void)
{
    rfc_dataEntry_t *entry;

    memset(_rx_buf0, 0, sizeof(_rx_buf0));
    memset(_rx_buf1, 0, sizeof(_rx_buf1));
    memset(_rx_buf2, 0, sizeof(_rx_buf2));
    memset(_rx_buf3, 0, sizeof(_rx_buf3));

    entry = (rfc_dataEntry_t *)_rx_buf0;
    entry->pNextEntry = _rx_buf1;
    entry->config.lenSz = sizeof(uint16_t);
    entry->length = sizeof(_rx_buf0) - sizeof(rfc_dataEntry_t);

    entry = (rfc_dataEntry_t *)_rx_buf1;
    entry->pNextEntry = _rx_buf2;
    entry->config.lenSz = sizeof(uint16_t);
    entry->length = sizeof(_rx_buf1) - sizeof(rfc_dataEntry_t);

    entry = (rfc_dataEntry_t *)_rx_buf2;
    entry->pNextEntry = _rx_buf3;
    entry->config.lenSz = sizeof(uint16_t);
    entry->length = sizeof(_rx_buf2) - sizeof(rfc_dataEntry_t);

    entry = (rfc_dataEntry_t *)_rx_buf3;
    entry->pNextEntry = _rx_buf0;
    entry->config.lenSz = sizeof(uint16_t);
    entry->length = sizeof(_rx_buf3) - sizeof(rfc_dataEntry_t);
}

static void cc13x2_rf_core_init_rx_params(void)
{
    static const rfc_CMD_PROP_RX_ADV_t cmd_receive_default = {
        .commandNo = CMD_PROP_RX_ADV,
        .status = IDLE,
        .pNextOp = NULL,
        .startTime = 0u,
        .startTrigger = {
            .triggerType = TRIG_NOW,
        },
        .condition = {
            .rule = COND_NEVER,
        },
        .pktConf = {
            .bFsOff = 0,
            .bRepeatOk = 0,
            .bRepeatNok = 0,
            .bUseCrc = 0x1,
            .bCrcIncSw = 0,
            .bCrcIncHdr = 0,
            .endType = 0,
            .filterOp = 0,
        },
        .rxConf = {
            .bAutoFlushIgnored = 0x1,
            .bAutoFlushCrcErr = 0x1,
            .bIncludeHdr = 0,
            .bIncludeCrc = 0,
            .bAppendRssi = 0x1,
            .bAppendTimestamp = 0,
            .bAppendStatus = 0x1,
        },
        .syncWord0 = IEEE802154_2FSK_UNCODED_SFD_0,
        .syncWord1 = 0,
        .maxPktLen = CC13X2_MAX_PACKET_SIZE,
        .hdrConf = {
            .numHdrBits = IEEE802154_PHR_BITS,
            .lenPos = 0,
            .numLenBits = IEEE802154_PHR_FRAME_LENGTH_BITS,
        },
        .addrConf = {
            .addrType = 0,
            .addrSize = 0,
            .addrPos = 0,
            .numAddr = 0,
        },
        .lenOffset = 0,
        .endTrigger = {
            .triggerType = TRIG_NEVER,
            .bEnaCmd = 0x0,
            .triggerNo = 0x0,
            .pastTrig = 0x0,
        },
        .endTime = 0x00000000,
        .pAddr = 0,
        .pQueue = 0,
        .pOutput = 0
    };

    _cmd_receive = cmd_receive_default;

    _cmd_receive.pQueue = &_rx_data_queue;
    _cmd_receive.pOutput = (uint8_t *)&_rf_stats;
}

/**
 * @brief   Sends the immediate clear RX queue command to the RF Core.
 *
 *          Uses the radio core to mark all of the entries in the receive queue
 *          as pending. This is used instead of clearing the entries manually
 *          to avoid race conditions between the main processor and the radio
 *          core.
 *
 * @param [in] queue A pointer to the receive queue to be cleared.
 *
 * @return  The value from the command status register.
 * @retval  CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t cc13x2_prop_rf_clear_rx_queue(dataQueue_t *queue)
{
    uint32_t cmd_clear_rx  = cc13x2_cmd_clear_rx(queue);
    return RFCDoorbellSendTo(cmd_clear_rx) & 0xFF;
}

/**
 * @brief   Sends the tx command to the radio core.
 *
 *          Sends the packet to the radio core to be sent asynchronously.
 *
 * @note @ref psdu *must* be 4 byte aligned and not include the FCS.
 *
 * @param [in] psdu A pointer to the data to be sent.
 * @param [in] len  The length in bytes of data pointed to by PSDU.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t cc13x2_prop_rf_send_tx_cmd(uint8_t *psdu, uint8_t len)
{
    static const rfc_CMD_PROP_TX_ADV_t cmd_transmit_default = {
        .commandNo = CMD_PROP_TX_ADV,
        .status = IDLE,
        .startTrigger = {
            .triggerType = TRIG_NOW,
        },
        .condition = {
            .rule = COND_NEVER,
        },
        .pNextOp = NULL,
        .pktConf = {
            .bFsOff = 0,
            .bUseCrc = 0,
            .bCrcIncSw = 0,
            .bCrcIncHdr = 0,
        },
        .numHdrBits = IEEE802154_PHR_BITS,
        .startConf = {
            .bExtTxTrig = 0,
            .inputMode = 0,
            .source = 0,
        },
        .preTrigger = {
            .triggerType = TRIG_REL_START,
            .bEnaCmd = 0,
            .triggerNo = 0,
            .pastTrig = 1,
        },
        .preTime = 0x00000000,
        .syncWord = IEEE802154_2FSK_UNCODED_SFD_0,
    };

    DEBUG_PUTS("[cc13x2_prop_rf_send_tx_cmd]: sending TX command.");
    /*DEBUG("[cc13x2_prop_rf_send_tx_cmd]: psdu = %lx.\n", (uint32_t)psdu);
    DEBUG("[cc13x2_prop_rf_send_tx_cmd]: len = %x.\n", len);*/

    _cmd_transmit = cmd_transmit_default;
    /* no need to look for an ack if the tx operation was stopped */
    _cmd_transmit.pktLen = IEEE802154_PHR_SIZE + len;
    _cmd_transmit.pPkt = psdu;

    return RFCDoorbellSendTo((uint32_t)&_cmd_transmit) & 0xFF;
}

/**
 * @brief   Sends the RX command to the RF Core.
 *
 *          Sends the pre-built receive command to the radio core. This sets up
 *          the radio to receive packets according to the settings in the
 *          global RX command.
 *
 * @note    This function does not alter any of the parameters of the RX
 *          command.
 *          It is only concerned with sending the command to the RF Core.
 *
 * @return  The value from the command status register.
 * @retval  CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t cc13x2_prop_rf_send_rx_cmd(void)
{
    DEBUG_PUTS("[cc13x2_prop_rf_send_rx_cmd]: sending receive cmd!");
    _cmd_receive.status = IDLE;
    return (RFCDoorbellSendTo((uint32_t)&_cmd_receive) & 0xFF);
}

/**
 * @brief   Send Frequency Synthesize command to the RF Core.
 *
 * @param [in] frequency  The frequency in MHz.
 * @param [in] fract_freq The fractional part of the frequency.
 *
 * @return  The value from the command status register.
 * @retval  CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t cc13x2_prop_rf_send_fs_cmd(uint16_t frequency,
                                               uint16_t fract_freq)
{
    static const rfc_CMD_FS_t cmd_fs_default = {
        .commandNo = CMD_FS,
        .status = IDLE,
        .startTrigger = {
            .triggerType = TRIG_NOW,
        },
        .condition = {
            .rule = COND_NEVER,
        },
        .synthConf = {
            .bTxMode = 0,
            .refFreq = 0,
        },
    };

    DEBUG_PUTS("[cc13x2_prop_rf_send_fs_cmd]: sending FS command");
    /*DEBUG("[cc13x2_prop_rf_send_fs_cmd]: frequency = %u.%u MHz\n", frequency, fract_freq);*/

    _cmd_fs = cmd_fs_default;

    _cmd_fs.frequency = frequency;
    _cmd_fs.fractFreq = fract_freq;

    uint_fast8_t ret = RFCDoorbellSendTo((uint32_t)&_cmd_fs) & 0xFF;

    if (ret != CMDSTA_Done) {
        DEBUG_PUTS("[cc13x2_prop_rf_send_fs_cmd]: command not done");
    }

    return ret;
}

static uint_fast8_t cc13x2_prop_rf_apply_patch(void)
{
    uint_fast8_t ret;
    ret = RFCDoorbellSendTo(CMDR_DIR_CMD_2BYTE(RF_CMD0, RFC_PWR_PWMCLKEN_MDMRAM | RFC_PWR_PWMCLKEN_RFERAM)) & 0xFF;
    if (ret != CMDSTA_Done) {
        return ret;
    }

    /* Patch the CPE */
    rf_patch_cpe_prop();

    /* Disable ram bus clocks */
    return RFCDoorbellSendTo(CMDR_DIR_CMD_2BYTE(RF_CMD0, 0)) & 0xFF;
}

/**
 * @brief   Sends the setup command string to the RF Core.
 *
 *          Enables the clock line from the RTC to the RF core RAT. Enables the
 *          RAT timer and sets up the radio in IEEE mode.
 *
 * @return  The value from the command status register.
 * @retval  CMDSTA_Done The command was received.
 */
static uint_fast16_t cc13x2_prop_rf_send_enable_cmd(void)
{
    uint_fast16_t ret;

    /* Turn on the clock line to the radio core */
    HWREGBITW(AON_RTC_BASE + AON_RTC_O_CTL, AON_RTC_CTL_RTC_UPD_EN_BITN) = 1;

    bool ints_disabled = IntMasterDisable();

    uint32_t cmd_setup = cc13x2_cmd_prop_radio_div_setup(_current_tx_power->value, _prop_overrides);
    uint32_t cmd_start_rat = cc13x2_cmd_sync_start_rat(cmd_setup, _rat_offset);

    DEBUG_PUTS("[cc13x2_prop_rf_send_enable_cmd]: patching RF Core");
    ret = cc13x2_prop_rf_apply_patch();
    if (ret != CMDSTA_Done) {
        goto exit;
    }

    /* Execute cmd_start_rat which will execute cmd_setup */
    ret = RFCDoorbellSendTo(cmd_start_rat) & 0xFF;
    if (ret != CMDSTA_Done) {
        goto exit;
    }

    /* Synchronously wait for the RF Core to stop executing */
    while (!is_interrupt_flag_present(IRQ_LAST_COMMAND_DONE)) {}

    /* Return the status of the cmd_setup command */
    ret = cc13x2_cmd_get_status(cmd_setup);

exit:
    if (!ints_disabled) {
        IntMasterEnable();
    }

    return ret;
}

/**
 * @brief   Sends the shutdown command string to the radio core.
 *
 *          Powers down the frequency synthesizer and stops the RAT.
 *
 * @note    Synchronously waits until the command string completes.
 *
 * @return  The status of the RAT stop command.
 * @retval  DONE_OK The command string executed properly.
 */
static uint_fast16_t cc13x2_prop_rf_send_disable_cmd(void)
{
    uint_fast16_t ret;

    DEBUG_PUTS("[cc13x2_prop_rf_send_disable_cmd]: sending disable command string.");

    HWREGBITW(AON_RTC_BASE + AON_RTC_O_CTL, AON_RTC_CTL_RTC_UPD_EN_BITN) = 0;

    uint32_t cmd_sync_stop_rat = cc13x2_cmd_sync_stop_rat();
    uint32_t cmd_fs_powerdown = cc13x2_cmd_fs_powerdown(cmd_sync_stop_rat);

    bool ints_disabled = IntMasterDisable();

    clear_interrupt_flag(IRQ_LAST_COMMAND_DONE);

    /* Execute command chain */
    ret = RFCDoorbellSendTo(cmd_fs_powerdown) & 0xFF;
    if (ret != CMDSTA_Done) {
        DEBUG_PUTS("[cc13x2_prop_rf_send_disable_cmd]: command string failed.");
        if (!ints_disabled) {
            IntMasterDisable();
        }
        return ret;
    }

    /* Synchronously wait for the RF Core to stop */
    while (!is_interrupt_flag_present(IRQ_LAST_COMMAND_DONE)) {}

    /* Get the status of the finished command */
    ret = cc13x2_cmd_get_status(cmd_sync_stop_rat);
    if (ret == DONE_OK) {
        /* Save the Radio Timer offset */
        _rat_offset = cc13x2_cmd_sync_stop_rat_get_rat0();
    }

    if (!ints_disabled) {
        IntMasterEnable();
    }

    return ret;
}

/**
 * @brief   Error interrupt handler.
 */
static void _isr_rfc_cpe1(void)
{
    DEBUG_PUTS("[_isr_rfc_cpe1]: ERROR!\n");
    /* Clear INTERNAL_ERROR interrupt flag */
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = 0x7FFFFFFF;
}

/**
 * @brief   Command done handler.
 */
static void _isr_rfc_cpe0(void)
{
    if (is_interrupt_flag_present(IRQ_BOOT_DONE)) {
        clear_interrupt_flag(IRQ_BOOT_DONE);
        DEBUG_PUTS("[_isr_rfc_cpe0]: RF Core boot done!");
    }

    if (is_interrupt_flag_present(IRQ_MODULES_UNLOCKED)) {
        clear_interrupt_flag(IRQ_MODULES_UNLOCKED);
        DEBUG_PUTS("[_isr_rfc_cpe0]: RF Core modules unlocked!");
    }

    if (is_interrupt_flag_present(IRQ_TX_DONE)) {
        clear_interrupt_flag(IRQ_TX_DONE);
        DEBUG_PUTS("[_isr_rfc_cpe0]: TX OK!");
        _irq_handler_flags |= 2;

        /* We finished transmitting, now we're sleeping, change state to RX */
        _state = FSM_STATE_SLEEP;
        cc13x2_prop_rf_rx_start();
    }

    if (is_interrupt_flag_present(IRQ_RX_OK)) {
        clear_interrupt_flag(IRQ_RX_OK);
        DEBUG_PUTS("[_isr_rfc_cpe0]: RX OK!");
        _irq_handler_flags |= 1;
    }

    if (_irq_handler_flags != 0) {
        if (_irq_handler) {
            _irq_handler(_irq_handler_arg);
        }
    }

    if (is_interrupt_flag_present(IRQ_LAST_COMMAND_DONE)) {
        clear_interrupt_flag(IRQ_LAST_COMMAND_DONE);
        if (is_interrupt_flag_present(IRQ_COMMAND_DONE)) {
            clear_interrupt_flag(IRQ_COMMAND_DONE);
        }

        if (_state == FSM_STATE_RX &&
            _cmd_receive.status != ACTIVE &&
            _cmd_receive.status != PROP_DONE_RXTIMEOUT) {
            DEBUG_PUTS("[_isr_rfc_cpe0]: RX aborted");
            /* The RX command was aborted */
            _state = FSM_STATE_SLEEP;
        }

        if (_state == FSM_STATE_TX &&
            _cmd_transmit.status == PROP_DONE_OK) {
            DEBUG_PUTS("[_isr_rfc_cpe0]: transmission finished, receiving");
            _state = FSM_STATE_SLEEP;
            cc13x2_prop_rf_rx_start();
        }
    }

    DEBUG_PUTS("[_isr_rfc_cpe0]: finished");
}

void cc13x2_prop_rf_init(void)
{
    cc26xx_cc13xx_set_isr_rfc_cpe0_handler(_isr_rfc_cpe0);
    cc26xx_cc13xx_set_isr_rfc_cpe1_handler(_isr_rfc_cpe1);

    /* Populate the RX parameters data structure with default values */
    cc13x2_rf_core_init_rx_params();

    _state = FSM_STATE_OFF;
}

int_fast8_t cc13x2_prop_rf_power_on(void)
{
    DEBUG("[cc13x2_prop_rf_power_on]: enable RF Core.\n");

    int_fast8_t error = -1;

    if (_state == FSM_STATE_SLEEP) {
        error = 0;
    }
    else if (_state == FSM_STATE_OFF) {

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

        _state = FSM_STATE_SLEEP;
        error = 0;
    }

exit:

    if (error == -1) {
        cc26x2_cc13x2_rf_power_off();
        _state = FSM_STATE_OFF;
    }

    return error;
}

void cc13x2_prop_rf_power_off(void)
{
    if (_state == FSM_STATE_OFF) {
        return;
    }
    else if (_state == FSM_STATE_SLEEP) {
        cc13x2_prop_rf_send_disable_cmd();
        cc26x2_cc13x2_rf_power_off();
        _state = FSM_STATE_OFF;
    }
}

int_fast8_t cc13x2_prop_rf_reset(void)
{
    if (_state == FSM_STATE_OFF) {
        if (cc13x2_prop_rf_power_on() == -1) {
            return -1;
        }

        if (cc13x2_prop_rf_rx_start() == -1) {
            return -1;
        }
    }
    else if (_state == FSM_STATE_SLEEP ||
             _state == FSM_STATE_TX) {
        /* If we're in Sleep mode, start receiving, if we're on the Transmit
         * mode trigger the start of the Receive state, this will abort any
         * transmission being done. */
        if (cc13x2_prop_rf_rx_start() == -1) {
            return -1;
        }
    }
    else if (_state == FSM_STATE_RX) {
        return 0;
    }

    return 0;
}

int8_t cc13x2_prop_rf_get_txpower(void)
{
    return _current_tx_power->dbm;
}

int_fast8_t cc13x2_prop_rf_set_txpower(int8_t power)
{
    unsigned int i;
    output_config_t const *powercfg = &(output_power_table[0]);

    for (i = 1; i < OUTPUT_CONFIG_COUNT; i++) {
        if (output_power_table[i].dbm >= power) {
            powercfg = &(output_power_table[i]);
        }
        else {
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
    if (_state == FSM_STATE_SLEEP) {
        DEBUG_PUTS("[cc13x2_prop_rf_rx_start]: sleeping, changing to RX");
        _state = FSM_STATE_RX;

        /* Initialize the receive command
         * XXX: no memset here because we assume init has been called and we
         *      may have changed some values in the rx command
         */
        if (cc13x2_prop_rf_send_rx_cmd() != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_rx_start]: RX command failed!");
            return -1;
        }
    }
    else if (_state == FSM_STATE_RX && _cmd_receive.status != ACTIVE) {
        DEBUG_PUTS("[cc13x2_prop_rf_rx_start]: RX not active, setting again!");
        /* We have either not fallen back into our receive command or we are
         * running on the wrong channel. Either way assume the caller correctly
         * called us and abort all running commands. */
        if (cc26x2_cc13x2_rf_execute_abort_cmd() != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_rx_start]: abort command failed!");
            return -1;
        }

        /* Any frames in the queue will be for the old channel */
        if (cc13x2_prop_rf_clear_rx_queue(&_rx_data_queue) != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_rx_start]: couldn't clear RX queue!");
            return -1;
        }

        if (cc13x2_prop_rf_send_rx_cmd() != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_rx_start]: send RX command failed!");
            return -1;
        }

        _state = FSM_STATE_RX;
    }

    return 0;
}

int_fast8_t cc13x2_prop_rf_rx_stop(void)
{
    if (_state != FSM_STATE_RX) {
        return 0;
    }

    if (cc26x2_cc13x2_rf_execute_abort_cmd() != CMDSTA_Done) {
        return -1;
    }

    return 0;
}

uint8_t cc13x2_prop_rf_get_chan(void)
{
    return _channel;
}

void cc13x2_prop_rf_set_chan(uint16_t channel, bool force)
{
    DEBUG_PUTS("[cc13x2_prop_rf_set_chan]: setting channel");
    if (_channel == channel && force == false) {
        DEBUG_PUTS("[cc13x2_prop_rf_set_chan]: we are on channel");
        return;
    }

    const uint32_t new_freq = cc13x2_prop_rf_channel_freq(channel);

    uint16_t freq;
    uint16_t frac;
    cc13x2_prop_rf_freq_parts(new_freq, &freq, &frac);

    bool enable_rx = false;

    if (_state == FSM_STATE_RX) {
        cc13x2_prop_rf_rx_stop();
        enable_rx = true;
    } else if (_state == FSM_STATE_TX) {
        /* Stop the transmission and change freq. */
        if (cc26x2_cc13x2_rf_execute_abort_cmd() != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_set_chan]: abort command failed!");
            return;
        }

        /* Switch to Receive after channel change. */
        enable_rx = true;
        _state = FSM_STATE_SLEEP;
    }

    bool ints_disabled = IntMasterDisable();

    if (cc13x2_prop_rf_send_fs_cmd(freq, frac) != CMDSTA_Done) {
        DEBUG_PUTS("[cc13x2_prop_rf_set_chan]: couldn't change channel");
        return;
    }

    if (!ints_disabled) {
        IntMasterEnable();
    }

    _channel = channel;

    if (enable_rx) {
        cc13x2_prop_rf_rx_start();
    }

    return;
}

void cc13x2_prop_rf_get_ieee_eui64(uint8_t *ieee_eui64)
{
    uint8_t *eui64;
    unsigned int i;

    /*
     * The IEEE MAC address can be stored two places. We check the Customer
     * Configuration was not set before defaulting to the Factory
     * Configuration.
     */
    eui64 = (uint8_t *)(CCFG_BASE + CCFG_O_IEEE_MAC_0);

    for (i = 0; i < IEEE802154_LONG_ADDRESS_LEN; i++) {
        if (eui64[i] != CCFG_UNKNOWN_EUI64) {
            break;
        }
    }

    if (i >= IEEE802154_LONG_ADDRESS_LEN) {
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
    for (i = 0; i < IEEE802154_LONG_ADDRESS_LEN; i++) {
        ieee_eui64[i] = eui64[(IEEE802154_LONG_ADDRESS_LEN - 1) - i];
    }
}

void cc13x2_prop_rf_irq_set_handler(void (*handler)(void *), void *arg)
{
    _irq_handler = handler;
    _irq_handler_arg = arg;
}

int cc13x2_prop_rf_recv(void *buf, size_t len,
                        netdev_ieee802154_rx_info_t *rx_info)
{
    DEBUG_PUTS("[cc13x2_prop_rf_recv]: receiving");

    uint_fast8_t available = 0;
    rfc_dataEntryGeneral_t *start_entry =
        (rfc_dataEntryGeneral_t *)_rx_data_queue.pCurrEntry;
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
     * +----------------+---------+------+--------+
     * | Payload Length | Payload | RSSI | Status |
     * +----------------+---------+------+--------+
     *                    |          |         |
     *                    |          |         1 byte
     *                    |          1 byte (signed)
     *                    Payload Length
     *
     * This length includes all of those.
     */
    uint8_t *payload = &(cur_entry->data);
    uint16_t payload_len = *((uint16_t *)payload);

    if (payload_len <= CC13X2_METADATA_SIZE) {
        DEBUG_PUTS("[cc13x2_prop_rf_recv]: too short!");
        cur_entry->status = DATA_ENTRY_PENDING;
        return 0;
    }

    payload += 2;
    payload_len -= CC13X2_METADATA_SIZE;

    int8_t rssi = (int8_t)payload[payload_len];
    uint8_t lqi = payload[payload_len + CC13X2_RSSI_SIZE];

    printf("[cc132_prop_rf_recv]: rssi = %d, lqi = %u", rssi, lqi);

    if (!buf) {
        if (len == 0) {
            return payload_len;
        }
        else {
            cur_entry->status = DATA_ENTRY_PENDING;
            return 0;
        }
    }

    if (payload_len > len) {
        return -ENOSPC;
    }

    /* Copy packet data. */
    memcpy(buf, payload, payload_len);

    /* Save RX information. */
    if (rx_info) {
        rx_info->rssi = rssi;
        rx_info->lqi = lqi;
    }

    return payload_len;
}

bool cc13x2_prop_rf_recv_avail(void)
{
    rfc_dataEntryGeneral_t *start_entry =
        (rfc_dataEntryGeneral_t *)_rx_data_queue.pCurrEntry;
    rfc_dataEntryGeneral_t *cur_entry = start_entry;

    /* Loop through receive queue */
    do {
        if (cur_entry->status == DATA_ENTRY_FINISHED) {
            return true;
        }

        cur_entry = (rfc_dataEntryGeneral_t *)(cur_entry->pNextEntry);
    } while (cur_entry != start_entry);

    return false;
}

int cc13x2_prop_rf_send(const iolist_t *iolist)
{
    if (_state == FSM_STATE_RX) {
        DEBUG_PUTS("[cc13x2_prop_rf_send]: we are in receive state");
        size_t len = 0;
        /* Reserve the first bytes of the PHR */
        uint8_t *bufpos = _tx_buf + IEEE802154_PHR_SIZE;

        for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
            len += iol->iol_len;
            if (len > (BUF_SIZE - IEEE802154_PHR_SIZE)) {
                DEBUG_PUTS("[cc13x2_prop_rf_send]: payload is too big!");
                return -EOVERFLOW;
            }

            memcpy(bufpos, iol->iol_base, iol->iol_len);
            bufpos += iol->iol_len;
        }

        /* Length in .15.4g PHY HDR. Includes the CRC but not the HDR itself */
        const uint16_t total_length = len + sizeof(uint16_t);
        /*
         * Prepare the .15.4g PHY header
         * MS=0, Length MSBits=0, DW and CRC configurable
         * Total length = len (payload) + CRC length
         *
         * The Radio will flip the bits around, so _tx_buf[0] must have the length
         * LSBs (PHR[15:8] and _tx_buf[1] will have PHR[7:0]
         */
        _tx_buf[0] = ((total_length >> 0) & 0xFF);
        _tx_buf[1] = ((total_length >> 8) & 0xFF) + 0x8 + 0x0;

        for (size_t i = 0; i < len; i++) {
            DEBUG("%#02x, ", _tx_buf[i]);
        }
        DEBUG("\n");


        DEBUG_PUTS("[cc13x2_prop_rf_send]: stop RX.");
        if (cc13x2_prop_rf_rx_stop() == -1) {
            DEBUG_PUTS("[cc13x2_prop_rf_send]: couldn't stop RX");
            return -EIO;
        }

        DEBUG_PUTS("[cc13x2_prop_rf_send]: switching state to transmit");

        _state = FSM_STATE_TX;
        if (cc13x2_prop_rf_send_tx_cmd(_tx_buf, len) != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_send]: TX send failed!");
            _state = FSM_STATE_SLEEP;
            return -EIO;
        }

        return len;
    }

    /* We are busy, try again next time */
    return -EAGAIN;
}

unsigned cc13x2_prop_rf_irq_is_enabled(unsigned irq)
{
    return (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) & irq) != 0;
}

void cc13x2_prop_rf_irq_enable(unsigned irq)
{
    /*DEBUG("[cc13x2_prop_rf_irq_enable]: %x\n", irq);*/
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) &= ~irq;
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) |= irq;
}

void cc13x2_prop_rf_irq_disable(unsigned irq)
{
    /*DEBUG("[cc13x2_prop_rf_irq_enable]: %x\n", irq);*/
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) &= ~irq;
}

unsigned cc13x2_prop_rf_get_flags(void)
{
    unsigned flags = _irq_handler_flags;

    _irq_handler_flags = 0;
    return flags;
}

cc13x2_prop_rf_state_t cc13x2_prop_rf_get_state(void)
{
    return _state;
}
