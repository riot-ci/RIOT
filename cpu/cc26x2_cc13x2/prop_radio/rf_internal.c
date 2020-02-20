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
#include "cc13x2_prop_rf_params.h"
#include "cc26x2_cc13x2_rfc.h"

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

#define ALIGN(n) __attribute__((aligned(n))) /**< Alignment attribute */

#define ALIGN_TO_4(size) (((size) + 3) & ~3)

#define CC13X2_LENGTH_OFFSET (sizeof(rfc_dataEntry_t))
#define CC13X2_DATA_OFFSET   (CC13X2_LENGTH_OFFSET + IEEE802154_PHR_SIZE)

#define CC13X2_MAX_PAYLOAD_SIZE (125)

#define BUF_SIZE                         \
    ALIGN_TO_4(CC13X2_DATA_OFFSET      + \
               CC13X2_MAX_PAYLOAD_SIZE + \
               CC13X2_METADATA_SIZE)

/**
 * @brief   RF Core status structure
 */
typedef struct {
    volatile cc13x2_prop_rf_state_t state; /**< RF Core current state */
    volatile output_config_t const *tx_power; /**< Transmit power current
                                                   configuration */
    volatile uint32_t rat_offset; /**< Radio Timer last offset, used when we
                                       start/stop the RAT on enabling and
                                       disabling of the RF Core */
    volatile uint16_t channel; /**< Current channel */
    volatile cc13x2_prop_rf_irq_flags_t irq_handler_flags; /**< IRQ handler
                                                                flags */
    void (* irq_handler)(void *); /**< Netdev IRQ handler */
    void *irq_handler_arg; /**< Netdev IRQ handler argument */
} rf_core_t;

static volatile rf_core_t _rf_core; /**< Driver internal status */

static ALIGN(4) rfc_propRxOutput_t _rf_stats; /**< RX statistics */

/**
 * @brief   RX buffers; each one being an entry in the @ref _rx_data_queue.
 *          If needed more buffers _could_ be added as necessary by updating
 *          the necessary code in @ref init_bufs.
 * @{
 */
static ALIGN(4) uint8_t _rx_buf0[BUF_SIZE]; /**< RX buffer. Entry 0 */
static ALIGN(4) uint8_t _rx_buf1[BUF_SIZE]; /**< RX buffer. Entry 1 */
static ALIGN(4) uint8_t _rx_buf2[BUF_SIZE]; /**< RX buffer. Entry 2 */
static ALIGN(4) uint8_t _rx_buf3[BUF_SIZE]; /**< RX buffer. Entry 3 */
/** @} */

static ALIGN(4) uint8_t _tx_buf[BUF_SIZE]; /**< The transmit buffer */

static ALIGN(4) dataQueue_t _rx_data_queue; /**< The RX data queue */

/**
 * @brief   Setup the interrupts for the CPE.
 *
 *          - IRQ_INTERNAL_ERROR is mapped to isr_rfc_cpe1.
 *          - IRQ_LAST_COMMAND_DONE are mapped to
 *          isr_rfc_cpe0.
 */
static void setup_interrupts(void)
{
    assert(PRCMRfReady());

    bool ints_disabled = IntMasterDisable();

    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = 0x0;
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) = 0x0;

    /* Set all interrupt channels to CPE0 channel, error to CPE1 */
    RFCCpe1IntSelect(IRQ_INTERNAL_ERROR);
    RFCCpe0IntSelect(0x7FFFFFFF);

    RFCCpeIntEnable(IRQ_LAST_COMMAND_DONE);

    IntPendClear(INT_RFC_CPE_0);
    IntPendClear(INT_RFC_CPE_1);
    IntEnable(INT_RFC_CPE_0);
    IntEnable(INT_RFC_CPE_1);

    if (!ints_disabled) {
        IntMasterEnable();
    }
}

/**
 * @brief   Stop the CPE interrupts.
 */
