/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    drivers_nrf24l01p NRF24L01P 2.4 GHz trasceiver driver
 * @ingroup     drivers_netdev
 *
 * This module contains the driver for the NRF24L01P 2.4 GHz
 * transceiver.
 *
 * @{
 * @file
 * @brief   Public interface for NRF24L01P devices
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_H
#define NRF24L01P_H

#include <stdint.h>

#include "net/gnrc/nettype.h"
#include "net/netdev.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Minimum width of a NRF24L01P layer-2 address
 */
#define NRF24L01P_MIN_ADDR_WIDTH            (3)

/**
 * @brief   Maximum width of a NRF24L01P layer-2 address
 */
#define NRF24L01P_MAX_ADDR_WIDTH            (5)

/**
 * @brief ShockBurst
 */
#define NRF24L01P_SHOCKBURST                (0)

/**
 * @brief   Enhanded ShockBurst
 */
#define NRF24L01P_ENHANCED_SHOCKBURST       (1)

/**
 * @brief NRF24L01P operation states
 */
typedef enum nrf24l01p_state {
    NRF24L01P_STATE_UNDEFINED   =   2,      /**< State right after voltage supply */
    NRF24L01P_STATE_POWER_DOWN  =   4,      /**< Register values are available and maintained, SPI active*/
    NRF24L01P_STATE_STANDBY_1   =   8,      /**< Idle*/
    NRF24L01P_STATE_STANDBY_2   =  16,      /**< TX FIFO empty, fill up TX FIFO again*/
    NRF24L01P_STATE_RX_MODE     =  32,      /**< Baseband protocol engine constantly searches for a valid packet */
    NRF24L01P_STATE_TX_MODE     =  64,      /**< Transmit next packet */
} nrf24l01p_state_t;

/**
 * @brief   Possible protocols for NRF24L01P
 */
typedef enum {
    NRF24L01P_PROTOCOL_SB   = NRF24L01P_SHOCKBURST,         /**< ShockBurst */
    NRF24L01P_PROTOCOL_ESB  = NRF24L01P_ENHANCED_SHOCKBURST /**< EnhancedShockBurst */
} nrf24l01p_protocol_t;

/**
 * @brief   Enumeration of NRF24L01P data pipes
 */
typedef enum {
    NRF24L01P_P0    = 0,    /**< Pipe 0 */
    NRF24L01P_P1    = 1,    /**< Pipe 1 */
    NRF24L01P_P2    = 2,    /**< Pipe 2 */
    NRF24L01P_P3    = 3,    /**< Pipe 3 */
    NRF24L01P_P4    = 4,    /**< Pipe 4 */
    NRF24L01P_P5    = 5,    /**< Pipe 5 */
    NRF24L01P_PX_NUM_OF     /**< Number of supported pipes */
} nrf24l01p_pipe_t;

/**
 * @brief   Possible values to configure the layer-2 address width
 */
typedef enum {
    NRF24L01P_AW_3byte  = 1,        /**< value to use a 3 bytes long layer-2 address */
    NRF24L01P_AW_4byte  = 2,        /**< value to use a 4 bytes long layer-2 address */
    NRF24L01P_AW_5byte  = 3,        /**< value to use a 5 bytes long layer-2 address */
    NRF24L01P_AW_NUM_OF             /**< Number of possible values to configure the layer-2 address width */
} nrf24l01p_aw_t;

/**
 * @brief   Possible values to configure the retransmission delay in ESB
 */
typedef enum {
    NRF24L01P_ARD_250us     = 0,    /**< 250 us */
    NRF24L01P_ARD_500us     = 1,    /**< 500 us */
    NRF24L01P_ARD_750us     = 2,    /**< 750 us */
    NRF24L01P_ARD_1000us    = 3,    /**< 1000 us */
    NRF24L01P_ARD_1250us    = 4,    /**< 1250 us */
    NRF24L01P_ARD_1500us    = 5,    /**< 1500 us */
    NRF24L01P_ARD_1750us    = 6,    /**< 1750 us */
    NRF24L01P_ARD_2000us    = 7,    /**< 2000 us */
    NRF24L01P_ARD_2250us    = 8,    /**< 2250 us */
    NRF24L01P_ARD_2500us    = 9,    /**< 2500 us */
    NRF24L01P_ARD_2750us    = 10,   /**< 2750 us */
    NRF24L01P_ARD_3000us    = 11,   /**< 3000 us */
    NRF24L01P_ARD_3250us    = 12,   /**< 3250 us */
    NRF24L01P_ARD_3500us    = 13,   /**< 3500 us */
    NRF24L01P_ARD_3750us    = 14,   /**< 3750 us */
    NRF24L01P_ARD_4000us    = 15,   /**< 4000 us */
    NRF24L01P_ARD_NUM_OF            /**< Number of possible values to configure the retransmission delay */
} nrf24l01p_ard_t;

/**
 * @brief   Possible values to configure the CRC length
 */
typedef enum {
    NRF24L01P_CRCO_0    = 1,    /**< 0 bytes CRC length */
    NRF24L01P_CRCO_1    = 2,    /**< 1 byte CRC length */
    NRF24L01P_CRCO_2    = 3,    /**< 2 bytes CRC length */
    NRF24L01P_CRCO_NUM_OF       /**< Number of possible values to configure CRC length */
} nrf24l01p_crco_t;

/**
 * @brief   Possible values to configure the radio power
 */
typedef enum {
    NRF24L01P_RF_PWR_MINUS_18dBm    = 0,    /**< -18 dBm */
    NRF24L01P_RF_PWR_MINUS_12dBm    = 1,    /**< -12 dBm */
    NRF24L01P_RF_PWR_MINUS_6dBm     = 2,    /**<  -6 dBm */
    NRF24L01P_RF_PWR_0dBm           = 3,    /**<   0 dBm */
    NRF24L01P_RF_PWR_NUM_OF                 /**< Number of possible values to configure the radio power */
} nrf24l01p_rfpwr_t;

/**
 * @brief   Possible values to configure the data rate
 */
typedef enum {
    NRF24L01P_RF_DR_1Mbps   = 0,    /**< 1 Mbit/s */
    NRF24L01P_RF_DR_250kbps = 1,    /**< 250 kbit/s */
    NRF24L01P_RF_DR_2Mbps   = 2,    /**< 2 Mbit/s */
    NRF24L01P_RF_DR_NUM_OF          /**< Number of possible values to configure the data rate */
} nrf24l01p_rfdr_t;

/**
 * @brief   Struct that holds all active configuration values
 */
typedef struct {
    /**
     * @brief Padding for byte alignment
     */
    uint8_t :                   5;
    /**
     * @brief   Currently configured protocol (Do not change after initialization)
     *          @see nrf24l01p_protocol_t
     */
    uint8_t cfg_protocol :       1;
    /**
     * @brief   Current CRC length configuration value
     *          @see nrf24l01p_crco_t
     */
    uint8_t cfg_crc :            2;
    /**
     * @brief   Current tx power configuration value
     *          @see nrf24l01p_rfpwr_t
     */
    uint8_t cfg_tx_power :       2;
    /**
     * @brief   Current data rate configuration value
     *          @see nrf24l01p_rfdr_t
     */
    uint8_t cfg_data_rate :      2;
    /**
     * @brief   Current channel
     */
    uint8_t cfg_channel :        4;
    /**
     * @brief   Current address width configuration value (Do not change after initialization
     *          @see nrf24l01p_aw_t
     */
    uint8_t cfg_addr_width :     2;
    /**
     * @brief   Current maximum number of retransmissions
     *          (Only used if protocol is ESB)
     */
    uint8_t cfg_max_retr :       4;
    /**
     * @brief   Current retransmission delay configuration value
     *          (Only used if protocol is ESB) @see nrf24l01p_ard_t
     */
    uint8_t cfg_retr_delay :     4;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 0 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p0 :    5;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 1 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p1 :    5;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 2 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p2 :    5;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 3 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p3 :    5;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 4 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p4 :    5;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 5 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p5 :    5;
} nrf24l01p_cfg_t;

/**
 * @brief Struct of NRF24L01P initialization parameters
 */
typedef struct {
    spi_t spi;          /**< SPI bus */
    spi_clk_t spi_clk;  /**< SPI clock speed */
    gpio_t pin_cs;      /**< SPI chip select gpio pin */
    gpio_t pin_ce;      /**< NRF24L01P chip enable gpio pin */
    gpio_t pin_irq;     /**< NRF24L01P IRQ gpio pin */
    union {
        struct {
            /**
             * @brief   Array to access the addresses of pipe 0 and pipe 1 via
             *          pipe indices @see nrf24l01p_pipe_t
             */
            uint8_t rx_addr_long[2][NRF24L01P_MAX_ADDR_WIDTH];
            /**
             * @brief   Array to access the addresses of pipe 2, pipe 3,
             *          pipe 4 and pipe 5 via pipe indices
             *          @see nrf24l01p_pipe_t
             */
            uint8_t rx_addr_short[4];
        } arxaddr; /**< Rx addresses as arrays */
        struct {
            /**
             * @brief   pipe 0 Rx address
             */
            uint8_t rx_pipe_0_addr[NRF24L01P_MAX_ADDR_WIDTH];
            /**
             * @brief   pipe 1 Rx address
             */
            uint8_t rx_pipe_1_addr[NRF24L01P_MAX_ADDR_WIDTH];
            /**
             * @brief   pipe 2 Rx address
             */
            uint8_t rx_pipe_2_addr;
            /**
             * @brief   pipe 3 Rx address
             */
            uint8_t rx_pipe_3_addr;
            /**
             * @brief   pipe 4 Rx address
             */
            uint8_t rx_pipe_4_addr;
            /**
             * @brief   pipe 5 Rx address
             */
            uint8_t rx_pipe_5_addr;
        } rxaddrpx; /**< Rx addresses as named variables */
    } urxaddr; /**< Union of Rx addresses as named variables and as arrays */
    nrf24l01p_cfg_t config; /**< Current configuration values */
} nrf24l01p_params_t;

/**
 * @brief   NRF24L01P device struct
 */
typedef struct {
    netdev_t netdev;            /**< Netdev member */
    nrf24l01p_params_t params;  /**< Parameters */
    /**
     * @brief Destination address as PTX
     *        A PTX node must change pipe 0 Rx address to Tx address in order to receive ACKs.
     *        If node switches back to Rx mode, pipe 0 Rx address must be restored from params.
     */
    uint8_t tx_addr[NRF24L01P_MAX_ADDR_WIDTH];
    uint8_t tx_addr_len;        /**< Tx address length */
    uint8_t state;              /**< Current operation state */
#if !defined (NDEBUG) || defined(DOXYGEN)
    uint8_t have_spi_access;    /**< != 0: dev already has SPI bus acquired*/
    uint8_t transitions;        /**< Possible transitions from current state */
#endif
} nrf24l01p_t;

/**
 * @brief Get state variable as a string
 *
 * @param[in] state     State
 *
 * @return              @p state as a string
 */
const char *nrf24l01p_state_to_string(nrf24l01p_state_t state);

/**
 * @brief Convert string to state variable
 *
 * @param[in] sstate    State string
 *
 * @return              State variable
 */
nrf24l01p_state_t nrf24l01p_string_to_state(const char *sstate);

/**
 * @brief   Setup the NRF24L01P driver, but perform no initialization
 *
 * @ref netdev_driver_t::init can be used after this call to initialize the
 * transceiver.
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] params        Parameters of the device to setup
 *
 * @retval 0                Device successfully set up
 * @retval -ENOTSUP         Parameter request could be satisfied
 */
int nrf24l01p_setup(nrf24l01p_t *dev, const nrf24l01p_params_t *params);

/**
 * @brief   Configure air data rate
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] data_rate     Data rate configuration value
 *
 * @retval 0                Success
 * @retval -EINVAL         Bad data rate value
 * @retval -EAGAIN          Current state does not permit changing data rate
 */
int nrf24l01p_set_air_data_rate(nrf24l01p_t *dev, nrf24l01p_rfdr_t data_rate);

/**
 * @brief   Get currently configured data rate
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[out] data_rate    Configuration data rate value (may be NULL)
 *
 * @return                  Data rate in [kbit/s]
 */
uint16_t nrf24l01p_get_air_data_rate(nrf24l01p_t *dev,
                                     nrf24l01p_rfdr_t *data_rate);

/**
 * @brief   Configure CRC length
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] crc           Configuration CRC value
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad CRC configuration value
 * @retval -EAGAIN          Current state does not permit changing CRC length
 */
int nrf24l01p_set_crc(nrf24l01p_t *dev, nrf24l01p_crco_t crc);

/**
 * @brief   Get currently configured CRC length
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[out] crc          Configuration crc value (may be NULL)
 *
 * @return                  Current CRC length
 */
uint8_t nrf24l01p_get_crc(nrf24l01p_t *dev, nrf24l01p_crco_t *crc);

/**
 * @brief   Configure Tx trasceiver power
 *
 * @param[in] dev           NRf24L01P device handle
 * @param[in] power         Configuration Tx power value
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad Tx power configuration value
 * @retval -EAGAIN          Current state does not permit changin Tx power
 */
int nrf24l01p_set_tx_power(nrf24l01p_t *dev, nrf24l01p_rfpwr_t power);

/**
 * @brief   Get currently configured Tx transceiver power
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[out] power        Configuration Tx power value
 *
 * @return                  Tx power in [dbm]
 */
int8_t nrf24l01p_get_tx_power(nrf24l01p_t *dev, nrf24l01p_rfpwr_t *power);

/**
 * @brief   Set transceiver channel
 *
 * @param[in] dev           NRF24l01P device handle
 * @param[in] channel       Channel [0; 15]
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad channel
 * @retval -EAGAIN          Current state does not permit switching channel
 */
int nrf24l01p_set_channel(nrf24l01p_t *dev, uint8_t channel);

/**
 * @brief   Get currently configured transceiver channel
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @return                  Transceiver channel
 */
uint8_t nrf24l01p_get_channel(nrf24l01p_t *dev);

/**
 * @brief   Configure expected MTU of a certain data pipe
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] mtu           MTU
 * @param[in] pipe          Pipe index
 *
 * @retval 0                Success
 * @retval -ERANGE          Bad pipe index
 * @retval -EINVAL          Bad payload width
 * @retval -EAGAIN          Current state does not permit changing payload width
 */
int nrf24l01p_set_mtu(nrf24l01p_t *dev, uint8_t mtu, nrf24l01p_pipe_t pipe);

/**
 * @brief   Get currently configured expected MTU of a certain data pipe
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] pipe          Pipe index
 *
 * @return                  MTU
 * @retval NRF24L_MTU       Dynamic length for ESB protocol
 * @retval -ERANGE          Bad pipe index
 */
int nrf24l01p_get_mtu(nrf24l01p_t *dev, nrf24l01p_pipe_t pipe);

/**
 * @brief   Set Rx address of a certain data pipe
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] addr          Rx address
 * @param[in] addr_len      Address length (must match current address length)
 * @param[in] pipe          Pipe index
 *
 * @retval 0                Success
 * @retval -ERANGE          Bad
 * @retval -EINVAL          Bad address length
 * @return -EAGAIN          Current state does not permit changin Rx address
 */
int nrf24l01p_set_rx_address(nrf24l01p_t *dev, const uint8_t *addr,
                             size_t addr_len, nrf24l01p_pipe_t pipe);

/**
 * @brief   Get current Rx address of a certain data pipe
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[out] addr         Rx address
 * @param[in] pipe          Pipe index
 *
 * @return                  Address width
 * @retval -ERANGE          Bad pipe index
 */
int nrf24l01p_get_rx_address(nrf24l01p_t *dev, uint8_t *addr,
                             nrf24l01p_pipe_t pipe);

/**
 * @brief   Configure maximum number of retransmissions for ESB
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] max_rt        Number of maximum retransmissions [0; 15]
 *
 * @retval 0                Success
 * @retval -ENOTSUP         Protocol is SB
 * @retval -EINVAL          Unsupported number of retransmissions
 * @retval -EAGAIN          State does not permit changing the maximum number of retransmissions
 */
int nrf24l01p_set_max_retransm(nrf24l01p_t *dev, uint8_t max_rt);

/**
 * @brief   Get currently configured number of maximum retransmissions for ESB
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @return                  Maximum number of retransmissions
 */
uint8_t nrf24l01p_get_max_retransm(nrf24l01p_t *dev);

/**
 * @brief   Set retransmission delay for ESB
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] rt_delay      Configuration retransmission delay value
 *
 * @return 0
 * @retval -ENOTSUP         Protocol is SB
 * @return -EINVAL          Bad retransmission delay value
 * @return -EAGAIN          Current state does not permit changing retransmission delay
 */
int nrf24l01p_set_retransm_delay(nrf24l01p_t *dev, nrf24l01p_ard_t rt_delay);

/**
 * @brief   Get retransmission delay for ESB
 *
 * @param[in] dev          NRF24L01P device handle
 * @param[out] rt_delay    Configuration retransmission delay value
 *
 * @return                  Retransmission delay in [us]
 */
uint16_t nrf24l01p_get_retransm_delay(nrf24l01p_t *dev,
                                      nrf24l01p_ard_t *rt_delay);

/**
 * @brief   Write payload to be transmitted in an ACK frame
 *
 *          The ACK payload is flushed if a MAX_RT interrupt occurs.
 *          The ACK payload must be set in advance of the reception of a frame.
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 * @param[in] pipe          Pipe index
 *
 * @retval 0                Success
 * @retval -ENOTSUP         Protocol is SB
 * @retval -EINVAL          Payload too big
 * @retval -ERANGE          Bad pipe index
 * @retval -EAGAIN          Current state does not permit setting ACK payload
 */
int nrf24l01p_set_ack_payload(nrf24l01p_t *dev, const void *payload,
                              size_t payload_width, nrf24l01p_pipe_t pipe);

/**
 * @brief   Put device into
 *          sleep mode(@ref NRF24L01P_STATE_POWER_DOWN),
 *          standby mode (@ref NRF24L01P_STATE_STANDBY_1),
 *          or Rx mode (@ref NRF24L01P_STATE_RX_MODE)
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] state         State
 *
 * @return                  Old state
 * @retval -EAGAIN          Device is currently not permitted to change it´s state
 * @retval -ENOTSUP         Device is not permitted to change state to @p state
 */
int nrf24l01p_set_state(nrf24l01p_t *dev, nrf24l01p_state_t state);

/**
 * @brief   Get current device state
 *
 * @param[in] dev             NRf24L01P device handle
 *
 * @return                    Device state
 */
nrf24l01p_state_t nrf24l01p_get_state(nrf24l01p_t *dev);

/**
 * @brief   Convert @ref nrf24l01p_aw_t to actual address width
 *
 * @param[in] address_width Address width enum
 *
 * @return                  Address width in [bytes]
 */
static inline uint8_t nrf24l01p_etoval_aw(nrf24l01p_aw_t address_width)
{
    if (address_width <= NRF24L01P_AW_3byte) {
        return 3;
    }
    if (address_width == NRF24L01P_AW_4byte) {
        return 4;
    }
    return 5;
}

/**
 * @brief   Convert address width in [bytes] to @ref nrf24l01p_aw_t
 *
 * @param[in] address_width Address width in [bytes]
 *
 * @return                  Corresponding enum
 */
static inline nrf24l01p_aw_t nrf24l01p_valtoe_aw(uint8_t address_width)
{
    if (address_width <= 3) {
        return NRF24L01P_AW_3byte;
    }
    if (address_width == 4) {
        return NRF24L01P_AW_4byte;
    }
    return NRF24L01P_AW_5byte;
}

/**
 * @brief   Convert @ref nrf24l01p_ard_t to actual retransmission delay
 *
 * @param[in] retr_delay    Retransmission delay enum
 *
 * @return                  Retransmission delay in [us]
 */
static inline uint16_t nrf24l01p_etoval_ard(nrf24l01p_ard_t retr_delay)
{
    if (retr_delay >= NRF24L01P_ARD_4000us) {
        return 4000;
    }
    return (retr_delay + 1) * 250;
}

/**
 * @brief   Convert retransmission delay in [us] to @ref nrf24l01p_ard_t
 *
 * @param[in] retr_delay    Retransmission delay in [us]
 *
 * @return                  Corresponding enum
 */
static inline nrf24l01p_ard_t nrf24l01p_valtoe_ard(uint16_t retr_delay)
{
    if (retr_delay >= 4000) {
        return NRF24L01P_ARD_4000us;
    }
    return retr_delay / 250;
}

/**
 * @brief   Convert @ref nrf24l01p_crco_t to actual CRC length
 *
 * @param[in] crc_len       CRC length enum
 *
 * @return                  CRC length in [bytes]
 */
static inline uint8_t nrf24l01p_etoval_crco(nrf24l01p_crco_t crc_len)
{
    if (crc_len <= NRF24L01P_CRCO_0) {
        return 0;
    }
    if (crc_len == NRF24L01P_CRCO_1) {
        return 1;
    }
    return 2;
}

/**
 * @brief   Convert CRC length in [bytes] to @ref nrf24l01p_crco_t
 *
 * @param[in] crc_len       CRC length in [bytes]
 *
 * @return                  Corresponding enum
 */
static inline nrf24l01p_crco_t nrf24l01p_valtoe_crco(uint8_t crc_len)
{
    if (!crc_len) {
        return NRF24L01P_CRCO_0;
    }
    if (crc_len == 1) {
        return NRF24L01P_CRCO_1;
    }
    return NRF24L01P_CRCO_2;
}

/**
 * @brief   Convert @ref nrf24l01p_rfpwr_t to actual Tx power
 *
 * @param[in] power         RF power enum
 *
 * @return                  RF power in [dbm]
 */
static inline int8_t nrf24l01p_etoval_rfpwr(nrf24l01p_rfpwr_t power)
{
    if (power <= NRF24L01P_RF_PWR_MINUS_18dBm) {
        return -18;
    }
    if (power == NRF24L01P_RF_PWR_MINUS_12dBm) {
        return -12;
    }
    if (power == NRF24L01P_RF_PWR_MINUS_6dBm) {
        return -6;
    }
    return 0;
}

/**
 * @brief   Convert RF power in [dbm] to @ref nrf24l01p_rfpwr_t
 *
 * @param[in] power         RF power in [dbm]
 *
 * @return                  Corresponding enum
 */
static inline nrf24l01p_rfpwr_t nrf24l01p_valtoe_rfpwr(int16_t power)
{
    if (power <= -18) {
        return NRF24L01P_RF_PWR_MINUS_18dBm;
    }
    if (power <= -12) {
        return NRF24L01P_RF_PWR_MINUS_12dBm;
    }
    if (power <= -6) {
        return NRF24L01P_RF_PWR_MINUS_6dBm;
    }
    return NRF24L01P_RF_PWR_0dBm;
}

/**
 * @brief   Convert @ref nrf24l01p_rfdr_t to actual air data rate
 *
 * @param[in] data_rate     Air data rate enum
 *
 * @return                  Air data rate in [kbit/s]
 */
static inline uint16_t nrf24l01p_etoval_rfdr(nrf24l01p_rfdr_t data_rate)
{
    if (data_rate <= NRF24L01P_RF_DR_1Mbps) {
        return 1000;
    }
    if (data_rate == NRF24L01P_RF_DR_250kbps) {
        return 250;
    }
    return 2000;
}

/**
 * @brief   Convert Air data rate in [kbit/s] to @ref nrf24l01p_rfdr_t
 *
 * @param[in] data_rate     Air data rate in [kbit/s]
 *
 * @return                  Corresponding enum
 */
static inline nrf24l01p_rfdr_t nrf24l01p_valtoe_rfdr(uint16_t data_rate)
{
    if (data_rate <= 250) {
        return NRF24L01P_RF_DR_250kbps;
    }
    if (data_rate <= 1000) {
        return NRF24L01P_RF_DR_1Mbps;
    }
    return NRF24L01P_RF_DR_2Mbps;
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 0
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p0(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P0);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 1
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p1(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 2
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p2(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P2);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 3
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p3(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P3);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 4
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p4(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P4);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 5
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p5(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P5);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 0
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p0(nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P0);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 1
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p1(nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 2
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p2(nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P2);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 3
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p3(nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P3);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 4
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p4(nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P4);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 5
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p5(nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P5);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 0
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p0(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P0);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 1
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p1(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 2
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p2(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P2);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 3
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p3(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P3);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 4
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p4(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P4);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 5
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p5(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P5);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 0
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p0(nrf24l01p_t *dev, uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P0);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 1
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p1(nrf24l01p_t *dev, uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 2
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p2(nrf24l01p_t *dev, uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P2);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 3
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p3(nrf24l01p_t *dev, uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P3);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 4
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p4(nrf24l01p_t *dev, uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P4);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 4
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p5(nrf24l01p_t *dev, uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P5);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 0
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p0(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P0);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 1
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p1(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 2
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p2(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P2);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 3
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p3(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P3);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 4
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p4(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P4);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 5
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p5(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P5);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_state
 *          to go to sleep mode
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @return                  @see nrf24l01p_set_state
 */
static inline int nrf24l01p_set_state_sleep(nrf24l01p_t *dev)
{
    return nrf24l01p_set_state(dev, NRF24L01P_STATE_POWER_DOWN);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_state
 *          to go to idle mode
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @return                  @see nrf24l01p_set_state
 */
static inline int nrf24l01p_set_state_idle(nrf24l01p_t *dev)
{
    return nrf24l01p_set_state(dev, NRF24L01P_STATE_STANDBY_1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_state
 *          to go to Rx mode
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @return                  @see nrf24l01p_set_state
 */
static inline int nrf24l01p_set_state_rx(nrf24l01p_t *dev)
{
    return nrf24l01p_set_state(dev, NRF24L01P_STATE_RX_MODE);
}

#ifdef MODULE_NRF24L01P_DIAGNOSTICS
/**
 * @brief Print all registers
 *
 * @param[in] dev       NRf24L01P device handle
 */
void nrf24l01p_print_all_regs(nrf24l01p_t *dev);
/**
 * @brief Print device parameters
 *
 * @param[in] dev       NRf24L01P device handle
 */
void nrf24l01p_print_dev_info(nrf24l01p_t *dev);
#endif

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_H */
/** @} */
