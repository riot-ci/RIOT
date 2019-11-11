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
 * @brief   Timings of NRF24L01P devices
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_TIMING_H
#define NRF24L01P_TIMING_H

#include "nrf24l01p_lookup_tables.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Calculate the on-air time in [us] of a frame with a certain length
 *
 * @param dev           NRF24L01P device handle
 * @param payload_width Payload width of the frame
 *
 * @return              frame on-air time in [us]
 */
static inline int nrf24l01p_time_on_air_us(const nrf24l01p_t *dev,
                                           size_t payload_width)
{
    /* packet length [bit] / air data rate [bit/us]*/
    if (dev->params.config.cfg_data_rate == NRF24L01P_RF_DR_250KBPS) {
        return 4 *
               (8 *
                (1 + nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width) +
                 payload_width +
                 nrf24l01p_etoval_crco(dev->params.config.cfg_crc)) + 9);
    }
    else {
        return (8 *
                (1 + nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width) +
                 payload_width +
                 nrf24l01p_etoval_crco(dev->params.config.cfg_crc)) + 9)
               / ((nrf24l01p_etoval_rfdr(dev->params.config.cfg_data_rate) /
                   1000)); /* would be 0 if 250kbps */
    }
}

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_TIMING_H */
/** @} */