static void stop_interrupts(void)
{
    bool ints_disabled = IntMasterDisable();

    /* clear and disable interrupts */
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = 0x0;
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) = 0x0;

    IntUnregister(INT_RFC_CPE_0);
    IntUnregister(INT_RFC_CPE_1);
    IntPendClear(INT_RFC_CPE_0);
    IntPendClear(INT_RFC_CPE_1);
    IntDisable(INT_RFC_CPE_0);
    IntDisable(INT_RFC_CPE_1);

    if (!ints_disabled)
    {
        IntMasterEnable();
    }
}

/**
 * @brief   Initialize the RX/TX buffers.
 *
 *          Zeros out the receive and transmit buffers and sets up the data
 *          structures of the receive queue.
 */
static void init_bufs(void)
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
static uint_fast8_t clear_rx_queue(dataQueue_t *queue)
{
    uint32_t cmd_clear_rx = cc13x2_cmd_clear_rx(queue);
    return cc13x2_dbell_execute(cmd_clear_rx);
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
    uint16_t packet_len = IEEE802154_PHR_SIZE + len;
    uint32_t cmd_prop_tx_adv = cc13x2_cmd_prop_tx_adv(psdu, packet_len);

    return cc13x2_dbell_execute(cmd_prop_tx_adv);
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
    uint32_t cmd_prop_rx_adv =
        cc13x2_cmd_prop_rx_adv(&_rx_data_queue, &_rf_stats);
    return cc13x2_dbell_execute(cmd_prop_rx_adv);
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
static uint_fast8_t cc13x2_prop_rf_send_fs_cmd(uint16_t freq,
                                               uint16_t fract_freq)
{
    DEBUG_PUTS("[cc13x2_prop_rf_send_fs_cmd]: sending FS command");

    uint32_t cmd_fs = cc13x2_cmd_fs(0, freq, fract_freq);

    return cc13x2_dbell_execute(cmd_fs);
}

/**
 * @brief    Apply CPE patch for propietary mode.
 *
 *           This updates the internal code of the CPE with fixes from TI.
 *
 * @return   The value of the last command called.
 * @retval   CMDSTA_Done The patch was applied successfully.
 */
static uint_fast8_t cc13x2_prop_rf_apply_patch(void)
{
    uint_fast8_t ret;
    uint32_t cmd_cmd0 = cc13x2_cmd_cmd0(RFC_PWR_PWMCLKEN_MDMRAM |
                                        RFC_PWR_PWMCLKEN_RFERAM);
    ret = cc13x2_dbell_execute(cmd_cmd0);
    if (ret != CMDSTA_Done) {
        return ret;
    }

    /* Patch the CPE */
    rf_patch_cpe_prop();

    /* Disable ram bus clocks */
    cmd_cmd0 = cc13x2_cmd_cmd0(0);
    return cc13x2_dbell_execute(cmd_cmd0);
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

    bool ints_disabled = IntMasterDisable();

    /* Turn on the clock line to the radio core */
    HWREGBITW(AON_RTC_BASE + AON_RTC_O_CTL, AON_RTC_CTL_RTC_UPD_EN_BITN) = 1;

    uint32_t cmd_setup =
        cc13x2_cmd_prop_radio_div_setup(_rf_core.tx_power->value);
    uint32_t cmd_start_rat =
        cc13x2_cmd_sync_start_rat(cmd_setup, _rf_core.rat_offset);

    DEBUG_PUTS("[cc13x2_prop_rf_send_enable_cmd]: patching RF Core");
    ret = cc13x2_prop_rf_apply_patch();
    if (ret != CMDSTA_Done) {
        goto exit;
    }

    /* Execute cmd_start_rat which will execute cmd_setup */
    ret = cc13x2_dbell_execute(cmd_start_rat);
    if (ret != CMDSTA_Done) {
        goto exit;
    }

    /* Synchronously wait for the RF Core to stop executing */
    while ((HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIFG) &
            IRQ_LAST_COMMAND_DONE) == 0) {}

    /* Remove flag */
    RFCCpeIntClear(IRQ_LAST_COMMAND_DONE);

    /* Return the status of the cmd_setup command */
    ret = cc13x2_cmd_prop_radio_div_setup_status();

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

    HWREGBITW(AON_RTC_BASE + AON_RTC_O_CTL, AON_RTC_CTL_RTC_UPD_EN_BITN) = 0;

    uint32_t cmd_sync_stop_rat = cc13x2_cmd_sync_stop_rat();
    uint32_t cmd_fs_powerdown = cc13x2_cmd_fs_powerdown(cmd_sync_stop_rat);

    bool ints_disabled = IntMasterDisable();

    RFCCpeIntClear(IRQ_LAST_COMMAND_DONE);

    /* Execute command chain */
    ret = cc13x2_dbell_execute(cmd_fs_powerdown);
    if (ret != CMDSTA_Done) {
        if (!ints_disabled) {
            IntMasterDisable();
        }
        return ret;
    }

    /* Synchronously wait for the RF Core to stop */
    while ((HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIFG) &
            IRQ_LAST_COMMAND_DONE) == 0) {}

    /* Get the status of the CMD_SYNC_STOP_RAT command */
    ret = cc13x2_cmd_sync_stop_rat_get_status();
    if (ret == DONE_OK) {
        /* Save the Radio Timer offset */
        _rf_core.rat_offset = cc13x2_cmd_sync_stop_rat_get_rat0();
    }

    if (!ints_disabled) {
        IntMasterEnable();
    }

    return ret;
}

