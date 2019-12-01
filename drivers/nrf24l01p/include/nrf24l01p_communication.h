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
 * @brief   Functions to communicate with the NRF24L01P transceiver, over SPI
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_COMMUNICATION_H
#define NRF24L01P_COMMUNICATION_H

#include "periph/gpio.h"
#include "periph/spi.h"
#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mask for 5 bit register address
 */
#define NRF24L01P_ADDR_MASK                 (0x1F)

/**
 * @brief   Mask for 3 bit pipe number
 */
#define NRF24L01P_PIPE_MASK                 (0x07)

/**
 * @brief   Read register value
 *          (1 to 5 bytes)
 */
#define NRF24L01P_CMD_R_REGISTER(reg) \
    (0x00 | ((reg) & NRF24L01P_ADDR_MASK))

/**
 * @brief   Write register value
 *          (1 to 5 bytes)
 */
#define NRF24L01P_CMD_W_REGISTER(reg) \
    (0x20 | ((reg) & NRF24L01P_ADDR_MASK))

/**
 * @brief   Read RX payload buffer
 *          (1 to 32 bytes)
 */
#define NRF24L01P_CMD_R_RX_PAYLOAD         (0x61)

/**
 * @brief   Write TX payload buffer
 *          (1 to 32 bytes)
 */
#define NRF24L01P_CMD_W_TX_PAYLOAD         (0xA0)

/**
 * @brief   Flush TX buffer
 *          (0 bytes data)
 */
#define NRF24L01P_CMD_FLUSH_TX             (0xE1)

/**
 * @brief   Flush RX buffer
 *          (0 bytes data)
 */
#define NRF24L01P_CMD_FLUSH_RX             (0xE2)

/**
 * @brief   Reuse last transmitted payload
 *          (0 bytes data)
 */
#define NRF24L01P_CMD_REUSE_TX_PL          (0xE3)

/**
 * @brief   Read RX payload width for the top R_RX_PAYLOAD in RX FIFO
 *          (1 byte data)
 */
#define NRF24L01P_CMD_R_RX_PL_WID          (0x60)

/**
 * @brief   Write payload to be transmitted together with ACK for certain pipe
 *          (1 to 32 bytes data)
 */
#define NRF24L01P_CMD_W_ACK_PAYLOAD(pipe) \
    (0xA8 | ((pipe) & NRF24L01P_PIPE_MASK))

/**
 * @brief   Disable auto ACK on this specific frame
 */
#define NRF24L01P_CMD_W_TX_PAYLOAD_NO_ACK  (0xB0)

/**
 * @brief   No operation
 */
#define NRF24L01P_CMD_NOP                  (0xFF)

/**
 * @brief   Acquire the SPI bus of the transceiver
 *
 * @param[in] dev       NRF24L01P device handle
 *
 * @return              @see spi_acquire
 */
int nrf24l01p_acquire(nrf24l01p_t *dev);

/**
 * @brief   Release the SPI bus of the transceiver
 *
 * @param[in] dev       NRF24L01P device handle
 *
 * @return               @see spi_release
 */
void nrf24l01p_release(nrf24l01p_t *dev);

/**
 * @brief   Read a register value
 *
 * @param[in] dev       NRF24L01P device handle
 * @param[in] reg       Register address
 * @param[out] dest     Output register
 * @param[in] len       Output register width
 *
 * @return              Status register value
 */
uint8_t nrf24l01p_read_reg(nrf24l01p_t *dev, uint8_t reg, uint8_t *dest,
                           size_t len);

/**
 * @brief   Write a register value
 *
 * @param[in] dev       NRF24L01P device handle
 * @param[in] reg       Register address
 * @param[in] src       Data to be written into the register
 * @param[in] len       Register width
 *
 * @return              Status register value
 */
uint8_t nrf24l01p_write_reg(nrf24l01p_t *dev, uint8_t reg, const uint8_t *src,
                            size_t len);

/**
 * @brief   Read received payload
 *
 * @param[in] dev       NRF24L01P device handle
 * @param[out] dest     Output payload buffer
 * @param[in] len       Output payload length
 *
 * @return              Status register value
 */
uint8_t nrf24l01p_read_rx_payload(nrf24l01p_t *dev, void *dest, size_t len);

/**
 * @brief   Write payload to be transmitted
 *
 * @param[in] dev       NRF24L01P device handle
 * @param[in] src       Input payload buffer
 * @param[in] len       Input payload length
 *
 * @return              Status register value
 */
uint8_t nrf24l01p_write_tx_payload(nrf24l01p_t *dev, const void *src,
                                   size_t len);

/**
 * @brief   Flush TX FIFO
 *
 * @param[in] dev       NRF24L01P device handle
 *
 * @return              Status register value
 */
uint8_t nrf24l01p_flush_tx(nrf24l01p_t *dev);

/**
 * @brief   Flush RX FIFO
 *
 * @param[in] dev       NRF24L01P device handle
 *
 * @return              Status register value
 */
uint8_t nrf24l01p_flush_rx(nrf24l01p_t *dev);

/**
 * @brief   Reuse TX payload in FIFO
 *
 * This function can be used to reattempt to transmit a frame, after a MAX_RT
 * interrupt was triggered.
 *
 * @param[in] dev       NRF24L01P device handle
 *
 * @return              Status register value
 */
uint8_t nrf24l01p_reuse_tx_pl(nrf24l01p_t *dev);

/**
 * @brief   Read received payload width
 *
 * @param[in] dev       NRF24L01P device handle
 * @param[out] dest     Payload width
 *
 * @return              Status register value
 */
uint8_t nrf24l01p_read_rx_pl_width(nrf24l01p_t *dev, uint8_t *dest);

/**
 * @brief   Write Payload to be transmitted in an ACK frame
 *
 * @param[in] dev       NRF24L01P device handle
 * @param[in] src       Input payload buffer
 * @param[in] len       Input payload length
 * @param[in] pipe      Index of data pipe
 *
 * @return              Status register value
 */
uint8_t nrf24l01p_write_ack_pl(nrf24l01p_t *dev, const void *src, size_t len,
                               uint8_t pipe);

/**
 * @brief   Write payload to be transmitted but do not expect an ACK for
 *          this frame
 *
 * @param[in] dev       NRF24L01P device handle
 * @param[in] src       Input payload buffer
 * @param[in] len       Input payload length
 *
 * @return              Status register value
 */
uint8_t nrf24l01p_write_tx_pl_no_ack(nrf24l01p_t *dev, const void *src,
                                     size_t len);

/**
 * @brief   Get status register value
 *
 * @param[in] dev   NRF24L01P device handle
 *
 * @return          Status register value
 */
uint8_t nrf24l01p_get_status(nrf24l01p_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_COMMUNICATION_H */
/** @} */
