/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_srf04
 * @{
 *
 * @file
 * @brief       driver for srf04 ultra sonic range finder
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 *
 * @}
 */

#include "srf04.h"
#include "srf04_params.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"


/*
 * for inch define distance as "1480"
 */
#ifndef SRF04_DISTANCE
#define SRF04_DISTANCE  (584U)
#endif

static bool state;

static void _cb(void *arg)
{
    uint32_t t = xtimer_now_usec();

    srf04_t* dev = (srf04_t*)arg;
    if (dev->state == SRF04_IDLE) {
        dev->state = SRF04_MEASURING;
        dev->time = t;
    }
    else if (state == SRF04_MEASURING) {
        gpio_irq_disable(dev->p.echo);
        dev->state = SRF04_IDLE;
        dev->distance = (t - dev->time);
    }
}

int srf04_init(srf04_t* dev)
{
    dev->p = srf04_params[0];

    dev->state = SRF04_IDLE;
    dev->distance = 0;
    dev->time = 0;

    if (gpio_init(dev->p.trigger, GPIO_OUT) != 0) {
        DEBUG("[srf04] Error: could not initialize GPIO trigger pin\n");
        return SRF04_ERR_GPIO;
    }

    if (gpio_init_int(dev->p.echo, GPIO_IN, GPIO_BOTH, _cb, (void*)dev) != 0) {
        DEBUG("[srf04] Error: could not initialize GPIO echo pin\n");
        return SRF04_ERR_INT;
    }

    gpio_irq_disable(dev->p.echo);

    return SRF04_OK;
}

void srf04_trigger(const srf04_t* dev)
{
    gpio_irq_enable(dev->p.echo);

    gpio_set(dev->p.trigger);
    xtimer_usleep(10);
    gpio_clear(dev->p.trigger);
}

int srf04_read(const srf04_t* dev)
{
    return dev->distance;
}

int srf04_read_distance(const srf04_t* dev)
{
    return ((dev->distance * 100) / SRF04_DISTANCE);
}
