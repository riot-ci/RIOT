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


/* Persistent level longer than 1000ms starts a new cycle  */
#define DCF77_PULSE_START_HIGH_THRESHOLD        (1500000U)  /*~1000ms*/
/* Every pulse send by the DCF longer than 130ms is interpreted as 1 */
#define DCF77_PULSE_WIDTH_THRESHOLD             (130000U)   /*~130ms*/
/* If an expected pulse is not detected within 2,5s, something is wrong */
#define DCF77_TIMEOUT                           (2500000U)  /*~2500ms*/

#define DCF77_READING_CYCLE                     (59)

#define DCF77_MINUTE_MASK                       (0xFE00000ULL)
#define DCF77_HOUR_MASK                         (0x7E0000000ULL)
#define DCF77_DATE_MASK                         (0x1FFFFFC00000000ULL)

#define DCF77_MINUTE_SHIFT                      (21)
#define DCF77_HOUR_SHIFT                        (29)
#define DCF77_DATE_SHIFT                        (36)


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
                    memset(&dev->bitseq.bits, 0, sizeof dev->bitseq.bits);
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
                    dev->bitseq.bits |=  1ULL << dev->bitCounter;
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
    memset(&dev->last_values, 0, sizeof dev->last_values);
    memset(&dev->bitseq.bits, 0, sizeof dev->bitseq.bits);
    //dev->bitseq = malloc(sizeof(dev->bitseq)) ;
    gpio_init_int(dev->params.pin, dev->params.in_mode, GPIO_BOTH, _level_cb,
                  dev);

    mutex_init(&dev->event_lock);
    return DCF77_OK;
}

/**
 * @brief   Formats the information after all bits of a cycle https://www.google.com/search?q=daxhave been received
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

    if (dev->bitseq.val.mesz == 2) {
        time->tm_isdst = 1;
    }
    else {
        time->tm_isdst = 0;
    }

    uint8_t minute = POTENZY10 * dev->bitseq.val.minute_h +
                     dev->bitseq.val.minute_l;
    if (__builtin_parity((dev->bitseq.bits >> DCF77_MINUTE_SHIFT) &
                         (DCF77_MINUTE_MASK >> DCF77_MINUTE_SHIFT)) !=
        dev->bitseq.val.minute_par) {
        return DCF77_NOCSUM;
    }

    uint8_t hour = POTENZY10 * dev->bitseq.val.hour_h + dev->bitseq.val.hour_l;
    if (__builtin_parity((dev->bitseq.bits >> DCF77_HOUR_SHIFT) &
                         (DCF77_HOUR_MASK >> DCF77_HOUR_SHIFT)) !=
        dev->bitseq.val.hour_par) {
        return DCF77_NOCSUM;
    }

    uint8_t mday = POTENZY10 * dev->bitseq.val.day_h + dev->bitseq.val.day_l;

    uint8_t wday =  dev->bitseq.val.wday;

    uint8_t month = POTENZY10 * dev->bitseq.val.month_h +
                    dev->bitseq.val.month_l;

    uint8_t year = POTENZY10 * dev->bitseq.val.year_h + dev->bitseq.val.year_l;
    if (__builtin_parity((dev->bitseq.bits >> DCF77_DATE_SHIFT) &
                         (DCF77_DATE_MASK >> DCF77_DATE_SHIFT)) !=
        dev->bitseq.val.date_par) {
        return DCF77_NOCSUM;
    }
    /**Add aditional check if the parity is fortuitously correct*/
    if (minute <= dev->last_values.tm_min) {
        return DCF77_NOCSUM;
    }

    time->tm_min = minute;
    time->tm_hour = hour;
    time->tm_mday = mday;
    time->tm_wday = wday;
    time->tm_mon = month;
    time->tm_year = 100 + year;

    DEBUG("LAST MINUTE: %d\n", dev->last_values.tm_min);

    DEBUG("MESZ: %d\n", dev->bitseq.val.mesz);
    DEBUG("MIN L: %d\n", dev->bitseq.val.minute_l);
    DEBUG("MIN H: %d\n", dev->bitseq.val.minute_h);
    DEBUG("MIN PAR: %d\n", dev->bitseq.val.minute_par);

    DEBUG("HOUR L: %d\n", dev->bitseq.val.hour_l);
    DEBUG("HOUR H: %d\n", dev->bitseq.val.hour_h);
    DEBUG("HOUR PAR: %d\n", dev->bitseq.val.hour_par);

    DEBUG("DAY L: %d\n", dev->bitseq.val.day_l);
    DEBUG("DAY H: %d\n", dev->bitseq.val.day_h);

    DEBUG("WDAY  : %d\n", dev->bitseq.val.wday);

    DEBUG("MONTH L: %d\n", dev->bitseq.val.month_l);
    DEBUG("MONTH H: %d\n", dev->bitseq.val.month_h);

    DEBUG("YEAR L: %d\n", dev->bitseq.val.year_l);
    DEBUG("YEAR H: %d\n", dev->bitseq.val.year_h);
    DEBUG("DATE PAR: %d\n", dev->bitseq.val.date_par);

    dev->last_values = *time;

    return DCF77_OK;
}
