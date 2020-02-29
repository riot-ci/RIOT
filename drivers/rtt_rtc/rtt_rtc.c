/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph_rtc
 * @{
 *
 * @file
 * @brief       Basic RTC implementation based on a RTT
 *
 * @note        Unlike a real RTC, this emulated version is not guaranteed to keep
 *              time across reboots or deep sleep.
 *
 *              The maximum alarm offset to the current time is
 *              `RTT_MAX_VALUE`/`RTT_FREQUENCY` seconds.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <string.h>

#include "periph/rtc.h"
#include "periph/rtt.h"

#define RTT_SECOND (RTT_FREQUENCY)
#define RTT_MINUTE (RTT_SECOND * 60)
#define RTT_HOUR   (RTT_MINUTE * 60)
#define RTT_DAY    (RTT_HOUR   * 24)

/* In .noinit so we don't reset the counter on reboot */
static struct tm tm_now __attribute__((section(".noinit")));

/* get the time it takes the RTT to overflow */
static inline unsigned char _rtt_get_overflow(unsigned part)
{
    switch (part) {
    case RTT_SECOND:
    case RTT_MINUTE:
        return (RTT_MAX_VALUE/part) % 60;
    case RTT_HOUR:
        return (RTT_MAX_VALUE/part) % 24;
    default:
        return (RTT_MAX_VALUE/part);
    }
}

#define RTT_SEC_MAX  _rtt_get_overflow(RTT_SECOND)
#define RTT_MIN_MAX  _rtt_get_overflow(RTT_MINUTE)
#define RTT_HOUR_MAX _rtt_get_overflow(RTT_HOUR)
#define RTT_DAY_MAX  _rtt_get_overflow(RTT_DAY)

static void _rtt_overflow(void *arg) {
    (void) arg;

    tm_now.tm_sec  += RTT_SEC_MAX;
    tm_now.tm_min  += RTT_MIN_MAX;
    tm_now.tm_hour += RTT_HOUR_MAX;
    tm_now.tm_yday += RTT_DAY_MAX;
}

void rtc_init(void)
{
    if (!rtc_tm_valid(&tm_now)) {
        memset(&tm_now, 0, sizeof(tm_now));
    }

    rtt_set_overflow_cb(_rtt_overflow, NULL);
}

int rtc_set_time(struct tm *time)
{
    rtc_tm_normalize(time);

    rtt_set_counter(0);
    tm_now = *time;

    return 0;
}

int rtc_get_time(struct tm *time)
{
    *time = tm_now;
    time->tm_sec += rtt_get_counter()/RTT_SECOND;
    rtc_tm_normalize(time);

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    *time = tm_now;
    time->tm_sec += rtt_get_alarm()/RTT_SECOND;
    rtc_tm_normalize(time);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    rtc_tm_normalize(time);

    /* reset the RTT counter to get maximum range */
    tm_now.tm_sec += rtt_get_counter()/RTT_SECOND;
    rtc_tm_normalize(&tm_now);
    rtt_set_counter(0);

    uint32_t now   = rtc_mktime(&tm_now);
    uint32_t alarm = rtc_mktime(time);
    uint32_t diff  = alarm - now;

    if (now > alarm) {
        return -1;
    }

    if (diff > RTT_MAX_VALUE/RTT_SECOND) {
        return -1;
    }

    rtt_set_alarm(diff * RTT_SECOND, cb, arg);

    return 0;
}

void rtc_clear_alarm(void)
{
    rtt_clear_alarm();
}

void rtc_poweron(void)
{
    rtt_poweron();
}

void rtc_poweroff(void)
{
    rtt_poweroff();
}
