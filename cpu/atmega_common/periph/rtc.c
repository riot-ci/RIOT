/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_rtc
 *
 * @{
 *
 * @file
 * @brief       RTC interface wrapper for use with RTT modules.
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * In order to safely sleep when using the RTT:
 * 1. Disable interrupts
 * 2. Write to one of the asynch registers (e.g. TCCR2A)
 * 3. Wait for ASSR register's busy flags to clear
 * 4. Re-enable interrupts
 * 5. Sleep before interrupt re-enable takes effect
 *
 * @}
 */

#include <avr/interrupt.h>
#include <time.h>

#include "cpu.h"
#include "periph/rtc.h"
#include "periph/rtt.h"
#include "periph_conf.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard file in case no RTC device is defined */
#if RTC_NUMOF

#ifdef __cplusplus
extern "C" {
#endif

static inline void _asynch_wait(void);

typedef struct {
    time_t time;                /* seconds since the epoch */
    time_t alarm;               /* alarm_cb when time == alarm */
    rtc_alarm_cb_t alarm_cb;    /* callback called from RTC alarm */
    void *alarm_arg;            /* argument passed to the callback */
} rtc_state_t;

static volatile rtc_state_t rtc_state;

void rtc_init(void)
{
    /* RTC depends on RTT */
    rtt_init();
}

int rtc_set_time(struct tm *time)
{
    uint8_t offset;

    /* Make sure it is safe to read TCNT2, in case we just woke up */
    DEBUG("RTT sleeps until safe to read TCNT2\n");
    TCCR2A = 0;
    _asynch_wait();

    offset = TCNT2;

    /* Convert to seconds (highest 3 bits) */
    offset = (offset & 0xE0) >> 5;

    /* Convert to seconds since the epoch */
    rtc_state.time = mk_gmtime(time) - offset;

    DEBUG("RTC set time: %" PRIu32 " seconds\n", rtc_state.time);

    return 0;
}

int rtc_get_time(struct tm *time)
{
    time_t time_secs;

    /* Make sure it is safe to read TCNT2, in case we just woke up */
    DEBUG("RTT sleeps until safe to read TCNT2\n");
    TCCR2A = 0;
    _asynch_wait();

    time_secs = (time_t)TCNT2;

    /* Convert to seconds (highest 3 bits of TCNT2) */
    time_secs = (time_secs & 0x000000E0) >> 5;

    time_secs += rtc_state.time;

    /* Convert from seconds since the epoch */
    gmtime_r(&time_secs, time);

    DEBUG("RTC get time: %" PRIu32 " seconds\n", time_secs);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2B);
    rtc_state.alarm_cb = NULL;

    /* Wait until not busy anymore (should be immediate) */
    DEBUG("RTC sleeps until safe to write OCR2B\n");
    _asynch_wait();

    /* Set alarm time */
    rtc_state.alarm = mk_gmtime(time);

    /* Prepare the counter for sub 8-second precision */
    OCR2B = ((uint8_t)rtc_state.alarm & 0x07) << 5;

    DEBUG("RTC set alarm: %" PRIu32 " seconds, OCR2B: %" PRIu8 "\n",
          rtc_state.alarm, OCR2B);

    /* Interrupt safe order of assignment */
    rtc_state.alarm_arg = arg;
    rtc_state.alarm_cb = cb;

    /* Enable irq only if alarm is in the 8s period before it overflows */
    if ((rtc_state.alarm & 0xFFFFFFF8) <= (rtc_state.time & 0xFFFFFFF8)) {
        if (rtc_state.alarm <= rtc_state.time) {
            /* Prevent alarm offset if time is too soon */
            rtc_state.alarm_cb(rtc_state.alarm_arg);
        }
        else {
            /* Clear interrupt flag */
            TIFR2 = (1 << OCF2B);

            TIMSK2 |= (1 << OCIE2B);
        }
    }

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    /* Convert from seconds since the epoch */
    /* Note: assignment is to discard volatile */
    time_t alarm = rtc_state.alarm;
    gmtime_r(&alarm, time);

    DEBUG("RTC get alarm: %" PRIu32 " seconds\n", alarm);

    return 0;
}

void rtc_clear_alarm(void)
{
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2B);

    /* Clear interrupt flag */
    TIFR2 = (1 << OCF2B);

    /* Interrupt safe order of assignment */
    rtc_state.alarm_cb = NULL;
    rtc_state.alarm_arg = NULL;
}

void rtc_poweron(void)
{
    rtt_poweron();
}

void rtc_poweroff(void)
{
    rtt_poweroff();
}

void atmega_rtc_incr(void)
{
    rtc_state.time += 8;

    /* Enable irq only if alarm is in the 8s period before it overflows */
    if ((rtc_state.alarm & 0xFFFFFFF8) == (rtc_state.time & 0xFFFFFFF8)) {
        if (rtc_state.alarm <= rtc_state.time) {
            /* Prevent alarm offset if time is too soon */
            rtc_state.alarm_cb(rtc_state.alarm_arg);
        }
        else {
            /* Clear interrupt flag */
            TIFR2 = (1 << OCF2B);

            TIMSK2 |= (1 << OCIE2B);
        }
    }
}

void _asynch_wait(void)
{
    /* Wait until all busy flags clear. According to the datasheet,
     * this can take up to 2 positive edges of TOSC1 (32kHz). */
    while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB)
                   | (1 << TCR2AUB) | (1 << TCR2BUB))) ;
}

ISR(TIMER2_COMPB_vect) {
    __enter_isr();
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2B);

    if (rtc_state.alarm_cb != NULL) {
        rtc_state.alarm_cb(rtc_state.alarm_arg);
    }
    __exit_isr();
}

#ifdef __cplusplus
}
#endif

#endif /* RTC_NUMOF */
