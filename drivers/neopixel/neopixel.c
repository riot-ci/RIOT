/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_neopixel
 *
 * @{
 *
 * @file
 * @brief       Driver for the WS2812 or the SK6812 RGB LEDs sold as NeoPixel
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "neopixel.h"
#include "neopixel_constants.h"
#include "neopixel_params.h"
#include "periph/gpio.h"

/* Default buffer used in neopixel_params.h. Will be optimized out if unused */
uint8_t neopixel_buf[NEOPIXEL_PARAM_NUMOF * NEOPIXEL_BYTES_PER_DEVICE];

int neopixel_init(neopixel_t *dev, const neopixel_params_t *params)
{
    if (!dev || !params || !params->buf) {
        return -EINVAL;
    }

    memset(dev, 0, sizeof(neopixel_t));
    dev->params = *params;

    if (gpio_init(dev->params.pin, GPIO_OUT)) {
        return -EIO;
    }

    return 0;
}

void neopixel_set(neopixel_t *dev, uint16_t n, color_rgb_t c)
{
    assert(dev);
    dev->params.buf[NEOPIXEL_BYTES_PER_DEVICE * n + NEOPIXEL_OFFSET_R] = c.r;
    dev->params.buf[NEOPIXEL_BYTES_PER_DEVICE * n + NEOPIXEL_OFFSET_G] = c.g;
    dev->params.buf[NEOPIXEL_BYTES_PER_DEVICE * n + NEOPIXEL_OFFSET_B] = c.b;
}
