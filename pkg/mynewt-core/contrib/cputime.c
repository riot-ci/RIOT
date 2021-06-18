/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       cputime implementaiton for non nrf5%*BOARDs
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "os/os_cputime.h"
#include "ztimer.h"
#include "hal/hal_timer.h"

uint32_t os_cputime_get32(void)
{
    return ztimer_now(ZTIMER_MSEC_BASE);
}

void os_cputime_delay_ticks(uint32_t ticks)
{
    ztimer_sleep(ZTIMER_MSEC_BASE, ticks);
}

void os_cputime_delay_usecs(uint32_t usecs)
{
    ztimer_sleep(ZTIMER_MSEC_BASE, os_cputime_usecs_to_ticks(usecs));
}

int os_cputime_init(uint32_t clock_freq)
{
    (void)clock_freq;
    return 0;
}

void os_cputime_timer_init(struct hal_timer *timer, hal_timer_cb fp,
                           void *arg)
{
    timer->timer.callback = fp;
    timer->timer.arg = arg;
}

int os_cputime_timer_start(struct hal_timer *timer, uint32_t cputime)
{
    uint32_t now = ztimer_now(ZTIMER_MSEC_BASE);

    if (now > cputime) {
        ztimer_set(ZTIMER_MSEC_BASE, &timer->timer, 0);
    }
    else {
        ztimer_set(ZTIMER_MSEC_BASE, &timer->timer, cputime - now);
    }
    return 0;
}

int os_cputime_timer_relative(struct hal_timer *timer, uint32_t usecs)
{
    ztimer_set(ZTIMER_MSEC_BASE, &timer->timer, usecs);
    return 0;
}

void os_cputime_timer_stop(struct hal_timer *timer)
{
    ztimer_remove(ZTIMER_MSEC_BASE, &timer->timer);
}
