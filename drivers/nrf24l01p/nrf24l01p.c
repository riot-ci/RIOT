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
 * @brief   Implementation of the public NRF24L01P device interface
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <errno.h>
#include <string.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "nrf24l01p_constants.h"
#include "nrf24l01p_lookup_tables.h"
#include "nrf24l01p_communication.h"
#include "nrf24l01p_registers.h"
#include "nrf24l01p_states.h"
#include "nrf24l01p_netdev.h"
#include "nrf24l01p_internal.h"

static int nrf24l01p_set_payload_width(nrf24l01p_t *dev, uint8_t width,
                                       nrf24l01p_pipe_t pipe)
{
    assert(dev);
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_ESB) {
        return 0;
    }
    if (pipe >= NRF24L01P_PX_NUM_OF) {
        return -ERANGE;
    }
    if (!width || width > NRF24L01P_MAX_PAYLOAD_WIDTH) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8__RX_PW_Px_t rx_pw_px = NRF24L01P_FLG__RX_PW_Px(width);
    nrf24l01p_acquire(dev);
    nrf24l01p_write_reg(dev, reg_pipe_info[pipe].reg_pipe_plw, &rx_pw_px, 1);
    nrf24l01p_release(dev);
    if (pipe == NRF24L01P_P0) {
        dev->params.config.cfg_plw_padd_p0 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    else if (pipe == NRF24L01P_P1) {
        dev->params.config.cfg_plw_padd_p1 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    else if (pipe == NRF24L01P_P2) {
        dev->params.config.cfg_plw_padd_p2 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    else if (pipe == NRF24L01P_P3) {
        dev->params.config.cfg_plw_padd_p3 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    else if (pipe == NRF24L01P_P4) {
        dev->params.config.cfg_plw_padd_p4 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    else if (pipe == NRF24L01P_P5) {
        dev->params.config.cfg_plw_padd_p5 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    return 0;
}

static int nrf24l01p_get_payload_width(nrf24l01p_t *dev, nrf24l01p_pipe_t pipe)
{
    assert(dev);
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_ESB) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH;
    }
    else if (pipe == NRF24L01P_P0) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p0;
    }
    else if (pipe == NRF24L01P_P1) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p1;
    }
    else if (pipe == NRF24L01P_P2) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p2;
    }
    else if (pipe == NRF24L01P_P3) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p3;
    }
    else if (pipe == NRF24L01P_P4) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p4;
    }
    else if (pipe == NRF24L01P_P5) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p5;
    }
    return -ERANGE;
}

int nrf24l01p_setup(nrf24l01p_t *dev, const nrf24l01p_params_t *params)
{
    assert(dev);
    assert(params);
    if (params->config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        if (params->config.cfg_max_retr != 0) {
            return -ENOTSUP;
        }
    }
    /* Zero out everything but RIOT's driver interface, which should be
     * managed by RIOT
     */
    memset((char *)dev + sizeof(netdev_t), 0x00,
           sizeof(nrf24l01p_t) - sizeof(netdev_t));
    dev->state = NRF24L01P_STATE_UNDEFINED;
#ifndef NDEBUG
    dev->transitions = NRF24L01P_TRANSITIONS_FROM_UNDEFINED;
#endif
    dev->params = *params;
    dev->netdev.driver = &nrf24l01p_driver;
    nrf24l01p_power_on(dev);
    return 0;
}

int nrf24l01p_set_air_data_rate(nrf24l01p_t *dev, nrf24l01p_rfdr_t data_rate)
{
    assert(dev);
    if (data_rate >= NRF24L01P_RF_DR_NUM_OF) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8__RF_SETUP_t rf_setup = NRF24L01P_FLG__RF_DR(data_rate);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG__RF_SETUP, NRF24L01P_MSK__RF_DR,
                       &rf_setup);
    nrf24l01p_release(dev);
    dev->params.config.cfg_data_rate = data_rate;
    return 0;
}

uint16_t nrf24l01p_get_air_data_rate(nrf24l01p_t *dev,
                                     nrf24l01p_rfdr_t *data_rate)
{
    assert(dev);
    if (data_rate) {
        *data_rate = dev->params.config.cfg_data_rate;
    }
    return nrf24l01p_etoval_rfdr(dev->params.config.cfg_data_rate);
}

int nrf24l01p_set_crc(nrf24l01p_t *dev, nrf24l01p_crco_t crc)
{
    assert(dev);
    if (crc >= NRF24L01P_CRCO_NUM_OF) {
        return -EINVAL;
    }
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_ESB) {
        if (crc == NRF24L01P_CRCO_0) {
            return -ENOTSUP;
        }
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8__CONFIG_t config =
        (((crc & 2) ? NRF24L01P_FLG__EN_CRC : 0) |
         ((crc & 1) ? NRF24L01P_FLG__CRCO_2_BYTE : NRF24L01P_FLG__CRCO_1_BYTE));
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG__CONFIG, NRF24L01P_MSK__CRC,
                       &config);
    nrf24l01p_release(dev);
    dev->params.config.cfg_crc = crc;
    return 0;
}

