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

#ifndef CC26X2_CC13X2_RFC_MAILBOX_H
#define CC26X2_CC13X2_RFC_MAILBOX_H

#include <driverlib/rf_ble_cmd.h>
#include <driverlib/rf_common_cmd.h>
#include <driverlib/rf_prop_cmd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generic radio operation
 */
typedef rfc_radioOp_t rfc_op_t;

typedef rfc_CMD_RADIO_SETUP_t rfc_cmd_radio_setup_t;
typedef rfc_CMD_BLE5_RADIO_SETUP_t rfc_cmd_ble5_radio_setup_t;
typedef rfc_CMD_PROP_RADIO_SETUP_t rfc_cmd_prop_radio_setup_t;

/**
 * @brief    Propietary radio setup for Sub-GHz
 */
typedef rfc_CMD_PROP_RADIO_DIV_SETUP_t rfc_cmd_prop_radio_div_setup_t;

typedef rfc_CMD_RADIO_SETUP_PA_t rfc_cmd_radio_setup_pa_t;
typedef rfc_CMD_BLE5_RADIO_SETUP_PA_t rfc_cmd_ble5_radio_setup_pa_t;
typedef rfc_CMD_PROP_RADIO_SETUP_PA_t rfc_cmd_prop_radio_setup_pa_t;
typedef rfc_CMD_PROP_RADIO_DIV_SETUP_PA_t rfc_cmd_prop_radio_div_setup_pa_t;

/**
 * @brief   Start the Radio Timer (RAT)
 *
 * The radio timer MUST be enabled before using any receive/transmit command
 * or using commands with a delayed start
 */
typedef rfc_CMD_SYNC_START_RAT_t rfc_cmd_sync_start_rat_t;

/**
 * @brief   Frequency Synthesiszer command
 */
typedef rfc_CMD_FS_t rfc_cmd_fs_t;

#ifdef __cplusplus
}
#endif

#endif /* CC26X2_CC13X2_RFC_MAILBOX_H */

/*@}*/
