/*
 * Copyright 2015 Ludwig Knüpfer,
 *           2015 Christian Mehlis
 *           2016-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_dht DHT Family of Humidity and Temperature Sensors
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the DHT Family of humidity
 *              and temperature sensors
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for the DHT family of humidity
 *              and temperature sensors
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef DCF77_H
#define DCF77_H

#include <stdint.h>

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

enum{
  POTENZY1 = 1,
  POTENZY2 = 2,
  POTENZY4 = 4,
  POTENZY8 = 8,
  POTENZY10 = 10,
  POTENZY20 = 20,
  POTENZY40 = 40,
  POTENZY80 = 80
};

/**
 * @brief   Possible return codes
 */
enum {
    DCF77_OK      =  0,       /**< all good */
    DCF77_NOCSUM  = -1,       /**< checksum error */
    DCF77_TIMEOUT = -2,
    DCF77_INIT_ERROR= -3,      /**< communication timed out */
};

/**
 * @brief   BIT Position in DCF77 structure
 */
enum {
    START             =  0,
    WHEATER           =  1,
    CALLING           = 15,
    MEZ_MESZ_SHIFT    = 16,
    MEZ               = 17,
    MESZ              = 18,
    SHIFT_SECOND      = 19,
    TIME_INFORMATION  = 20,
    MINUTE_SINGLE     = 21,
    MINUTE_SECOND     = 22,
    MINUTE_QUAD       = 23,
    MINUTE_EIGHT      = 24,
    MINUTE_TENNER     = 25,
    MINUTE_TEWENTIES  = 26,
    MINUTE_FOURTIES   = 27,
    MINUTE_PR         = 28,
    HOUR_SINGLE       = 29,
    HOUR_SECOND       = 30,
    HOUR_QUAD         = 31,
    HOUR_EIGHT        = 32,
    HOUR_TENNER       = 33,
    HOUR_TEWENTIES    = 34,
    HOUR_PR           = 35,
    DAY_SINGLE        = 36,
    DAY_SECOND        = 37,
    DAY_QUAD          = 38,
    DAY_EIGHT         = 39,
    DAY_TENNER        = 40,
    DAY_TEWENTIES     = 41,
    WEEKDAY_SINGLE    = 42,
    WEEKDAY_SECOND    = 43,
    WEEKDAY_QUAD      = 44,
    MONTH_SINGLE      = 45,
    MONTH_SECOND      = 46,
    MONTH_QUAD        = 47,
    MONTH_EIGHT       = 48,
    MONTH_TENNER      = 49,
    YEAR_SINGLE       = 50,
    YEAR_SECOND       = 51,
    YEAR_QUAD         = 52,
    YEAR_EIGHT        = 53,
    YEAR_TENNER       = 54,
    YEAR_TEWENTIES    = 55,
    YEAR_FOURTIES     = 56,
    YEAR_EIGHTIES     = 57,
    YEAR_PR           = 58
};


/**
 * @brief   Data type for storing DCF77 sensor readings
 */
typedef struct {
    uint8_t minute;       /**< minutes*/
    uint8_t hours;        /**< hours*/
    uint8_t day;          /**< days*/
    uint8_t month;        /**< month*/
    uint8_t year;         /**< years*/
    uint8_t weekday;      /**< weekday*/
    uint8_t mesz;         /**< Status*/
    uint8_t parity;       /**< parity*/
    uint16_t addons;      /**< Addons Wheater, emergency measures*/
} dcf77_data_t;



/**
 * @brief   Configuration parameters for DHT devices
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin of the device's data pin */
    gpio_mode_t in_mode;    /**< input pin configuration, with or without pull
                           *   resistor */

} dcf77_params_t;

/**
 * @brief   Device descriptor for DHT sensor devices
 */
typedef struct {
    dcf77_params_t params;    /**< Device parameters */
    dcf77_data_t last_val;    /**< Values of the last measurement */
    uint32_t last_read_us;  /**< Time of the last measurement */
} dcf77_t;

/**
 * @brief   Initialize a new DCF device
 *
 * @param[out] dev      device descriptor of a DCF device
 * @param[in]  params   configuration parameters
 *
 * @return              0 on success
 * @return              -1 on error
 */
int dcf77_init(dcf77_t *dev, const dcf77_params_t *params);

/**
 * @brief   get a new minutes, hours, weekday, calenderday,
 *          month and year from the device
 *
 * @note    if reading fails or checksum is invalid, no new values will be
 *          written into the result values
 *
 * @param[in]  dev      device descriptor of a DCF device
 * @param[out] time     timestruct
 *
 * @retval `DCF_OK`         Success
 * @retval `DCF_NOCSUM`     Checksum error
 * @retval `DCF_TIMEOUT`    Reading data timed out (check wires!)
 */
int dcf77_read(dcf77_t *dev, uint8_t *minute, uint8_t *hour, uint8_t *weekday,
  uint8_t *calenderday, uint8_t *month, uint8_t *year, uint8_t *mesz);



#ifdef __cplusplus
}
#endif

#endif /* DCF77_H */
/** @} */