uint8_t nrf24l01p_get_crc(nrf24l01p_t *dev, nrf24l01p_crco_t *crc)
{
    assert(dev);
    if (crc) {
        *crc = dev->params.config.cfg_crc;
    }
    return nrf24l01p_etoval_crco(dev->params.config.cfg_crc);
}

int nrf24l01p_set_tx_power(nrf24l01p_t *dev, nrf24l01p_rfpwr_t power)
{
    assert(dev);
    if (power >= NRF24L01P_RF_PWR_NUM_OF) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8__RF_SETUP_t rf_setup = NRF24L01P_FLG__RF_PWR(power);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG__RF_SETUP, NRF24L01P_MSK__RF_PWR,
                       &rf_setup);
    nrf24l01p_release(dev);
    dev->params.config.cfg_tx_power = power;
    return 0;
}

int8_t nrf24l01p_get_tx_power(nrf24l01p_t *dev, nrf24l01p_rfpwr_t *power)
{
    assert(dev);
    if (power) {
        *power = dev->params.config.cfg_tx_power;
    }
    return nrf24l01p_etoval_rfpwr(dev->params.config.cfg_tx_power);
}

int nrf24l01p_set_channel(nrf24l01p_t *dev, uint8_t channel)
{
    assert(dev);
    if (channel >= NRF24L01P_NUM_CHANNELS) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8__RF_CH_t rf_ch = NRF24L01P_FLG__RF_CH(vchanmap[channel]);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG__RF_CH, NRF24L01P_MSK__RF_CH,
                       &rf_ch);
    nrf24l01p_release(dev);
    dev->params.config.cfg_channel = channel;
    return 0;
}

uint8_t nrf24l01p_get_channel(nrf24l01p_t *dev)
{
    assert(dev);
    return dev->params.config.cfg_channel;
}

int nrf24l01p_set_mtu(nrf24l01p_t *dev, uint8_t mtu, nrf24l01p_pipe_t pipe)
{
#ifdef NRF24L01P_CUSTOM_HEADER
    return nrf24l01p_set_payload_width(dev,
                                       mtu + NRF24L01P_MAX_ADDR_WIDTH + 1,
                                       pipe);
#else
    return nrf24l01p_set_payload_width(dev, mtu, pipe);
#endif
}

int nrf24l01p_get_mtu(nrf24l01p_t *dev, nrf24l01p_pipe_t pipe)
{
#ifdef NRF24L01P_CUSTOM_HEADER
    return nrf24l01p_get_payload_width(dev,
                                       pipe) - (NRF24L01P_MAX_ADDR_WIDTH + 1);
#else
    return nrf24l01p_get_payload_width(dev, pipe);
#endif
}

int nrf24l01p_set_rx_address(nrf24l01p_t *dev, const uint8_t *addr,
                             size_t addr_len, nrf24l01p_pipe_t pipe)
{
    assert(dev);
    assert(addr);
    if (pipe >= NRF24L01P_PX_NUM_OF) {
        return -ERANGE;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    if (pipe == NRF24L01P_P0 || pipe == NRF24L01P_P1) {
        if (addr_len !=
            nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width)) {
            return -EINVAL;
        }
        nrf24l01p_acquire(dev);
        nrf24l01p_write_reg(dev, reg_pipe_info[pipe].reg_pipe_addr, addr,
                            addr_len);
        nrf24l01p_release(dev);
        memcpy(dev->params.urxaddr.arxaddr.rx_addr_long[pipe], addr, addr_len);
    }
    else {
        if (addr_len > 1) {
            return -EINVAL;
        }
        nrf24l01p_acquire(dev);
        nrf24l01p_write_reg(dev, reg_pipe_info[pipe].reg_pipe_addr, addr,
                            addr_len);
        nrf24l01p_release(dev);
        dev->params.urxaddr.arxaddr.rx_addr_short[pipe - 2] = *addr;
    }
    return 0;
}

int nrf24l01p_get_rx_address(nrf24l01p_t *dev, uint8_t *addr,
                             nrf24l01p_pipe_t pipe)
{
    assert(dev);
    assert(addr);
    if (pipe >= NRF24L01P_PX_NUM_OF) {
        return -ERANGE;
    }
    uint8_t aw = nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width);
    if (pipe == NRF24L01P_P0 || pipe == NRF24L01P_P1) {
        memcpy(addr, dev->params.urxaddr.arxaddr.rx_addr_long[pipe], aw);
    }
    else {
        memcpy(addr, dev->params.urxaddr.arxaddr.rx_addr_long[NRF24L01P_P1], aw);
        addr[aw - 1] = dev->params.urxaddr.arxaddr.rx_addr_short[pipe - 2];
    }
    return aw;
}

