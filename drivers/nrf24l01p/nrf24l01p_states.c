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
 * @brief   Implementation of state transition procedures
 *          for the NRF24L01+ transceiver
 *
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <assert.h>
#include <string.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "xtimer.h"
#include "net/netopt.h"

#include "nrf24l01p_registers.h"
#include "nrf24l01p_communication.h"
#include "nrf24l01p_states.h"

static void _restore_address(const nrf24l01p_t *dev)
{
    nrf24l01p_reg8_setup_aw_t setup_aw =
        NRF24L01P_FLG_AW(nrf24l01p_valtoe_aw(NRF24L01P_ADDR_WIDTH));
    uint8_t addr_buffer[NRF24L01P_ADDR_WIDTH];

    memcpy(addr_buffer, dev->params.urxaddr.rxaddrpx.rx_p0,
           NRF24L01P_ADDR_WIDTH);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_ADDR_P0, addr_buffer,
                        NRF24L01P_ADDR_WIDTH);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_SETUP_AW, &setup_aw, 1);
}

void nrf24l01p_transition_to_power_down(nrf24l01p_t *dev)
{
    DEBUG("[nrf24l01p] transition to POWER_DOWN\n");
    assert(dev->state & NRF24L01P_TRANSITION_TO_POWER_DOWN);
    nrf24l01p_reg8_config_t config = NRF24L01P_FLG_PWR_UP;
    nrf24l01p_reg8_clear(dev, NRF24L01P_REG_CONFIG, &config);
    dev->state = NRF24L01P_STATE_POWER_DOWN;
}

void nrf24l01p_transition_to_standby_1(nrf24l01p_t *dev)
{
    DEBUG("[nrf24l01p] transition to STANDBY_1\n");
    assert(dev->state & NRF24L01P_TRANSITION_TO_STANDBY_1);
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
            gpio_clear(dev->params.pin_ce);
            nrf24l01p_reg8_config_t config = NRF24L01P_FLG_PWR_UP;
            nrf24l01p_reg8_set(dev, NRF24L01P_REG_CONFIG, &config);
            xtimer_usleep(NRF24L01P_DELAY_US_START_UP);
            break;
        case NRF24L01P_STATE_RX_MODE:
            gpio_clear(dev->params.pin_ce);
            break;
        case NRF24L01P_STATE_STANDBY_2:
        case NRF24L01P_STATE_TX_MODE:
            _restore_address(dev);
            /* TX finished with one packet */
            break;
        default:
            return;
    }
    dev->state = NRF24L01P_STATE_STANDBY_1;
}

void nrf24l01p_transition_to_standby_2(nrf24l01p_t *dev)
{
    DEBUG("[nrf24l01p] transition to STANDBY_2\n");
    assert(dev->state & NRF24L01P_TRANSITION_TO_STANDBY_2);
    switch (dev->state) {
        case NRF24L01P_STATE_STANDBY_1:;
            /* TX FIFO empty */
            nrf24l01p_reg8_config_t config = NRF24L01P_FLG_PRIM_RX;
            nrf24l01p_reg8_clear(dev, NRF24L01P_REG_CONFIG, &config);
            gpio_set(dev->params.pin_ce);
            break;
        case NRF24L01P_STATE_TX_MODE:
            gpio_set(dev->params.pin_ce);
            break;
        default:
            return;
    }
    dev->state = NRF24L01P_STATE_STANDBY_2;
}

void nrf24l01p_transition_to_rx_mode(nrf24l01p_t *dev)
{
    DEBUG("[nrf24l01p] transition to RX_MODE\n");
    assert(dev->state & NRF24L01P_TRANSITION_TO_RX_MODE);
    nrf24l01p_reg8_fifo_status_t fifo_status;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_FIFO_STATUS, &fifo_status, 1);
    if (fifo_status & NRF24L01P_FLG_RX_FULL) {
        nrf24l01p_flush_rx(dev);
    }
    nrf24l01p_reg8_config_t config = NRF24L01P_FLG_PRIM_RX;
    nrf24l01p_reg8_set(dev, NRF24L01P_REG_CONFIG, &config);
    gpio_set(dev->params.pin_ce);
    xtimer_usleep(NRF24L01P_DELAY_US_RX_SETTLING);
    dev->state = NRF24L01P_STATE_RX_MODE;
}

void nrf24l01p_transition_to_tx_mode(nrf24l01p_t *dev)
{
    DEBUG("[nrf24l01p] transition to TX_MODE\n");
    assert(dev->state & NRF24L01P_TRANSITION_TO_TX_MODE);
    /* TX FIFI not empty */
    nrf24l01p_reg8_config_t config = NRF24L01P_FLG_PRIM_RX;
    nrf24l01p_reg8_clear(dev, NRF24L01P_REG_CONFIG, &config);
    dev->state = NRF24L01P_STATE_TX_MODE;
}
