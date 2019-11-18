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
 * @brief     Lookup tables for the NRF24L01P transceiver
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include "nrf24l01p_registers.h"
#include "nrf24l01p_lookup_tables.h"

const uint8_t vchanmap[NRF24L01P_NUM_CHANNELS] = {
    0,      /* 2400 Mhz +   0 Mhz */
    8,      /* 2400 Mhz +   8 Mhz */
    16,     /* 2400 Mhz +  16 Mhz */
    24,     /* 2400 Mhz +  24 Mhz */
    32,     /* 2400 Mhz +  32 Mhz */
    40,     /* 2400 Mhz +  40 Mhz */
    48,     /* 2400 Mhz +  48 Mhz */
    56,     /* 2400 Mhz +  56 Mhz */
    64,     /* 2400 Mhz +  64 Mhz */
    72,     /* 2400 Mhz +  72 Mhz */
    80,     /* 2400 Mhz +  80 Mhz */
    88,     /* 2400 Mhz +  88 Mhz */
    96,     /* 2400 Mhz +  96 Mhz */
    104,    /* 2400 Mhz + 104 Mhz */
    112,    /* 2400 Mhz + 112 Mhz */
    120     /* 2400 Mhz + 120 Mhz */
};

const nrf24l01p_pipe_regs_t reg_pipe_info[NRF24L01P_PX_NUM_OF] = {
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P0,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P0
    },
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P1,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P1
    },
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P2,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P2
    },
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P3,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P3
    },
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P4,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P4
    },
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P5,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P5
    }
};
