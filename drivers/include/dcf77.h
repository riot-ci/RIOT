/*
 * Copyright 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_dcf77 DCF77 long wave receiver with 77,5 kHz
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver long wave receiver with 77,5 kHz
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the dcf77 sensor driver
 *
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 */

#ifndef DCF77_H
#define DCF77_H

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "xtimer.h"
#include "time.h"
#include "periph/gpio.h"
#include "dcf77_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Possible return codes
 */
enum {
    DCF77_OK          =  0,       /**< all good */
    DCF77_NOCSUM      = -1,       /**< checksum error */
    DCF77_TIMEOUT     = -2,       /**< communication timed out */
    DCF77_INIT_ERROR  = -3        /**< Initialization error */
};

/**
 * @brief   Configuration parameters for DCF77 devices
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin of the device's data pin */
    gpio_mode_t in_mode;    /**< input pin configuration from the device,
                             * without pull resistor */
} dcf77_params_t;


/**
 * @brief   Device descriptor for DCF77 sensor devices
 */
typedef struct {
    dcf77_params_t params;      /**< Device parameters */
    dcf77_bits_t bitseq;        /**< contains all Bits from a current cycle */
    dcf77_bits_t last_bitseq;   /**< contains all Bits from a last cycle */
    uint8_t internal_state;     /**< internal States  */
    uint32_t startTime;         /**< Timestamp to measure the term of the level */
    uint8_t bitCounter;         /**< Counter of the Bits in a Bitsequenz */

} dcf77_t;

/**
 * @brief   Initialize a new DCF77 device
 *
 * @param[out] dev      device descriptor of a DCF device
 * @param[in]  params   configuration parameters
 *
 * @retval `DCF77_OK`           Success
 * @retval `DCF77_INIT_ERROR`   Error in initialization
 */
int dcf77_init(dcf77_t *dev, const dcf77_params_t *params);

/**
 * @brief   get a new timestamp from the device.
 *
 * @note    if reading fails or checksum is invalid, last_vaules will be unwritten
 *
 * @param[in]  dev          device descriptor of a DCF device
 * @param[in]  time         datastruct for timeinformation
 *
 * @retval `DCF77_OK`         Success
 * @retval `DCF77_NOCSUM`     Checksum error
 */
int dcf77_get_time(dcf77_t *dev, struct tm *time);

#ifdef __cplusplus
}
#endif

#endif /* DCF77_H */
/** @} */
