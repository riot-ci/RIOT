/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer_rtt
 * @{
 *
 * @file
 * @brief       ztimer periph/rtt implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include "periph/rtt.h"
#include "ztimer/rtt.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef RTT_MIN_VALUE
#define RTT_MIN_VALUE (2U)
#endif

static void _ztimer_rtt_callback(void *arg)
{
    ztimer_handler((ztimer_clock_t *)arg);
}

static void _ztimer_rtt_set(ztimer_clock_t *ztimer, uint32_t val)
{
    unsigned state = irq_disable();

    if (val < RTT_MIN_VALUE) {
        /* the rtt might advance right between the call to rtt_get_counter()
         * and rtt_set_alarm(). If that happens with val==1, we'd set an alarm
         * to the current time, which would then underflow.  To avoid this, we
         * set the alarm at least two ticks in the future.  TODO: confirm this
         * is sufficient, or conceive logic to lower this value.
         *
         * @note RTT_MIN_VALUE defaults to 2, but some platforms might have
         * different values.
         */
        val = RTT_MIN_VALUE;
    }

    rtt_set_alarm(rtt_get_counter() + val, _ztimer_rtt_callback, ztimer);

    irq_restore(state);
}

static uint32_t _ztimer_rtt_now(ztimer_clock_t *ztimer)
{
    (void)ztimer;
    return rtt_get_counter();
}

static void _ztimer_rtt_cancel(ztimer_clock_t *ztimer)
{
    (void)ztimer;
    rtt_clear_alarm();
}

static const ztimer_ops_t _ztimer_rtt_ops = {
    .set = _ztimer_rtt_set,
    .now = _ztimer_rtt_now,
    .cancel = _ztimer_rtt_cancel,
};

void ztimer_rtt_init(ztimer_rtt_t *ztimer)
{
    ztimer->ops = &_ztimer_rtt_ops;
    ztimer->max_value = RTT_MAX_VALUE;
    rtt_init();
    rtt_poweron();
}
