/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ili9341
 * @{
 *
 * @file
 * @brief       Driver adaption to disp_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <stdint.h>
#include <errno.h>

#include "ili9341.h"
#include "ili9341_disp_dev.h"

static void _ili9341_map(disp_dev_t *dev, uint16_t x1, uint16_t x2,
                  uint16_t y1, uint16_t y2, const uint16_t *color)
{
    ili9341_t *ili9341 = (ili9341_t *)dev;
    ili9341_pixmap(ili9341, x1, x2, y1, y2, color);
}

static int _ili9341_get(disp_dev_t *disp_dev, disp_opt_t opt, void *value, size_t max_len)
{
    const ili9341_t *dev = (ili9341_t *)disp_dev;

    if (disp_dev == NULL) {
        return -ENODEV;
    }

    int res = -ENOTSUP;
    switch (opt) {
        case DISP_OPT_MAX_WIDTH:
            assert(max_len == sizeof(uint16_t));
            *(uint16_t *)value = dev->params->lines;
            res = sizeof(uint16_t);
            break;

        case DISP_OPT_MAX_HEIGHT:
            assert(max_len == sizeof(uint16_t));
            *(uint16_t *)value = 240;
            res = sizeof(uint16_t);
            break;

        case DISP_OPT_COLOR_DEPTH:
            assert(max_len == sizeof(uint8_t));
            *(uint8_t *)value = 16;
            res = sizeof(uint8_t);
            break;

        default:
            break;
    }

    return res;
}

static int _ili9341_set(disp_dev_t *disp_dev, disp_opt_t opt, const void *value, size_t max_len)
{
    const ili9341_t *dev = (ili9341_t *)disp_dev;

    if (disp_dev == NULL) {
        return -ENODEV;
    }

    int res = -ENOTSUP;
    switch (opt) {
        case DISP_OPT_COLOR_INVERT:
            assert(max_len == sizeof(bool));
            if (*(bool *)value) {
                ili9341_invert_on(dev);
            }
            else {
                ili9341_invert_off(dev);
            }
            res = sizeof(bool);
            break;

        default:
            break;
    }

    return res;
}

const disp_dev_driver_t ili9341_disp_dev_driver = {
    .map = _ili9341_map,
    .get = _ili9341_get,
    .set = _ili9341_set,
};
