/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_ata8520
 * @{
 *
 * @file
 *
 * @brief       Device interface for Atmel ATA8520 SigFox transceiver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef ATA8520_H
#define ATA8520_H

#include <stdint.h>
#include <inttypes.h>
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name SIGFOX constants
 * @{
 */
#define SIGFOX_PAC_LENGTH    (16U)          /**< Length of SigFox PAC */
#define SIGFOX_ID_LENGTH     (4U)           /**< Length of SigFox ID */
/** @} */

enum {
    ATA8520_OK = 0,                         /**< Everything is fine */
    ATA8520_ERR_SPI,                        /**< An error occurred with SPI */
    ATA8520_ERR_GPIO_INT,                   /**< An error occurred with interrupt pin */
    ATA8520_ERR_GPIO_RESET,                 /**< An error occurred with reset pin */
    ATA8520_ERR_GPIO_SLEEP,                 /**< An error occurred with sleep pin */
};

typedef enum {
    ATA8520_SSM,                            /**< SoC System Management status */
    ATA8520_ATMEL,                          /**< Atmel status */
    ATA8520_SIGFOX                          /**< SigFox status */
} ata8520_status_type_t;

/* SigFox statuses. See ATA8520 datasheet, section 2.1.2.10, page 9 */
typedef enum {
    ATA8520_ATMEL_OK,                       /**< No error */
    ATA8520_ATMEL_COMMAND_ERROR,            /**< Command error / not supported */
    ATA8520_ATMEL_GENERIC_ERROR,            /**< Generic error */
    ATA8520_ATMEL_FREQUENCY_ERROR,          /**< Frequency error */
    ATA8520_ATMEL_USAGE_ERROR,              /**< Usage error */
    ATA8520_ATMEL_OPENING_ERROR,            /**< Opening error */
    ATA8520_ATMEL_CLOSING_ERROR,            /**< Closing error */
    ATA8520_ATMEL_SEND_ERROR                /**< Send error */
} ata8520_atmel_status_t;

/* SigFox statuses. See ATA8520 datasheet, section 2.1.2.10, page 9 */
typedef enum {
    ATA8520_SIGFOX_OK,                      /**< No error */
    ATA8520_SIGFOX_MANUFACTURER_ERROR,      /**< Manufacturer error */
    ATA8520_SIGFOX_ID_OR_KEY_ERROR,         /**< ID or key error */
    ATA8520_SIGFOX_STATE_MACHINE_ERROR,     /**< State machine error */
    ATA8520_SIGFOX_FRAME_SIZE_ERROR,        /**< Frame size error */
    ATA8520_SIGFOX_MANUFACTURER_SEND_ERROR, /**< Manufacturer send error */
    ATA8520_SIGFOX_GET_VOLTAGE_TEMP_ERROR,  /**< Get voltage/temperature error */
    ATA8520_SIGFOX_CLOSE_ERROR,             /**< Close issues encountered */
    ATA8520_SIGFOX_API_ERROR,               /**< API error indication */
    ATA8520_SIGFOX_GET_PN9_ERROR,           /**< Error getting PN9 */
    ATA8520_SIGFOX_GET_FREQUENCY_ERROR,     /**< Error getting frequency */
    ATA8520_SIGFOX_BUILDING_FRAME_ERROR,    /**< Error building frame */
    ATA8520_SIGFOX_DELAY_ROUTINE_ERROR,     /**< Error in delay routine */
    ATA8520_SIGFOX_CALLBACK_ERROR,          /**< Callback causes error */
    ATA8520_SIGFOX_TIMING_ERROR,            /**< Timing error */
    ATA8520_SIGFOX_FREQUENCY_ERROR          /**< Frequency error */
} ata8520_sigfox_status_t;

/**
 * @brief Bus parameters for ATA8520.
 */
typedef struct {
    spi_t spi;                              /**< SPI device */
    spi_clk_t spi_clk;                      /**< SPI clock speed */
    gpio_t cs_pin;                          /**< Chip select pin */
    gpio_t int_pin;                         /**< IRQ pin */
    gpio_t sleep_pin;                       /**< Poweron pin */
    gpio_t reset_pin;                       /**< Reset pin */
} ata8520_params_t;

typedef struct {
    ata8520_params_t params;                /**< Device parameters */
} ata8520_t;

/**
 * @brief Initialize the ATA8520
 *
 * This will also initialize the CS pin as a GPIO output, without pull resistors
 * and the interrupt pin handler to manage events.
 *
 * @param[out] dev                Pointer to device descriptor
 * @param[in]  spi_params         Pointer to SPI settings
 *
 * @return                        ALL_OK on success
 * @return                        -ERR_SPI if the SPI initialization failed
 */
int ata8520_init(ata8520_t *dev, const ata8520_params_t *params);

/**
 * @brief Perform a complete hardware reset of the ATA8520
 *
 * @param[in] dev                 Pointer to device descriptor
 */
void ata8520_system_reset(const ata8520_t *dev);

/**
 * @brief Return the Atmel version of the ATA8520
 *
 * @param[in] dev                 Pointer to device descriptor
 * @param[out] version            Pointer to the string containing the version.
 *
 * @return                        the atmel version
 */
void ata8520_atmel_version(const ata8520_t *dev, uint8_t *version);

/**
 * @brief Return the Atmel version of the ATA8520
 *
 * @param[in] dev                 Pointer to device descriptor
 * @param[out] version            Pointer to the string containing the version.
 *                                The version contains 11 characters.
 */
void ata8520_sigfox_version(const ata8520_t *dev, uint8_t *version);

/**
 * @brief Print the current status of the ATA8520
 *
 * Calling this function clears the system event line.
 *
 * @param[in] dev                 Pointer to device descriptor
 */
void ata8520_status(const ata8520_t *dev);

/**
 * @brief Send a message with ATA8520
 *
 * @param[in] dev                 Pointer to device descriptor
 * @param[in] msg                 The message to send
 */
void ata8520_send_msg(const ata8520_t *dev, uint8_t *msg);

/**
 * @brief Return the porting authorization code (PAC) available in the ATA8520.
 *
 * @param[in] dev                 Pointer to device descriptor
 * @param[out] pac                Pointer to the string containing the pac
 */
void ata8520_pac(const ata8520_t *dev, uint8_t *pac);

/**
 * @brief Return the ID of the ATA8520
 *
 * @param[in] dev                 Pointer to device descriptor
 * @param[out] id                 Pointer to the string containing the ID
 *
 * @return                        the status code
 */
void ata8520_id(const ata8520_t *dev, uint8_t *id);

/**
 * @brief Return internal temperature of the ATA8520
 *
 * @param[in] dev                 Pointer to device descriptor
 * @param[out] temperature        Temperature in d°C
 * @param[out] idle_voltage       Voltage when device is idle
 * @param[out] active_voltage     Voltage when device is active
 */
void ata8520_internal_measurements(const ata8520_t *dev,
                                   uint16_t *temperature,
                                   uint16_t *idle_voltage,
                                   uint16_t *active_voltage);


#ifdef __cplusplus
}
#endif

#endif /* ATA8520_H */
/** @} */
