/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_disp_dev Display device generic API
 * @ingroup     sys
 * @brief       Define the generic API of a display device
 * @{
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef DISP_DEV_H
#define DISP_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forward declaration for display device struct
 */
typedef struct disp_dev disp_dev_t;

/**
 * @brief   Generic type for a display driver
 */
typedef struct {
    /**
     * @brief   Map an area to display on the device
     *
     * @param[in] dev   Pointer to the display device
     * @param[in] x1    Left coordinate
     * @param[in] x2    Right coordinate
     * @param[in] y1    Top coordinate
     * @param[in] y2    Bottom coordinate
     * @param[in] color Array of color to map to the display
     */
    void (*map)(disp_dev_t *dev,
                uint16_t x1, uint16_t x2,
                uint16_t y1, uint16_t y2,
                const uint16_t *color);
} disp_dev_driver_t;

/**
 * @brief   Generic type for a display device
 */
struct disp_dev {
    const disp_dev_driver_t *driver;    /**< Pointer to driver of the display device */
};

#ifdef __cplusplus
}
#endif

#endif /* DISP_DEV_H */
/** @} */
