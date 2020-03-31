/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief   Interrupt service routines for NRF24L01+ (NG) device driver
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_NG_ISR_H
#define NRF24L01P_NG_ISR_H

#include "nrf24l01p_ng.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handle MAX_RT (max. retransmissions) IRQ
 *
 * @param[in] dev       NRF24L01+ device handle
 *
 * @return              Netdev event to be passed to netdev callback
 */
netdev_event_t nrf24l01p_ng_isr_max_rt(const nrf24l01p_ng_t *dev);

/**
 * @brief Handle RX_DR (data ready) IRQ
 *
 * @param[in] dev       NRF24L01+ device handle
 *
 * @return              Netdev event to be passed to netdev callback
 */
netdev_event_t nrf24l01p_ng_isr_rx_dr(const nrf24l01p_ng_t *dev);

/**
 * @brief Handle TX_DS (data sent) IRQ
 *
 * @param[in] dev       NRF24L01+ device handle
 *
 * @return              Netdev event to be passed to netdev callback
 */
netdev_event_t nrf24l01p_ng_isr_tx_ds(const nrf24l01p_ng_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_NG_ISR_H */
/** @} */