/**
 * @brief   Error interrupt handler.
 */
void isr_rfc_cpe1(void)
{
    DEBUG_PUTS("[isr_rfc_cpe1]: ERROR!\n");
    /* Clear INTERNAL_ERROR interrupt flag */
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = 0x7FFFFFFF;
}

/**
 * @brief   Command done handler.
 */
void isr_rfc_cpe0(void)
{
#if ENABLE_DEBUG == 1
    printf("CPE INTS: %08lx\n",
           HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG));
    printf("HW INTS: %08lx\n",
           HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFHWIFG));
#endif

    if (RFCCpeIntGetAndClear(IRQ_BOOT_DONE)) {
        DEBUG_PUTS("[isr_rfc_cpe0]: RF Core boot done!");
    }

    if (RFCCpeIntGetAndClear(IRQ_MODULES_UNLOCKED)) {
        DEBUG_PUTS("[isr_rfc_cpe0]: RF Core modules unlocked!");
    }

    RFCCpeIntGetAndClear(IRQ_COMMAND_DONE);

    if (RFCCpeIntGetAndClear(IRQ_LAST_COMMAND_DONE)) {

        if (_rf_core.state == FSM_STATE_RX) {
            uint16_t rx_status = cc13x2_cmd_prop_rx_adv_get_status();
            switch (rx_status) {
                case PROP_DONE_RXERR:
                    cc13x2_prop_rf_rx_start();
                    break;

                case PROP_DONE_ABORT:
                    DEBUG_PUTS("[isr_rfc_cpe0]: RX aborted");
                    /* RX command aborted */
                    _rf_core.state = FSM_STATE_SLEEP;
                    break;
            }
        } else if (_rf_core.state == FSM_STATE_TX) {
            uint16_t tx_status = cc13x2_cmd_prop_tx_adv_get_status();

            if (tx_status == PROP_DONE_OK) {
                DEBUG_PUTS("[isr_rfc_cpe0]: transmission finished, receiving");
                _rf_core.state = FSM_STATE_RX;
                cc13x2_prop_rf_rx_start();
            }
        }
    }

    if (RFCCpeIntGetAndClear(IRQ_TX_DONE)) {
        DEBUG_PUTS("[isr_rfc_cpe0]: TX OK!");

        /* We finished transmitting, now we're sleeping, change state to RX */
        _rf_core.state = FSM_STATE_RX;
        _rf_core.irq_handler_flags |= IRQ_FLAGS_HANDLE_TX;
        cc13x2_prop_rf_rx_start();
    }

    if (RFCCpeIntGetAndClear(IRQ_RX_ENTRY_DONE)) {
        DEBUG_PUTS("[isr_rfc_cpe0]: RX OK!");

        assert(_rf_core.state == FSM_STATE_RX);
        _rf_core.irq_handler_flags |= IRQ_FLAGS_HANDLE_RX;
    }

    /* Dispatch IRQ events if there are */
    if (_rf_core.irq_handler_flags != 0) {
        if (_rf_core.irq_handler) {
            _rf_core.irq_handler(_rf_core.irq_handler_arg);
        }
    }

}

