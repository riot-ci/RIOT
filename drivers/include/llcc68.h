/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_llcc68 LLCC68 LoRa radio driver
 * @ingroup     drivers_netdev
 * @brief       Driver for the LLCC68 LoRa radio device
 *
 * @{
 *
 * @file
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LLCC68_H
#define LLCC68_H

#include "llcc68_driver.h"

#include "net/netdev.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    spi_t spi;                          /**< SPI device */
    gpio_t nss_pin;                     /**< SPI NSS pin */
    gpio_t reset_pin;                   /**< Reset pin */
    gpio_t busy_pin;                    /**< Busy pin */
    gpio_t dio1_pin;                    /**< Dio1 pin */
} llcc68_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    netdev_t netdev;                        /**< Netdev parent struct */
    llcc68_params_t *params;                /**< Initialization parameters */
    llcc68_pkt_params_lora_t pkt_params;    /**< Lora packet parameters */
    llcc68_mod_params_lora_t mod_params;    /**< Lora modulation parameters */
    uint32_t channel;                       /**< Current channel frequency (in Hz) */
} llcc68_t;

/**
 * @brief   Setup the radio device
 *
 * @param[in] dev                       Device descriptor
 * @param[in] params                    Parameters for device initialization
 * @param[in] index                     Index of @p params in a global parameter struct array.
 *                                      If initialized manually, pass a unique identifier instead.
 */
void llcc68_setup(llcc68_t *dev, const llcc68_params_t *params, uint8_t index);

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev                    Device descriptor of the driver
 * @param[in]    params                 Initialization parameters
 *
 * @return                  0 on success
 */
int llcc68_init(llcc68_t *dev);

/**
 * @brief   Gets the channel RF frequency.
 *
 * @param[in]  dev                     Device descriptor of the driver
 *
 * @return The channel frequency
 */
uint32_t llcc68_get_channel(const llcc68_t *dev);

/**
 * @brief   Sets the channel RF frequency.
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] freq                     Channel RF frequency
 */
void llcc68_set_channel(llcc68_t *dev, uint32_t freq);

/**
 * @brief   Gets the LoRa bandwidth
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the bandwidth
 */
uint8_t llcc68_get_bandwidth(const llcc68_t *dev);

/**
 * @brief   Sets the LoRa bandwidth
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] bandwidth                The new bandwidth
 */
void llcc68_set_bandwidth(llcc68_t *dev, uint8_t bandwidth);

/**
 * @brief   Gets the LoRa spreading factor
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the spreading factor
 */
uint8_t llcc68_get_spreading_factor(const llcc68_t *dev);

/**
 * @brief   Sets the LoRa spreading factor
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] sf                       The spreading factor
 */
void llcc68_set_spreading_factor(llcc68_t *dev, uint8_t sf);

/**
 * @brief   Gets the LoRa coding rate
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the current LoRa coding rate
 */
uint8_t llcc68_get_coding_rate(const llcc68_t *dev);

/**
 * @brief   Sets the LoRa coding rate
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] cr                       The LoRa coding rate
 */
void llcc68_set_coding_rate(llcc68_t *dev, uint8_t cr);

/**
 * @brief   Gets the payload length
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the payload length
 */
uint8_t llcc68_get_lora_payload_length(const llcc68_t *dev);

/**
 * @brief   Sets the payload length
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] len                      The payload len
 */
void llcc68_set_lora_payload_length(llcc68_t *dev, uint8_t len);

/**
 * @brief   Checks if CRC verification mode is enabled
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the LoRa single mode
 */
bool llcc68_get_lora_crc(const llcc68_t *dev);

/**
 * @brief   Enable/Disable CRC verification mode
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] crc                      The CRC check mode
 */
void llcc68_set_lora_crc(llcc68_t *dev, bool crc);

/**
 * @brief   Gets the LoRa implicit header mode
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the LoRa implicit mode
 */
bool llcc68_get_lora_implicit_header(const llcc68_t *dev);

/**
 * @brief   Sets LoRa implicit header mode
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] mode                     The header mode
 */
void llcc68_set_lora_implicit_header(llcc68_t *dev, bool mode);

/**
 * @brief   Gets the LoRa preamble length
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the preamble length
 */
uint16_t llcc68_get_lora_preamble_length(const llcc68_t *dev);

/**
 * @brief   Sets the LoRa preamble length
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] preamble                 The LoRa preamble length
 */
void llcc68_set_lora_preamble_length(llcc68_t *dev, uint16_t preamble);

/**
 * @brief   Checks if the LoRa inverted IQ mode is enabled/disabled
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return the LoRa IQ inverted mode
 */
bool llcc68_get_lora_iq_invert(const llcc68_t *dev);

/**
 * @brief   Enable/disable the LoRa IQ inverted mode
 *
 * @param[in] dev                      Device descriptor of the driver
 * @param[in] iq_invert                The LoRa IQ inverted mode
 */
void llcc68_set_lora_iq_invert(llcc68_t *dev, bool iq_invert);

#ifdef __cplusplus
}
#endif

#endif /* LLCC68_H */
/** @} */
