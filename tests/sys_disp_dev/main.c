/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Generic display device test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <assert.h>

#include "disp_dev.h"

#include "ili9341.h"
#include "ili9341_params.h"
#include "ili9341_disp_dev.h"

#include "riot_logo.h"

static ili9341_t ili9341;

int main(void)
{
    ili9341_init(&ili9341, &ili9341_params[0]);

    disp_dev_t *dev = (disp_dev_t *)&ili9341;
    dev->driver = &ili9341_disp_dev_driver;

    bool inverted = true;
    disp_dev_set(dev, DISP_OPT_COLOR_INVERT, &inverted, sizeof(bool));

    uint16_t max_height;
    uint16_t max_width;
    disp_dev_get(dev, DISP_OPT_MAX_WIDTH, &max_width, sizeof(uint16_t));
    disp_dev_get(dev, DISP_OPT_MAX_HEIGHT, &max_height, sizeof(uint16_t));

    assert(max_width == ili9341.params->lines);
    assert(max_height == 240);

    uint16_t color = 0;
    for (uint16_t x = 0; x < max_width; ++x) {
        for (uint16_t y = 0; y < max_height; ++y) {
            disp_dev_map(dev, x, x, y, y, &color);
        }
    }

    disp_dev_map(dev, 95, 222, 85, 153, (const uint16_t *)picture);

    puts("SUCCESS");

    return 0;
}
