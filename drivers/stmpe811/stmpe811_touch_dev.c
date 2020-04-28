/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_stmpe811
 * @{
 *
 * @file
 * @brief       Driver adaption to touch_dev generic interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "stmpe811.h"
#include "stmpe811_touch_dev.h"

uint16_t _stmpe811_height(const touch_dev_t *touch_dev)
{
    const stmpe811_t *dev = (stmpe811_t *)touch_dev;
    assert(dev);

    return dev->params.ymax;
}

uint16_t _stmpe811_width(const touch_dev_t *touch_dev)
{
    const stmpe811_t *dev = (stmpe811_t *)touch_dev;
    assert(dev);

    return dev->params.xmax;
}

void _stmpe811_position(const touch_dev_t *touch_dev, touch_position_t *position)
{
    stmpe811_t *dev = (stmpe811_t *)touch_dev;
    assert(dev);
    assert(position);

    stmpe811_touch_position_t pos;
    stmpe811_read_touch_position(dev, &pos);
    position->x = pos.x;
    position->y = pos.y;
}

bool _stmpe811_is_pressed(const touch_dev_t *touch_dev)
{
    const stmpe811_t *dev = (stmpe811_t *)touch_dev;
    assert(dev);

    stmpe811_touch_state_t state;
    stmpe811_read_touch_state(dev, &state);
    return (state == STMPE811_TOUCH_STATE_PRESSED);
}

const touch_dev_driver_t stmpe811_touch_dev_driver = {
    .height     = _stmpe811_height,
    .width      = _stmpe811_width,
    .position   = _stmpe811_position,
    .is_pressed = _stmpe811_is_pressed,
};
