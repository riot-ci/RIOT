/*
 * Copyright (C) 2016 Unwired Devices <info@unwds.com>
 *               2017 Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx127x
 * @{
 * @file
 * @brief       Semtech SX127X internal functions
 *
 * @author      Eugene P. <ep@unwds.com>
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef SX127X_INTERNAL_H
#define SX127X_INTERNAL_H

#include <inttypes.h>
#include "sx127x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Constant values needed to compute the RSSI value
 */
#define RSSI_OFFSET_LF                              (-164)
#define RSSI_OFFSET_HF                              (-157)

/**
 * @brief   Tests the transceiver version type.
 *
 * @param[in] dev                      The sx127x device descriptor
 * @return true if test passed, false otherwise
 */
bool sx127x_test(sx127x_t *dev);

/**
 * @brief   Generates 32 bits random value based on the RSSI readings
 *
 *          This function sets the radio in LoRa mode and disables all
 *          interrupts from it. After calling this function either
 *          sx127x_set_rx_config or sx127x_set_tx_config functions must
 *          be called.
 *
 * @param[in] dev                      The sx127x device structure pointer
 *
 * @return random 32 bits value
 */
uint32_t sx127x_random(sx127x_t *dev);

/**
 * @brief   Writes the radio register at specified address.
 *
 * @param[in] dev                      The sx127x device structure pointer
 * @param[in] addr                     Register address
 * @param[in] data                     New register value
 */
void sx127x_reg_write(sx127x_t *dev, uint8_t addr, uint8_t data);

/**
 * @brief   Reads the radio register at specified address.
 *
 * @param[in] dev                      The sx127x device structure pointer
 * @param[in] addr                     Register address
 *
 * @return	Register value
 */
uint8_t sx127x_reg_read(sx127x_t *dev, uint8_t addr);

/**
 * @brief   Writes multiple radio registers starting at address (burst-mode).
 *
 * @param[in] dev                      The sx127x device structure pointer
 * @param[in] addr                     First radio register address
 * @param[in] buffer                   Buffer containing the new register's values
 * @param[in] size                     Number of registers to be written
 */
void sx127x_reg_write_burst(sx127x_t *dev, uint8_t addr, uint8_t *buffer,
                            uint8_t size);

/**
 * @brief   Reads multiple radio registers starting at address.
 *
 * @param[in]  dev                     The sx127x device structure pointer
 * @param[in]  addr                    First radio register address
 * @param[in]  size                    Number of registers to be read
 * @param[out] buffer                  Buffer where to copy registers data
 */
void sx127x_reg_read_burst(sx127x_t *dev, uint8_t addr, uint8_t *buffer,
                           uint8_t size);

/**
 * @brief   Writes the buffer contents to the SX1276 FIFO
 *
 * @param[in] dev                      The sx127x device structure pointer
 * @param[in] buffer                   Buffer Buffer containing data to be put on the FIFO.
 * @param[in] size                     Size Number of bytes to be written to the FIFO
 */
void sx127x_write_fifo(sx127x_t *dev, uint8_t *buffer, uint8_t size);

/**
 * @brief   Reads the contents of the SX1276 FIFO
 *
 * @param[in] dev                      The sx127x device structure pointer
 * @param[in] size                     Size Number of bytes to be read from the FIFO
 * @param[out] buffer                  Buffer Buffer where to copy the FIFO read data.
 */
void sx127x_read_fifo(sx127x_t *dev, uint8_t *buffer, uint8_t size);

/**
 * @brief   Performs the Rx chain calibration for LF and HF bands
 *
 *          Must be called just after the reset so all registers are at their
 *          default values
 *
 * @param[in] dev                      The sx127x device structure pointer
 */
void sx127x_rx_chain_calibration(sx127x_t *dev);

/**
 * @brief   Reads the current RSSI value.
 *
 * @param[in] dev                      The sx127x device descriptor
 *
 * @return current value of RSSI in [dBm]
 */
int16_t sx127x_read_rssi(sx127x_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* SX127X_INTERNAL_H */
