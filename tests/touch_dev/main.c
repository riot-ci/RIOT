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
 * @brief       Generic touch device test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "xtimer.h"

#include "touch_dev.h"

#include "stmpe811.h"
#include "stmpe811_params.h"
#include "stmpe811_touch_dev.h"

#include "test_utils/expect.h"

static stmpe811_t stmpe811;

static void _touch_event_cb(void *arg)
{
    (void)arg;
    puts("Pressed!");
}

int main(void)
{
    stmpe811_init(&stmpe811, &stmpe811_params[0], _touch_event_cb, NULL);

    touch_dev_t *dev = (touch_dev_t *)&stmpe811;
    dev->driver = &stmpe811_touch_dev_driver;

    uint16_t xmax = touch_dev_width(dev);
    uint16_t ymax = touch_dev_height(dev);

    expect(xmax == stmpe811.params.xmax);
    expect(ymax == stmpe811.params.ymax);

    bool last_pressed = touch_dev_is_pressed(dev);
    bool current_pressed;

    while (1) {
        current_pressed = touch_dev_is_pressed(dev);
        if (current_pressed != last_pressed) {
            if (!current_pressed) {
                puts("Released!");
            }
            last_pressed = current_pressed;
        }

        /* Display touch position if pressed */
        if (current_pressed) {
            touch_position_t position;
            touch_dev_position(dev, &position);
            printf("X: %i, Y:%i\n", position.x, position.y);
        }

        xtimer_usleep(10 * US_PER_MS);
    }

    return 0;
}
