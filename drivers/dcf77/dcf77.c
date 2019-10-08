/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     drivers_dcf77
 * @{
 *
 * @file
 * @brief       Device driver implementation for the dcf 77
 *              longwave time signal and standard-frequency radio station
 *
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "log.h"
#include "assert.h"
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"

#include "dcf77.h"
#include "dcf77_internal.h"
#include "dcf77_params.h"


#define ENABLE_DEBUG    (0)
#include "debug.h"


/* If a pulse lasts longer than 200ms, a new cycle begins  */
#define DCF77_PULSE_START_HIGH_THRESHOLD        (1500000U)  /*~1000ms*/
/* Every pulse send by the DCF longer than 100ms is interpreted as 1 */
#define DCF77_PULSE_WIDTH_THRESHOLD             (140000U)   /*~150ms*/
/* If an expected pulse is not detected within 1,5s, something is wrong */
#define DCF77_TIMEOUT                           (2500000)   /*~0200ms*/
/* The DCF sensor cannot measure more than once a second */
#define DCF77_DATA_HOLD_TIME                    (US_PER_SEC)

#define DCF77_READING_CYCLE                     (59)


/**
 * @brief   Calculate a bitsequenz to decimal
 *
 * @param   b3     MSB
 * @param   b2     third Bit
 * @param   b1     second Bit
 * @param   b0     LSB
 *
 * @return  result             Total
 */

static uint8_t bcd2dez(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
{
    uint8_t result = b0 + b1 * POTENZY2 + b2 * POTENZY4 + b3 * POTENZY8;

    return result;
}


/**
 * @brief   Check the parity of a Bitsequenz
 *
 * @param   dev       Device
 * @param   start     MSB
 * @param   end       third Bit
 *
 * @return  bool      result
 */

static uint8_t checkEvenParity(uint8_t start, uint8_t end, dcf77_t *dev)
{
    uint8_t ipar = 0;

    for (uint8_t i = start; i <= end; i++) {
        ipar += dev->bitseq[i];
    }
    return (ipar % 2);
}



static void _level_cb(void *arg)
{
    dcf77_t *dev = (dcf77_t *)arg;

    switch (dev->internal_state) {
        case DCF77_STATE_IDLE:
            if (gpio_read(dev->params.pin) == 0) {
                DEBUG("[dcf77] EVENT IDLE 0  !\n");
                dev->startTime = xtimer_now_usec();
                dev->internal_state = DCF77_STATE_START;
            }
            break;
        case DCF77_STATE_START:
            if (gpio_read(dev->params.pin) == 1) {
                DEBUG("[dcf77] EVENT START 1 !\n");
                dev->stopTime = xtimer_now_usec();
                if ((dev->stopTime - dev->startTime) >
                    DCF77_PULSE_START_HIGH_THRESHOLD) {
                    dev->internal_state = DCF77_STATE_RX;
                }
                else {
                    dev->internal_state = DCF77_STATE_IDLE;
                }
            }
            break;
        case DCF77_STATE_RX:
            if (gpio_read(dev->params.pin) == 1) {
                DEBUG("[dcf77] EVENT RX 1 !\n");
                dev->startTime = xtimer_now_usec();

            }
            else {
                DEBUG("[dcf77] EVENT RX 0 !\n");
                dev->stopTime = xtimer_now_usec();
                if ((dev->stopTime - dev->startTime) >
                    DCF77_PULSE_WIDTH_THRESHOLD) {
                    dev->bitseq[dev->bitCounter] = 1;
                }
                else {
                    dev->bitseq[dev->bitCounter] = 0;

                }

                dev->bitCounter++;
                if (dev->bitCounter >= DCF77_READING_CYCLE) {
                    mutex_unlock(&dev->event_lock);
                    dev->bitCounter = 0;
                    dev->startTime = xtimer_now_usec();
                    dev->internal_state = DCF77_STATE_START;
                }

            }
            break;

    }

}




/**
 * @brief   Initalize the Device
 *
 * @param   dev     device
 * @param   params  device_params
 *
 * @retval  DCF77_OK             Success
 * @retval  DCF77_INIT_ERROR     Error in initalization
 */

int dcf77_init(dcf77_t *dev, const dcf77_params_t *params)
{
    DEBUG("dcf77_init\n");

    /* check parameters and configuration */
    assert(dev && params);
    dev->params = *params;
    dev->internal_state = DCF77_STATE_IDLE;
    dev->bitCounter = 0;
    gpio_init_int(dev->params.pin, dev->params.in_mode, GPIO_BOTH, _level_cb,
                  dev);

    mutex_init(&dev->event_lock);
    return DCF77_OK;
}

/**
 * @brief   Formats the information after all bits of a cycle have been received
 *
 * @param   dev               device
 * @param   time              datastruct for timeinformation
 *
 * @retval  DCF77_OK          Success
 * @retval  DCF77_TIMEOUT     Timeout occurred before level was reached
 * @retval  DCF77_NOCSUM      Parity Bits arent correct
 */

int dcf77_read(dcf77_t *dev, struct tm *time)
{
    assert(dev);
    mutex_lock(&dev->event_lock);

    if ((dev->bitseq[DCF77_MESZ1] == 1) && (dev->bitseq[DCF77_MESZ2] == 0)) {
        time->tm_isdst = 1;
    }
    else {
        time->tm_isdst = 0;
    }

    uint8_t minute = bcd2dez(dev->bitseq[DCF77_MINUTE_EIGHT],
                             dev->bitseq[DCF77_MINUTE_QUAD],
                             dev->bitseq[DCF77_MINUTE_SECOND],
                             dev->bitseq[DCF77_MINUTE_SINGLE]);
    minute = minute + POTENZY10 * bcd2dez(0, dev->bitseq[DCF77_MINUTE_FOURTIES],
                                          dev->bitseq[DCF77_MINUTE_TEWENTIES],
                                          dev->bitseq[DCF77_MINUTE_TENNER]);

    if (checkEvenParity(DCF77_MINUTE_SINGLE, DCF77_MINUTE_FOURTIES,
                        dev) != dev->bitseq[DCF77_MINUTE_PR]) {
        return DCF77_NOCSUM;
    }

    time->tm_min = minute;

    uint8_t hour = bcd2dez(dev->bitseq[DCF77_HOUR_EIGHT], dev->bitseq[DCF77_HOUR_QUAD],
                           dev->bitseq[DCF77_HOUR_SECOND], dev->bitseq[DCF77_HOUR_SINGLE]);
    hour = hour + POTENZY10 * bcd2dez(0, 0, dev->bitseq[DCF77_HOUR_TEWENTIES],
                                      dev->bitseq[DCF77_HOUR_TENNER]);

    if (checkEvenParity(DCF77_HOUR_SINGLE, DCF77_HOUR_TEWENTIES,
                        dev) != dev->bitseq[DCF77_HOUR_PR]) {
        return DCF77_NOCSUM;
    }

    time->tm_hour = hour;

    uint8_t mday = bcd2dez(dev->bitseq[DCF77_DAY_EIGHT], dev->bitseq[DCF77_DAY_QUAD],
                           dev->bitseq[DCF77_DAY_SECOND], dev->bitseq[DCF77_DAY_SINGLE]);
    mday = mday + POTENZY10 * bcd2dez(0, 0, dev->bitseq[DCF77_DAY_TEWENTIES],
                                      dev->bitseq[DCF77_DAY_TENNER]);


    uint8_t month = bcd2dez(dev->bitseq[DCF77_MONTH_EIGHT], dev->bitseq[DCF77_MONTH_QUAD],
                            dev->bitseq[DCF77_MONTH_SECOND],
                            dev->bitseq[DCF77_MONTH_SINGLE]);
    month = month + POTENZY10 * bcd2dez(0, 0, 0, dev->bitseq[DCF77_MONTH_TENNER]);


    uint8_t year = bcd2dez(dev->bitseq[DCF77_YEAR_EIGHT], dev->bitseq[DCF77_YEAR_QUAD],
                           dev->bitseq[DCF77_YEAR_SECOND], dev->bitseq[DCF77_YEAR_SINGLE]);
    year = year + POTENZY10 *
           bcd2dez(dev->bitseq[DCF77_YEAR_EIGHTIES], dev->bitseq[DCF77_YEAR_FOURTIES],
                   dev->bitseq[DCF77_YEAR_TEWENTIES], dev->bitseq[DCF77_YEAR_TENNER]);

    if (checkEvenParity(DCF77_DAY_SINGLE, DCF77_YEAR_EIGHTIES,
                        dev) != dev->bitseq[DCF77_DATE_PR]) {
        return DCF77_NOCSUM;
    }

    time->tm_mday = mday;
    time->tm_mon = month;
    time->tm_year = year;

    return DCF77_OK;
}
