/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
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
 * @brief       common RTC function fallback implementations
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "periph/rtc.h"

#ifndef RTC_NORMALIZE_COMPAT
#define RTC_NORMALIZE_COMPAT (0)
#endif

/* avr_time provides these functions already. */
#ifndef ATMEGA_INCOMPATIBLE_TIME_H
static int is_leap_year(int year)
{
    if (year & 0x3) {
        return 0;
    }

    return !!(year % 25) || !(year & 15);
}

static int month_length(int month, int year)
{
    if (month == 2) {
        return 28 + is_leap_year(year);
    }

    return 31 - (month - 1) % 7 % 2;
}
#endif /* ATMEGA_INCOMPATIBLE_TIME_H */

#if RTC_NORMALIZE_COMPAT
static int _wday(int day, int month, int year)
{
    /* Tomohiko Sakamoto's Algorithm */
    static const uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 2;
    return (year + year/4 - year/100 + year/400 + t[month] + day) % 7;
}

static int _yday(int day, int month, int year)
{
    static const uint16_t d[] = {31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    if (month == 0) {
        return day;
    }

    if (month > 1) {
        day += is_leap_year(year);
    }

    return d[month - 1] + day;
}
#endif /* RTC_NORMALIZE_COMPAT */

void rtc_tm_normalize(struct tm *t)
{
    int days;

    t->tm_min += t->tm_sec / 60;
    t->tm_sec %= 60;

    t->tm_hour += t->tm_min / 60;
    t->tm_min  %= 60;

    t->tm_mday += t->tm_hour / 24;
    t->tm_hour %= 24;

    t->tm_year += t->tm_mon / 12;
    t->tm_mon  %= 12;

    days = month_length(t->tm_mon + 1, t->tm_year + 1900);

    while (t->tm_mday > days) {
        if (++t->tm_mon > 11) {
            t->tm_mon = 0;
            ++t->tm_year;
        }

        t->tm_mday -= days;
        days = month_length(t->tm_mon + 1, t->tm_year + 1900);
    }

#if RTC_NORMALIZE_COMPAT
    t->tm_yday = _yday(t->tm_mday, t->tm_mon, t->tm_year + 1900) - 1;
    t->tm_wday = _wday(t->tm_mday, t->tm_mon, t->tm_year + 1900);
#endif
}
