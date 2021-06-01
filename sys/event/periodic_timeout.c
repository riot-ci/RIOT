/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "kernel_defines.h"
#include "ztimer.h"
#include "ztimer/periodic.h"
#include "event/timeout.h"

static int _event_timeout_callback(void *arg)
{
    event_periodic_timeout_t *event_timeout = (event_periodic_timeout_t *)arg;
    event_post(event_timeout->queue, event_timeout->event);

    return 0;
}

void event_periodic_timeout_init(event_periodic_timeout_t *event_timeout,
                                 ztimer_clock_t *clock,
                                 event_queue_t *queue, event_t *event)
{
    ztimer_periodic_init(clock, &event_timeout->timer, _event_timeout_callback,
                         event_timeout, 0);
    event_timeout->queue = queue;
    event_timeout->event = event;
}
