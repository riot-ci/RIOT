/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_touch_dev
 * @{
 *
 * @file
 * @brief       Helper functions for generic API of touch device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <inttypes.h>

#include "touch_dev.h"

uint16_t touch_dev_height(touch_dev_t *dev)
{
    assert(dev);

    return dev->driver->height(dev);
}

uint16_t touch_dev_width(touch_dev_t *dev)
{
    assert(dev);

    return dev->driver->width(dev);
}

void touch_dev_position(touch_dev_t *dev, touch_position_t *position)
{
    assert(dev);

    dev->driver->position(dev, position);
}

bool touch_dev_is_pressed(touch_dev_t *dev)
{
    assert(dev);

    return dev->driver->is_pressed(dev);
}
