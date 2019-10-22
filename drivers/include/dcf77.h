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
#include "periph/gpio.h"
#include "time.h"



#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   Possible return codes
 */
enum {
    DCF77_OK        =  0,       /**< all good */
    DCF77_NOCSUM    = -1,       /**< checksum error */
    DCF77_TIMEOUT   = -2,
    DCF77_INIT_ERROR= -3,       /**< communication timed out */
};

/**
 * @brief   Configuration parameters for DCF77 devices
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin of the device's data pin */
    gpio_mode_t in_mode;    /**< input pin configuration, with or without pull
                             *   resistor */
} dcf77_params_t;

typedef union {
    struct values {
        uint64_t start : 1,
                 wheater : 14,
                 calling : 1,
                 mez_mesz_shift : 1,
                 mesz : 2,
                 shift_sec : 1,
                 start_time : 1,
                 minute_l : 4,
                 minute_h : 3,
                 minute_par : 1,
                 hour_l : 4,
                 hour_h : 2,
                 hour_par : 1,
                 day_l : 4,
                 day_h : 2,
                 wday : 3,
                 month_l : 4,
                 month_h : 1,
                 year_l : 4,
                 year_h : 4,
                 date_par : 1,
                 buff : 5;
    } val;
    uint64_t bits;
} dcf77_bits_t;

/**
 * @brief   Device descriptor for DCF77 sensor devices
 */
typedef struct {
    dcf77_params_t params;      /**< Device parameters */
    struct tm last_values;      /**< Values of the last measurement */
    mutex_t event_lock;         /**< mutex for waiting for event */
    uint8_t internal_state;     /**< internal States  */
    uint32_t startTime;         /**< Timestamp tomeasure the term of the level */
    uint32_t stopTime;          /**< Timestamp tomeasure the term of the level */
    uint8_t bitCounter;         /**< Counter of the Bits in a Bitsequenz */
    dcf77_bits_t bitseq;        /**< contains all Bits from a cycle */
} dcf77_t;

/**
 * @brief   Initialize a new DCF77 device
 *
 * @param[out] dev      device descriptor of a DCF device
 * @param[in]  params   configuration parameters
 *
 * @retval `DCF77_OK`         Success
 */
int dcf77_init(dcf77_t *dev, const dcf77_params_t *params);

/**
 * @brief   get a new minutes, hours, weekday, calenderday,
 *          month and year from the device
 *
 * @note    if reading fails or checksum is invalid, no new values will be
 *          written into the result values
 *
 * @param[in]  dev          device descriptor of a DCF device
 * @param[in]  time         datastruct for timeinformation
 *
 * @retval `DCF77_OK`         Success
 * @retval `DCF77_NOCSUM`     Checksum error
 * @retval `DCF77_TIMEOUT`    Reading data timed out (check wires!)
 */
int dcf77_read(dcf77_t *dev, struct tm *time);

#ifdef __cplusplus
}
#endif

#endif /* DCF77_H */
/** @} */
