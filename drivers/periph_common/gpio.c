/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file        gpio.c
 * @brief       Fallback Implementations for platform specific GPIO routines.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <assert.h>
#include "periph/gpio.h"

#ifndef GPIO_HAVE_INIT_LEVEL

int gpio_init_low(gpio_t pin, gpio_mode_t mode)
{
    assert(mode != GPIO_IN);
    assert(mode != GPIO_IN_PD);
    assert(mode != GPIO_IN_PU);

    gpio_clear(pin);
    return gpio_init(pin, mode);
}

int gpio_init_high(gpio_t pin, gpio_mode_t mode)
{
    assert(mode != GPIO_IN);
    assert(mode != GPIO_IN_PD);
    assert(mode != GPIO_IN_PU);

    gpio_set(pin);
    return gpio_init(pin, mode);
}

#else
typedef int dont_be_pedantic;
#endif /* GPIO_HAVE_INIT_LEVEL */
