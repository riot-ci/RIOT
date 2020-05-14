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
 * @brief     Implementation of ISR functions for the NRF24L01+ (NG)
 *            transceiver
 *
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "nrf24l01p_ng_communication.h"
#include "nrf24l01p_ng_isr.h"

netdev_event_t nrf24l01p_ng_isr_max_rt(const nrf24l01p_ng_t *dev)
{
    assert(dev->state == NRF24L01P_NG_STATE_STANDBY_1 ||
           dev->state == NRF24L01P_NG_STATE_STANDBY_2 ||
           dev->state == NRF24L01P_NG_STATE_RX_MODE   ||
           dev->state == NRF24L01P_NG_STATE_TX_MODE);
    DEBUG_PUTS("[nrf24l01p_ng] IRS MAX_RT\n");
    netdev_event_t event = NETDEV_EVENT_TX_NOACK;
    nrf24l01p_ng_flush_tx(dev);
    return event;
}

netdev_event_t nrf24l01p_ng_isr_rx_dr(const nrf24l01p_ng_t *dev)
{
    assert(dev->state == NRF24L01P_NG_STATE_STANDBY_1 ||
           dev->state == NRF24L01P_NG_STATE_STANDBY_2 ||
           dev->state == NRF24L01P_NG_STATE_RX_MODE   ||
           dev->state == NRF24L01P_NG_STATE_TX_MODE);
    DEBUG_PUTS("[nrf24l01p_ng] IRS RX_DR\n");
    netdev_event_t event = NETDEV_EVENT_RX_COMPLETE;
    return event;
}

netdev_event_t nrf24l01p_ng_isr_tx_ds(const nrf24l01p_ng_t *dev)
{
    assert(dev->state == NRF24L01P_NG_STATE_STANDBY_1 ||
           dev->state == NRF24L01P_NG_STATE_STANDBY_2 ||
           dev->state == NRF24L01P_NG_STATE_RX_MODE   ||
           dev->state == NRF24L01P_NG_STATE_TX_MODE);
    DEBUG_PUTS("[nrf24l01p_ng] IRS TX_DS\n");
    netdev_event_t event = NETDEV_EVENT_TX_COMPLETE;
    return event;
}
