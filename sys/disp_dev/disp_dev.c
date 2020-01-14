/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Helper functions for generic API of display device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>

#include "disp_dev.h"

void disp_dev_map(disp_dev_t *dev,
                 uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2,
                 const uint16_t *color)
{
    if (!dev) {
        return;
    }

    dev->driver->map(dev, x1, x2, y1, y2, color);
}

int disp_dev_get(disp_dev_t *dev,
                 disp_opt_t opt, void *value, size_t max_len)
{
    if (!dev) {
        return -EINVAL;
    }

    return dev->driver->get(dev, opt, value, max_len);
}

int disp_dev_set(disp_dev_t *dev,
                 disp_opt_t opt, const void *value, size_t max_len)
{
    if (!dev) {
        return -EINVAL;
    }

    return dev->driver->set(dev, opt, value, max_len);
}
