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

#include <stdint.h>

/**
 * @brief   Global list of configuration options available for the disp dev API
 */
typedef enum {
    /**
     * @brief   (uint16_t) maximum width in pixels
     */
    DISP_OPT_MAX_WIDTH,

    /**
     * @brief   (uint16_t) maximum height in pixels
     */
    DISP_OPT_MAX_HEIGHT,

    /**
     * @brief   (uint8_t) color depth
    */
    DISP_OPT_COLOR_DEPTH,

    /**
     * @brief   (bool) enable/disable invert modes
    */
    DISP_OPT_COLOR_INVERT,

} disp_opt_t;

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
                uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2,
                const uint16_t *color);

    /**
     * @brief   Get an option value from a given display device
     *
     * @param[in] dev       Pointer to the display device
     * @param[in] opt       Option type
     * @param[out] value    Pointer to store the option's value in
     * @param[in] max_len   Maximal amount of byte that fit into @p value
     *
     * @return              number of bytes written to @p value
     * @return              `-ENOTSUP` if @p opt is not provided by the device
     */
    int (*get)(disp_dev_t *dev, disp_opt_t opt, void *value, size_t max_len);

    /**
     * @brief   Set an option value for a given display device
     *
     * @param[in] dev       Network device descriptor
     * @param[in] opt       Option type
     * @param[in] value     Value to set
     * @param[in] value_len The length of @p value
     *
     * @return              number of bytes written to @p value
     * @return              `-ENOTSUP` if @p opt is not configurable for the
     *                      device
     * @return              `-EINVAL` if @p value is an invalid value with
     *                      regards to @p opt
     */
    int (*set)(disp_dev_t *dev, disp_opt_t opt,
               const void *value, size_t value_len);
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
