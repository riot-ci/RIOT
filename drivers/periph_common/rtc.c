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

#include <stdint.h>
#include <stdlib.h>
#include "periph/rtc.h"

#ifndef RTC_NORMALIZE_COMPAT
#define RTC_NORMALIZE_COMPAT (1)
#endif

static int _is_leap_year(int year)
{
    if (year & 0x3) {
        return 0;
    }

    return !!(year % 25) || !(year & 15);
}

static int _month_length(int month, int year)
{
    if (month == 1) {
        return 28 + _is_leap_year(year);
    }

    return 31 - ((month % 7) & 1);
}

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
    static const uint8_t d[] = { 0,  31,  59, 90, 120, 151,
                               181, 212, 243, 17,  48,  78};

    if (month > 1) {
        day += _is_leap_year(year);
    }

    /* at this point we can be sure that day <= 31 */
    if (month > 8) {
        day |= 0x100;
    }

    return d[month] + day - 1;
}
#endif /* RTC_NORMALIZE_COMPAT */

void rtc_tm_normalize(struct tm *t)
{
    int days;
    div_t d;

    d = div(t->tm_sec, 60);
    t->tm_min += d.quot;
    t->tm_sec  = d.rem;

    d = div(t->tm_min, 60);
    t->tm_hour += d.quot;
    t->tm_min   = d.rem;

    d = div(t->tm_hour, 24);
    t->tm_mday += d.quot;
    t->tm_hour  = d.rem;

    d = div(t->tm_mon, 12);
    t->tm_year += d.quot;
    t->tm_mon   = d.rem;

    while (1) {
        days = _month_length(t->tm_mon, t->tm_year + 1900);

        if (t->tm_mday <= days) {
            break;
        }

        if (++t->tm_mon > 11) {
            t->tm_mon = 0;
            ++t->tm_year;
        }

        t->tm_mday -= days;
    }

#if RTC_NORMALIZE_COMPAT
    t->tm_yday = _yday(t->tm_mday, t->tm_mon, t->tm_year + 1900);
    t->tm_wday = _wday(t->tm_mday, t->tm_mon, t->tm_year + 1900);
#endif
}
