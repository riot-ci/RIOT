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

static bool state;

static void _cb(void *arg)
{
    uint32_t t = xtimer_now_usec();

    srf04_t* dev = (srf04_t*)arg;
    if (dev->p.state == SRF04_IDLE) {
        dev->p.state = SRF04_MEASURING;
        dev->p.time = t;
    }
    else if (state == SRF04_MEASURING) {
        gpio_irq_disable(dev->p.echo);
        dev->p.state = SRF04_IDLE;
        dev->p.distance = (t - dev->p.time);
    }
}

int srf04_init(srf04_t* dev, const gpio_t trigger, const gpio_t echo)
{
    dev->p.state = SRF04_IDLE;
    dev->p.trigger = trigger;
    dev->p.echo = echo;
    dev->p.distance = 0;
    dev->p.time = 0;

    if ((gpio_init(dev->p.trigger, GPIO_OUT) != 0) |
        (gpio_init(dev->p.echo, GPIO_IN) != 0)){
        DEBUG("[srf04] Error: could not initialize GPIO\n");
        return SRF04_GPIO;
    }

    if (gpio_init_int(dev->p.echo, GPIO_IN, GPIO_BOTH, _cb, (void*)dev) != 0) {
        DEBUG("[srf04] Error: could not initialize GPIO Interrupts\n");
        return SRF04_INT;
    }
    gpio_irq_disable(dev->p.echo);

    return SRF04_OK;
}

int srf04_trigger(const srf04_t* dev)
{
    gpio_irq_enable(dev->p.echo);

    //trigger on falling edge for 10us
    gpio_set(dev->p.trigger);
    xtimer_usleep(10);
    gpio_clear(dev->p.trigger);

    return SRF04_OK;
}

int srf04_read(const srf04_t* dev)
{
    return dev->p.distance;
}