void cc13x2_prop_rf_init(void)
{
    /* RF Core is off */
    _rf_core.state = FSM_STATE_OFF;

    /* Max. power by default */
    _rf_core.tx_power = &(output_power_table[0]);

    _rf_core.rat_offset = 0;

    _rf_core.irq_handler = NULL;
    _rf_core.irq_handler_arg = NULL;

    memset(&_rf_stats, 0, sizeof(_rf_stats));
}

int_fast8_t cc13x2_prop_rf_power_on(void)
{
    DEBUG("[cc13x2_prop_rf_power_on]: enable RF Core.\n");

    int_fast8_t error = -1;

    if (_rf_core.state == FSM_STATE_SLEEP) {
        error = 0;
    }
    else if (_rf_core.state == FSM_STATE_OFF) {

        /* Set of RF Core data queue. Circular buffer, no last entry */
        _rx_data_queue.pCurrEntry = _rx_buf0;
        _rx_data_queue.pLastEntry = NULL;

        init_bufs();

        cc26x2_cc13x2_rf_power_on();
        setup_interrupts();

        _rf_core.state = FSM_STATE_SLEEP;

        /* Send a CMD_PING command to verify the RF Core is alive */
        if (cc13x2_dbell_execute(cc13x2_cmd_ping()) != CMDSTA_Done) {
            error = -1;
            goto exit;
        }

        if (cc13x2_prop_rf_send_enable_cmd() != PROP_DONE_OK) {
            error = -1;
            goto exit;
        }

        error = 0;
    }

exit:

    if (error == -1) {
        stop_interrupts();
        cc26x2_cc13x2_rf_power_off();
        _rf_core.state = FSM_STATE_OFF;
    }

    return error;
}

void cc13x2_prop_rf_power_off(void)
{
    if (_rf_core.state == FSM_STATE_OFF) {
        return;
    }

    /* Disable the radio, don't check for errors since the RF Core will be
     * shut down anyway. */
    cc13x2_prop_rf_send_disable_cmd();

    /* Stop RF Core interrupts */
    stop_interrupts();

    /* Power off the RF Core */
    cc26x2_cc13x2_rf_power_off();

    _rf_core.state = FSM_STATE_OFF;
}

int_fast8_t cc13x2_prop_rf_reset(void)
{
    if (_rf_core.state == FSM_STATE_OFF) {
        if (cc13x2_prop_rf_power_on() == -1) {
            return -1;
        }

        if (cc13x2_prop_rf_rx_start() == -1) {
            return -1;
        }
    }
    else if (_rf_core.state == FSM_STATE_SLEEP ||
             _rf_core.state == FSM_STATE_TX) {
        /* If we're in Sleep mode, start receiving, if we're on the Transmit
         * mode trigger the start of the Receive state, this will abort any
         * transmission being done. */
        if (cc13x2_prop_rf_rx_start() == -1) {
            return -1;
        }
    }
    else if (_rf_core.state == FSM_STATE_RX) {
        return 0;
    }

    return 0;
}

int8_t cc13x2_prop_rf_get_txpower(void)
{
    return _rf_core.tx_power->dbm;
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

    _rf_core.tx_power = powercfg;

    uint32_t cmd_set_tx_power =
        cc13x2_cmd_set_tx_power(_rf_core.tx_power->value);

    return cc13x2_dbell_execute(cmd_set_tx_power);
}

