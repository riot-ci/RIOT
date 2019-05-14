/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_lorawan
 * @{
 *
 * @file
 * @brief   GNRC LoRaWAN region specific functions
 *
 * @author  José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef NET_GNRC_LORAWAN_REGION_H
#define NET_GNRC_LORAWAN_REGION_H

#include "net/gnrc/lorawan/lorawan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GNRC_LORAWAN_LC_1 (868100000)   /**< First default EU868 channel */
#define GNRC_LORAWAN_LC_2 (868300000)   /**< Second default EU868 channel */
#define GNRC_LORAWAN_LC_3 (868500000)   /**< Third default EU868 channel */

#define GNRC_LORAWAN_DEFAULT_CHANNELS 3 /**< Number of default channels for the current region */

/**
 * @brief Process Channel Frequency list frame
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] cflist the CFList to be processed
 */
void gnrc_lorawan_process_cflist(gnrc_lorawan_t *mac, uint8_t *cflist);

/**
 * @brief Get the datarate of the first reception windows
 *
 * @param[in] dr_up the datarate of the transmission
 * @param[in] dr_offset the offset of the first reception window
 *
 * @return datarate
 */
uint8_t gnrc_lorawan_rx1_get_dr_offset(uint8_t dr_up, uint8_t dr_offset);

/**
 * @brief Check if a datarate is valid in the current region
 *
 * @param[in] the datarate to be checked
 *
 * @return true if datarate is valid
 * @return false otherwise
 */
int gnrc_lorawan_validate_dr(uint8_t dr);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_LORAWAN_REGION_H */
