/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Shared peripheral timer code
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include "periph/timer.h"

#ifndef PERIPH_TIMER_PROVIDES_SET
int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    return timer_set_absolute(dev, channel, timer_read(dev) + timeout);
}
#endif

unsigned int timer_diff(tim_t tim, unsigned int begin, unsigned int until)
{
    unsigned int diff = 0;
    if (tim < TIMER_NUMOF) {
        diff = (until - begin) & timer_max(tim);
    }
    return diff;
}