int8_t cc13x2_prop_rf_get_rssi(void)
{
    return _rf_stats.lastRssi;
}

int_fast8_t cc13x2_prop_rf_rx_start(void)
{
    /* TODO: this can be made a command chain, command construction needs to
     * be changed for this. It should be more efficient and more clean */
    if (_rf_core.state == FSM_STATE_OFF) {
        return -1;
    }

    if (_rf_core.state == FSM_STATE_SLEEP) {
        /* Initialize the receive command
         * XXX: no memset here because we assume init has been called and we
         *      may have changed some values in the rx command
         */
        if (cc13x2_prop_rf_send_rx_cmd() != CMDSTA_Done) {
            return -1;
        }

        _rf_core.state = FSM_STATE_RX;

        return 0;
    }

    if (_rf_core.state == FSM_STATE_TX) {
        if (cc13x2_cmd_prop_tx_adv_get_status() == PROP_DONE_OK) {
            /* We're in the middle of a TX command, abort it and start RX */
            if (cc13x2_dbell_execute(cc13x2_cmd_abort()) != CMDSTA_Done) {
                return -1;
            }

            /* Be sure that after the TX command there isn't any data on the RX
             * queue (of previous commands, just to be sure) */
            if (clear_rx_queue(&_rx_data_queue) != CMDSTA_Done) {
                return -1;
            }
        }
        else {
            /* We're in the middle of a TX command, abort it and start RX */
            if (cc13x2_dbell_execute(cc13x2_cmd_abort()) != CMDSTA_Done) {
                return -1;
            }

            /* Be sure that after the TX command there isn't any data on the RX
             * queue (of previous commands, just to be sure) */
            if (clear_rx_queue(&_rx_data_queue) != CMDSTA_Done) {
                return -1;
            }

            if (cc13x2_prop_rf_send_rx_cmd() != CMDSTA_Done) {
                return -1;
            }
        }

        return 0;
    }

    /* If we were previously in RX, start the RX again */
    if (_rf_core.state == FSM_STATE_RX &&
        cc13x2_cmd_prop_rx_adv_get_status() != ACTIVE) {
        /* We have either not fallen back into our receive command or we are
         * running on the wrong channel. Either way assume the caller correctly
         * called us and abort all running commands. */
        if (cc13x2_dbell_execute(cc13x2_cmd_abort()) != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_rx_start]: abort command failed!");
            return -1;
        }

        /* Any frames in the queue will be for the old channel */
        if (clear_rx_queue(&_rx_data_queue) != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_rx_start]: couldn't clear RX queue!");
            return -1;
        }

        if (cc13x2_prop_rf_send_rx_cmd() != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_rx_start]: send RX command failed!");
            return -1;
        }

        _rf_core.state = FSM_STATE_RX;
    }

    return 0;
}

int_fast8_t cc13x2_prop_rf_rx_stop(void)
{
    if (_rf_core.state == FSM_STATE_RX) {
        return cc13x2_dbell_execute(cc13x2_cmd_abort());
    }

    return -1;
}

uint8_t cc13x2_prop_rf_get_chan(void)
{
    return _rf_core.channel;
}

