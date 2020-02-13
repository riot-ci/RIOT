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

#ifndef CC13X2_PROP_RF_COMMANDS_H
#define CC13X2_PROP_RF_COMMANDS_H

#include <driverlib/rf_mailbox.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Construct a CMD_SYNC_START_RAT command.
 *
 * @note    @p next_cmd will only be runned if this command succeeds
 *          (COND_STOP_ON_FALSE).
 *
 * @param[in] next_cmd The next command to run, if none can be 0.
 * @param[in] rat0     The Radio Timer offset.
 *
 * @return  uint32_t The command address.
 */
uint32_t cc13x2_cmd_sync_start_rat(uint32_t next_cmd, uint32_t rat0);

/**
 * @brief   Construct a CMD_PROP_RADIO_DIV_SETUP command
 *
 * @param[in] tx_power     The transmit power.
 * @param[in] reg_override The register override array.
 *
 * @return  uint32_t The command address.
 */
uint32_t cc13x2_cmd_prop_radio_div_setup(uint16_t tx_power, uint32_t *reg_override);

/**
 * @brief   Construct a CMD_FS_POWERDOWN command
 *
 * @note    If @p next_cmd is present (not 0) the next command will be runned
 *          always even if this comand fails (COND_ALWAYS).
 *
 * @param[in] next_cmd Next command to run or 0 if not command needs to be
 *                     runned.
 *
 * @return  uint32_t The command address.
 */
uint32_t cc13x2_cmd_fs_powerdown(uint32_t next_cmd);

/**
 * @brief   Construct a CMD_SYNC_STOP_RAT command
 *
 * @return  uint32_t The command address.
 */
uint32_t cc13x2_cmd_sync_stop_rat(void);

/**
 * @brief   Get the Radio Timer offset from the CMD_SYNC_STOP_RAT command
 *
 * @note    Keep in mind that this value is updated _only_ when you execute a
 *          CMD_SYNC_STOP_RAT command.
 *
 * @return  uint32_t Timer offset.
 */
uint32_t cc13x2_cmd_sync_stop_rat_get_rat0(void);

/**
 * @brief   Construct a CMD_CLEAR_RX command
 *
 * @param[in] queue A pointer to a data queue.
 *
 * @return uint32_t The command address.
 */
uint32_t cc13x2_cmd_clear_rx(dataQueue_t *queue);

/**
 * @brief   Construct a CMD_FS command
 *
 * @note    If @p next_cmd (if isn't 0) will not run if this command fails
 *          (COND_STOP_ON_FALSE).
 *
 * @param[in] next_cmd Next command to run.
 * @param[in] freq     The frequency in MHz.
 * @param[in] frac     The fractional part of the frequency.
 *
 * @return  uint32_t The command address.
 */
uint32_t cc13x2_cmd_fs(uint32_t next_cmd, uint16_t freq, uint16_t frac);

/**
 * @brief   Construct a CMD_PROP_RX_ADV command
 *
 * @pre     @p queue != NULL
 *
 * @param[in] queue  The RX data queue.
 * @param[in] output The RX statistics.
 *
 * @return  uint32_t The command address.
 */
uint32_t cc13x2_cmd_prop_rx_adv(dataQueue_t *queue, void *output);

/**
 * @brief   Construct a CMD_PROP_TX_ADV command
 *
 * @pre     (@p != NULL) && (@p packet_len != 0)
 *
 * @param[in] packet     The packet data.
 * @param[in] packet_len The packet data length in bytes.
 *
 * @return  uint32_t The command address.
 */
uint32_t cc13x2_cmd_prop_tx_adv(void *packet, uint16_t packet_len);

/**
 * @brief   Get the CMD_PROP_RADIO_DIV_SETUP command status.
 *
 * @return  uint16_t The status.
 */
uint16_t cc13x2_cmd_prop_radio_div_setup_status(void);

/**
 * @brief   Get the CMD_SYNC_STOP_RAT command status.
 *
 * @return  uint16_t The status.
 */
uint16_t cc13x2_cmd_sync_stop_rat_get_status(void);

/**
 * @brief   Get the CMD_PROP_RX_ADV command status.
 *
 * @return  uint16_t The status.
 */
uint16_t cc13x2_cmd_prop_rx_adv_get_status(void);

/**
 * @brief   Get the CMD_PROP_TX_ADV command status.
 *
 * @return  uint16_t The status.
 */
uint16_t cc13x2_cmd_prop_tx_adv_get_status(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC13X2_PROP_RF_COMMANDS_H */

/*@}*/
