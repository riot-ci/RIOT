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
 * @brief       mynewt-core callout abstraction
 *
 * Callout sets a timer that on expiration will post an event to an
 * event queue. This mimics the same as MyNewt callout api.
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef OS_CALLOUT_H
#define OS_CALLOUT_H

#include "ztimer.h"

#include "os/os_types.h"
#include "os/os_eventq.h"
#include "os/os_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   callout structure
 */
struct os_callout {
    ztimer_t timer;         /**< timer */
    struct os_event c_e;   /**< callout event */
    struct os_eventq *c_q; /**< callout event queue */
};

/**
 * @brief   Initialize a callout.
 *
 * Callouts are used to schedule events in the future onto an event
 * queue. Callout timers are scheduled using the os_callout_reset()
 * function.  When the timer expires, an event is posted to the event
 * queue specified in os_callout_init(). The event argument given here
 * is posted in the ev_arg field of that event.
 *
 * @param[out]  c       callout to initialize
 * @param[in]   q       event queue to queue event in
 * @param[in]   e_cb    callback function
 * @param[in]   e_arg   callback function argument
 */
void os_callout_init(struct os_callout *c, struct os_eventq *q,
                      os_event_fn *e_cb, void *e_arg);

/**
 * @brief   Reset the callout to fire off in 'ticks' ticks.
 *
 * @param[in]   c       callout to reset
 * @param[in]   ticks   number of ticks to wait before posting an event
 *
 * @return 0 on success, non-zero on failure
 */
os_error_t os_callout_reset(struct os_callout *c, os_time_t ticks);

/**
 * @brief   Stops the callout from firing.
 *
 * @param[in]   c   the callout to stop
 */
void os_callout_stop(struct os_callout *c);

#ifdef __cplusplus
}
#endif

#endif /* OS_CALLOUT_H */