void cc13x2_prop_rf_set_chan(uint16_t channel, bool force)
{
    DEBUG_PUTS("[cc13x2_prop_rf_set_chan]: setting channel");
    if (_rf_core.channel == channel && force == false) {
        DEBUG_PUTS("[cc13x2_prop_rf_set_chan]: we are on channel");
        return;
    }

    const uint32_t new_freq = cc13x2_prop_rf_channel_freq(channel);

    uint16_t freq;
    uint16_t frac;
    cc13x2_prop_rf_freq_parts(new_freq, &freq, &frac);

    bool enable_rx = false;

    if (_rf_core.state == FSM_STATE_RX) {
        cc13x2_prop_rf_rx_stop();
        enable_rx = true;
    } else if (_rf_core.state == FSM_STATE_TX) {
        /* Stop the transmission and change freq. */
        if (cc13x2_dbell_execute(cc13x2_cmd_abort()) != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_set_chan]: abort command failed!");
            return;
        }

        /* Switch to Receive after channel change. */
        enable_rx = true;
    }

    bool ints_disabled = IntMasterDisable();

    _rf_core.state = FSM_STATE_SLEEP;

    if (cc13x2_prop_rf_send_fs_cmd(freq, frac) != CMDSTA_Done) {
        DEBUG_PUTS("[cc13x2_prop_rf_set_chan]: couldn't change channel");
        return;
    }

    if (!ints_disabled) {
        IntMasterEnable();
    }

    _rf_core.channel = channel;

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
    _rf_core.irq_handler = handler;
    _rf_core.irq_handler_arg = arg;
}

int cc13x2_prop_rf_recv(void *buf, size_t len,
                        netdev_ieee802154_rx_info_t *rx_info)
{
    DEBUG_PUTS("[cc13x2_prop_rf_recv]: receiving");

    bool available = false;
    rfc_dataEntryGeneral_t *start_entry =
        (rfc_dataEntryGeneral_t *)_rx_data_queue.pCurrEntry;
    rfc_dataEntryGeneral_t *cur_entry = start_entry;

    /* loop through receive queue */
    do {
        if (cur_entry->status == DATA_ENTRY_FINISHED ||
            cur_entry->status == DATA_ENTRY_BUSY) {
            available = true;
            break;
        }

        cur_entry = (rfc_dataEntryGeneral_t *)(cur_entry->pNextEntry);
    } while (cur_entry != start_entry);

    /* No entry in the RX data queue */
    if (!available) {
        return 0;
    }

    /* Entry is being processed by the RF Core, wait for entry to become
     * finished.
     * TODO: timeout
     */
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

    printf("[cc132_prop_rf_recv]: rssi = %d, lqi = %u\n", rssi, lqi);

    if (!buf) {
        /* Without buf return only the length so it can be read later */
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

    cur_entry->status = DATA_ENTRY_PENDING;

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
    if (_rf_core.state == FSM_STATE_RX) {
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
         * The Radio will flip the bits around, so _tx_buf[0] must have the
         * length LSBs (PHR[15:8] and _tx_buf[1] will have PHR[7:0]
         */
        _tx_buf[0] = ((total_length >> 0) & 0xFF);
        _tx_buf[1] = ((total_length >> 8) & 0xFF) + 0x8 + 0x0;

        for (size_t i = 0; i < len; i++) {
            DEBUG("%#02x, ", _tx_buf[i]);
        }
        DEBUG("\n");

        DEBUG_PUTS("[cc13x2_prop_rf_send]: stop RX.");
        if (cc13x2_prop_rf_rx_stop() != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_send]: couldn't stop RX");
            return -EIO;
        }

        DEBUG_PUTS("[cc13x2_prop_rf_send]: switching state to transmit");

        if (cc13x2_prop_rf_send_tx_cmd(_tx_buf, len) != CMDSTA_Done) {
            DEBUG_PUTS("[cc13x2_prop_rf_send]: TX send failed!");
            _rf_core.state = FSM_STATE_SLEEP;
            return -EIO;
        }
        _rf_core.state = FSM_STATE_TX;

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
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) &= ~irq;
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) |= irq;
}

void cc13x2_prop_rf_irq_disable(unsigned irq)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) &= ~irq;
}

cc13x2_prop_rf_irq_flags_t cc13x2_prop_rf_get_flags(void)
{
    cc13x2_prop_rf_irq_flags_t flags = _rf_core.irq_handler_flags;

    /* Clear flags */
    _rf_core.irq_handler_flags = 0;
    return flags;
}

cc13x2_prop_rf_state_t cc13x2_prop_rf_get_state(void)
{
    return _rf_core.state;
}
