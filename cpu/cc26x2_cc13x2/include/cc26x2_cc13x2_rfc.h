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
 * @brief           CC26x2/CC13x2 RF Core common functions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26X2_CC13X2_RFC_H
#define CC26X2_CC13X2_RFC_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "cc26x2_cc13x2_rfc_mailbox.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A unified type for radio setup commands of different PHYs.
 *
 * Radio setup commands are used to initialize a PHY on the RF core. Various
 * partially similar commands exist, each one represented by a different data
 * type. rfc_radio_setup is a generic container for all types.
 */
typedef union {
    uint16_t command_id; /**< Generic command identifier. This is the first
                              field in every radio operation command. */
    rfc_cmd_radio_setup_t common; /**< Radio setup command for BLE and IEEE
                                       modes */
    rfc_cmd_ble5_radio_setup_t ble5; /**< Radio setup command for BLE5 mode */
    rfc_cmd_prop_radio_setup_t prop; /**< Radio setup command for PROPRIETARY
                                          mode on 2.4 GHz */
    rfc_cmd_prop_radio_div_setup_t prop_div; /**< Radio setup command for
                                                  PROPRIETARY mode on
                                                  Sub-1 Ghz */
    rfc_cmd_radio_setup_pa_t common_pa; /**< Radio setup command for BLE and
                                             IEEE modes with High Gain PA */
    rfc_cmd_ble5_radio_setup_pa_t ble5_pa; /**< Radio setup command for BLE5
                                                mode with High Gain PA */
    rfc_cmd_prop_radio_setup_pa_t prop_pa; /**< Radio setup command for
                                                PROPRIETARY mode on 2.4 GHz
                                                with High Gain PA */
    rfc_cmd_prop_radio_div_setup_pa_t  prop_div_pa; /**< Radio setup command
                                                         for PROPRIETARY mode
                                                         on Sub-1 Ghz with
                                                         High Gain PA */
} rfc_radio_setup_t;

/**
 * @brief   Initialize RF driver
 *
 * @param[in] radio_setup The radio setup command.
 */
void rfc_init(rfc_radio_setup_t *radio_setup);

/**
 * @brief   Enable radio.
 *
 * @return 0 on success.
 * @return -1 on failure.
 */
int rfc_enable(void);

/**
 * @brief   Enable the RF core clocks.
 *
 * As soon as the RF core is started it will handle clock control autonomously.
 * No check should be performed to check the clocks. Instead the radio can be
 * ping'ed through the command interface.
 */
void rfc_clock_enable(void);

/**
 * @brief   Execute a command and wait for the RFC_DBELL:CMDR register to ACK
 *          the command.
 *
 * @return Command status.
 */
uint8_t rfc_execute_sync(uint32_t cmd);

/**
 * @brief   Get CPE interrupt flag.
 *
 * @param[in] flag Interrupt flag.
 *
 * @return The interrupt.
 */
uint32_t rfc_cpe_int_get(uint32_t flag);

/**
 * @brief   Clear CPE interrupt flag.
 *
 * @param[in] flag The flag to clear.
 */
void rfc_cpe_int_clear(uint32_t flag);

/**
 * @brief   Clear all enabled CPE interrupts.
 */
void rfc_cpe_int_enable_clear(void);

/**
 * @brief   Enable CPE interrupt.
 *
 * @param[in] irq The interrupt to enable.
 */
void rfc_cpe_int_enable(uint32_t irq);

/**
 * @brief   Disable CPE interrupt.
 *
 * @param[in] irq The interrupt to disable.
 */
void rfc_cpe_int_disable(uint32_t irq);

/**
 * @brief   Select interrupts for RF_CPE0_IRQN
 *
 * @param[in] irq Interrupt to select.
 */
void rfc_cpe0_int_select(uint32_t irq);

/**
 * @brief   Select interrupts for RF_CPE1_IRQN
 *
 * @param[in] irq Interrupt to select.
 */
void rfc_cpe1_int_select(uint32_t irq);

/**
 * @brief   Enable AON_RTC RTC_UPD
 *
 * @param[in] v On/off.
 */
void aon_rtc_ctl_rtc_upd(bool v);

#ifdef __cplusplus
}
#endif

#endif /* CC26X2_CC13X2_RFC_H */

/*@}*/
