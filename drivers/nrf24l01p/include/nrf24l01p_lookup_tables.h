/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p
 * @{
 *
 * @file
 * @brief   Lookup tables for NRF24L01P device driver
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_LOOKUP_TABLES_H
#define NRF24L01P_LOOKUP_TABLES_H

#include "nrf24l01p_constants.h"
#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Struct that holds certain register addresses for any pipe
 */
typedef struct {
    uint8_t reg_pipe_addr;  /**< Register that holds the rx address */
    uint8_t reg_pipe_plw;   /**< Register that holds the expected payload width */
} nrf24l01p_pipe_regs_t;

/**
 * @brief   Table that maps a channel to a 2.4Ghz offset
 */
extern const uint8_t vchanmap[NRF24L01P_NUM_CHANNELS];

/**
 * @brief   Table that maps data pipe indices to corresponding pipe
 *          register addresses
 */
extern const nrf24l01p_pipe_regs_t reg_pipe_info[NRF24L01P_PX_NUM_OF];

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_LOOKUP_TABLES_H */
/** @} */
