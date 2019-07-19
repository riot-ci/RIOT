/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer_periph
 * @{
 *
 * @file
 * @brief       ztimer periph/timer backend implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include "ztimer/periph.h"

static void _ztimer_periph_set(ztimer_dev_t *ztimer, uint32_t val)
{
    ztimer_periph_t *ztimer_periph = (ztimer_periph_t*) ztimer;

    unsigned adjust = ztimer_periph->adjust;
    if (val > adjust) {
        val -= adjust;
    }

    /* TODO: ensure this is done atomically */
    timer_set(ztimer_periph->dev, 0, val);
}

static uint32_t _ztimer_periph_now(ztimer_dev_t *ztimer)
{
    ztimer_periph_t *ztimer_periph = (ztimer_periph_t*) ztimer;
    return timer_read(ztimer_periph->dev);
}

static void _ztimer_periph_cancel(ztimer_dev_t *ztimer)
{
    ztimer_periph_t *ztimer_periph = (ztimer_periph_t*) ztimer;
    timer_clear(ztimer_periph->dev, 0);
}

static void _ztimer_periph_callback(void *arg, int channel)
{
    (void)channel;
    ztimer_handler((ztimer_dev_t*) arg);
}

static const ztimer_ops_t _ztimer_periph_ops = {
    .set=_ztimer_periph_set,
    .now=_ztimer_periph_now,
    .cancel=_ztimer_periph_cancel,
};

void ztimer_periph_init(ztimer_periph_t *ztimer, tim_t dev, unsigned long freq)
{
    ztimer->dev = dev;
    ztimer->super.ops = &_ztimer_periph_ops;
    timer_init(dev, freq, _ztimer_periph_callback, ztimer);
}
