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

#include "ili9341.h"
#include "ili9341_disp_dev.h"

void _ili9341_map(disp_dev_t *dev, uint16_t x1, uint16_t x2,
                  uint16_t y1, uint16_t y2, const uint16_t *color)
{
    ili9341_t *ili9341 = (ili9341_t *)dev;
    ili9341_pixmap(ili9341, x1, x2, y1, y2, color);
}

const disp_dev_driver_t ili9341_disp_dev_driver = {
    .map = _ili9341_map,
};
