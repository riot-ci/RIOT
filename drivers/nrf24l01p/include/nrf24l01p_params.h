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
 * @brief   Board specific configuration for all connected NRF24L01+ devices
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_PARAMS_H
#define NRF24L01P_PARAMS_H

#include "board.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "kernel_defines.h"
#include "nrf24l01p_constants.h"
#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NRF24L01P_PARAM_SPI
/**
 * @brief   SPI bus
 */
#define NRF24L01P_PARAM_SPI                 SPI_DEV(0)
#endif

#ifndef NRF24L01P_PARAM_SPI_CLK
/**
 * @brief   SPI clock speed
 */
#define NRF24L01P_PARAM_SPI_CLK             SPI_CLK_5MHZ
#endif

#ifndef NRF24L01P_PARAM_CS
/**
 * @brief   SPI CS gpio pin
 */
#define NRF24L01P_PARAM_CS                  GPIO_UNDEF
#endif

#ifndef NRF24L01P_PARAM_CE
/**
 * @brief   NRF24L01+ chip enable gpio pin
 */
#define NRF24L01P_PARAM_CE                  GPIO_UNDEF
#endif

#ifndef NRF24L01P_PARAM_IRQ
/**
 * @brief   NRF24L01+ interrupt gpio pin
 */
#define NRF24L01P_PARAM_IRQ                 GPIO_UNDEF
#endif

#ifndef NRF24L01P_PARAM_rx_p0
/**
 * @brief   Pipe 0 address (enabled by default)
 *          Make sure that you pass an address that
 *          is NRF24L01P_ADDR_WIDTH wide.
 */
#define NRF24L01P_PARAM_rx_p0          NRF24L01P_DEFAULT_L2ADDR_P0
#endif

#ifndef NRF24L01P_PARAM_rx_p1
/**
 * @brief   Pipe 1 address (enabled by default)
 *          Make sure that you pass an address that
 *          is NRF24L01P_ADDR_WIDTH wide.
 */
#define NRF24L01P_PARAM_rx_p1          NRF24L01P_DEFAULT_L2ADDR_P1
#endif

#ifndef NRF24L01P_PARAM_rx_p2
/**
 * @brief   Pipe 2 address (disabled by default)
 */
#define NRF24L01P_PARAM_rx_p2          NRF24L01P_L2ADDR_UNDEF
#endif

#ifndef NRF24L01P_PARAM_rx_p3
/**
 * @brief   Pipe 3 address (disabled by default)
 */
#define NRF24L01P_PARAM_rx_p3          NRF24L01P_L2ADDR_UNDEF
#endif

#ifndef NRF24L01P_PARAM_rx_p4
/**
 * @brief   Pipe 4 address (disabled by default)
 */
#define NRF24L01P_PARAM_rx_p4          NRF24L01P_L2ADDR_UNDEF
#endif

#ifndef NRF24L01P_PARAM_rx_p5
/**
 * @brief   Pipe 5 address (disabled by default)
 */
#define NRF24L01P_PARAM_rx_p5          NRF24L01P_L2ADDR_UNDEF
#endif

#ifndef NRF24L01P_PARAM_CRC_LEN
/**
 * @brief   Default CRC length [1; 3]
 *          @see nrf24l01p_crc_t
 */
#define NRF24L01P_PARAM_CRC_LEN             (NRF24L01P_CRC_2BYTE)
#endif

#ifndef NRF24L01P_PARAM_TX_POWER_LVL
/**
 * @brief   Default TX power in [0; 3]
 *          @see nrf24l01p_tx_power_t
 */
#define NRF24L01P_PARAM_TX_POWER_LVL        (NRF24L01P_TX_POWER_0DBM)
#endif

#ifndef NRF24L01P_PARAM_DATA_RATE_LVL
/**
 * @brief   Default data rate [0; 2]
 *          @see nrf24l01p_rfdr_t
 */
#define NRF24L01P_PARAM_DATA_RATE_LVL       (NRF24L01P_RF_DR_2MBPS)
#endif

#ifndef NRF24L01P_PARAM_CHANNEL
/**
 * @brief   Default channel in [0; 124]
 */
#define NRF24L01P_PARAM_CHANNEL             (4)
#endif

#ifndef NRF24L01P_PARAM_MAX_RETRANSM
/**
 * @brief   Default number of retransmissions
 */
#define NRF24L01P_PARAM_MAX_RETRANSM        (5)
#endif

#ifndef NRF24L01P_PARAM_RETRANSM_DELAY
/**
 * @brief   Default retransmission delay
 */
#define NRF24L01P_PARAM_RETRANSM_DELAY      (10)
#endif

#ifndef NRF24L01P_PARAMS
/**
 * @brief Default NRF24L01+ device parameters
 */
#define NRF24L01P_PARAMS    {                                                  \
        .spi = NRF24L01P_PARAM_SPI,                                            \
        .spi_clk = NRF24L01P_PARAM_SPI_CLK,                                    \
        .pin_cs = NRF24L01P_PARAM_CS,                                          \
        .pin_ce = NRF24L01P_PARAM_CE,                                          \
        .pin_irq = NRF24L01P_PARAM_IRQ,                                        \
        .urxaddr.rxaddrpx = {                                                  \
            .rx_p0 = NRF24L01P_PARAM_rx_p0,                                    \
            .rx_p1 = NRF24L01P_PARAM_rx_p1,                                    \
            .rx_p2 = NRF24L01P_PARAM_rx_p2,                                    \
            .rx_p3 = NRF24L01P_PARAM_rx_p3,                                    \
            .rx_p4 = NRF24L01P_PARAM_rx_p4,                                    \
            .rx_p5 = NRF24L01P_PARAM_rx_p5,                                    \
        },                                                                     \
        .config =       {                                                      \
            .cfg_crc = NRF24L01P_PARAM_CRC_LEN,                                \
            .cfg_tx_power = NRF24L01P_PARAM_TX_POWER_LVL,                      \
            .cfg_data_rate = NRF24L01P_PARAM_DATA_RATE_LVL,                    \
            .cfg_channel = NRF24L01P_PARAM_CHANNEL,                            \
            .cfg_max_retr = NRF24L01P_PARAM_MAX_RETRANSM,                      \
            .cfg_retr_delay = NRF24L01P_PARAM_RETRANSM_DELAY,                  \
        }                                                                      \
}
#endif

/**
 * @brief   Static array that holds NRF24L01+ device configurations
 */
static const nrf24l01p_params_t nrf24l01p_params[] = {
    NRF24L01P_PARAMS
};

/**
 * @brief   Number of NRF24L01+ device configurations
 */
#define NRF24L01P_NUM                       ARRAY_SIZE(nrf24l01p_params)

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_PARAMS_H */
/** @} */
