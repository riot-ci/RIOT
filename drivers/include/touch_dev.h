/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_touch_dev Touch device generic API
 * @ingroup     drivers_misc
 * @brief       Define the generic API of a touch device
 * @experimental This API is experimental and in an early state - expect
 *               changes!
 * @{
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef TOUCH_DEV_H
#define TOUCH_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief   Forward declaration for touch device struct
 */
typedef struct touch_dev touch_dev_t;

/**
 * @brief   Touch position coordinates
 */
typedef struct {
    uint16_t x;     /**< X coordinate */
    uint16_t y;     /**< Y coordinate */
} touch_position_t;

/**
 * @brief   Generic type for a touch driver
 */
typedef struct {

    /**
     * @brief   Get the height of the touch device
     *
     * @param[in] dev       Pointer to the touch device
     *
     * @return              Height in points
     */
    uint16_t (*height)(const touch_dev_t *dev);

    /**
     * @brief   Get the width of the touch device
     *
     * @param[in] dev       Pointer to the touch device
     *
     * @return              Width in points
     */
    uint16_t (*width)(const touch_dev_t *dev);

    /**
     * @brief   Get the current pressed position of the touch device
     *
     * @param[in] dev       Pointer to the touch device
     * @param[out] position The position pressed
     */
    void (*position)(const touch_dev_t *dev, touch_position_t *position);

    /**
     * @brief   Check if the touch device is pressed
     *
     * @param[in] dev       Pointer to the touch device
     *
     * @return              true if the touch device is pressed, false otherwise
     */
    bool (*is_pressed)(const touch_dev_t *dev);

} touch_dev_driver_t;

/**
 * @brief   Generic type for a touch device
 */
struct touch_dev {
    const touch_dev_driver_t *driver;    /**< Pointer to driver of the touch device */
};

/**
 * @brief   Get the height of the touch device
 *
 * @param[in] dev       Pointer to the touch device
 *
 * @return              Height in points
 */
uint16_t touch_dev_height(const touch_dev_t *dev);

/**
 * @brief   Get the width of the touch device
 *
 * @param[in] dev       Pointer to the touch device
 *
 * @return              Width in points
 */
uint16_t touch_dev_width(const touch_dev_t *dev);

/**
 * @brief   Get the current pressed position of the touch device
 *
 * @param[in] dev       Pointer to the touch device
 * @param[out] position The position pressed
 */
void touch_dev_position(const touch_dev_t *dev, touch_position_t *position);

/**
 * @brief   Check if the touch device is pressed
 *
 * @param[in] dev       Pointer to the touch device
 *
 * @return              true if the touch device is pressed, false otherwise
 */
bool touch_dev_is_pressed(const touch_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* TOUCH_DEV_H */
/** @} */