int nrf24l01p_set_ack_payload(nrf24l01p_t *dev, const void *payload,
                              size_t payload_width, nrf24l01p_pipe_t pipe)
{
    assert(dev);
    assert(payload);
    if (payload_width > NRF24L01P_MTU) {
        return -EINVAL;
    }
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        return -ENOTSUP;
    }
    if (pipe >= NRF24L01P_PX_NUM_OF) {
        return -ERANGE;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
#ifdef NRF24L01P_CUSTOM_HEADER
    uint8_t aw = nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width);
    uint8_t pl[1 + aw + payload_width];
    sb_hdr_init((shockburst_hdr_t *)pl);
    sb_hdr_set_src_addr_width((shockburst_hdr_t *)pl, aw);
    if (pipe == NRF24L01P_P0) {
        memcpy(pl + 1, dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr, aw);
    }
    else {
        memcpy(pl + 1, dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr, aw);
        if (pipe > NRF24L01P_P1) {
            pl[aw] = dev->params.urxaddr.arxaddr.rx_addr_short[pipe - 2];
        }
    }
    memcpy(pl + 1 + aw, payload, payload_width);
    payload = pl;
    payload_width = sizeof(pl);
#endif
    nrf24l01p_acquire(dev);
    nrf24l01p_write_ack_pl(dev, payload, payload_width, pipe);
    nrf24l01p_release(dev);
    return 0;
}

int nrf24l01p_set_max_retransm(nrf24l01p_t *dev, uint8_t max_rt)
{
    assert(dev);
    if (max_rt > NRF24L01P_MAX_RETRANSMISSIONS) {
        return -EINVAL;
    }
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        return -ENOTSUP;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8__SETUP_RETR_t setup_retr = NRF24L01P_FLG__ARC(max_rt);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG__SETUP_RETR, NRF24L01P_MSK__ARC,
                       &setup_retr);
    nrf24l01p_release(dev);
    dev->params.config.cfg_max_retr = max_rt;
    return 0;
}

uint8_t nrf24l01p_get_max_retransm(nrf24l01p_t *dev)
{
    assert(dev);
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        return 0;
    }
    return dev->params.config.cfg_max_retr;
}

int nrf24l01p_set_retransm_delay(nrf24l01p_t *dev, nrf24l01p_ard_t rt_delay)
{
    assert(dev);
    if (rt_delay >= NRF24L01P_ARD_NUM_OF) {
        return -EINVAL;
    }
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        return -ENOTSUP;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8__SETUP_RETR_t setup_retr = NRF24L01P_FLG__ARD(rt_delay);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG__SETUP_RETR, NRF24L01P_MSK__ARD,
                       &setup_retr);
    nrf24l01p_release(dev);
    dev->params.config.cfg_retr_delay = rt_delay;
    return 0;
}

uint16_t nrf24l01p_get_retransm_delay(nrf24l01p_t *dev,
                                      nrf24l01p_ard_t *rt_delay)
{
    assert(dev);
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        return 0;
    }
    if (rt_delay) {
        *rt_delay = dev->params.config.cfg_retr_delay;
    }
    return nrf24l01p_etoval_ard(dev->params.config.cfg_retr_delay);
}

int nrf24l01p_set_state(nrf24l01p_t *dev, nrf24l01p_state_t state)
{
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_state_t old = dev->state;
    nrf24l01p_acquire(dev);
    if (state == NRF24L01P_STATE_POWER_DOWN) {
        if (dev->state != NRF24L01P_STATE_POWER_DOWN) {
            nrf24l01p_transition_to_POWER_DOWN(dev);
        }
    }
    else if (state == NRF24L01P_STATE_STANDBY_1) {
        if (dev->state != NRF24L01P_STATE_STANDBY_1) {
            nrf24l01p_transition_to_STANDBY_1(dev);
        }
    }
    else if (state == NRF24L01P_STATE_RX_MODE) {
        if (dev->state != NRF24L01P_STATE_RX_MODE) {
            if (state != NRF24L01P_STATE_STANDBY_1) {
                nrf24l01p_transition_to_STANDBY_1(dev);
            }
            nrf24l01p_transition_to_RX_MODE(dev);
        }
    }
    else {
        nrf24l01p_release(dev);
        return -ENOTSUP;
    }
    nrf24l01p_release(dev);
    return (int)old;
}

nrf24l01p_state_t nrf24l01p_get_state(nrf24l01p_t *dev)
{
    assert(dev);
    return dev->state;
}
