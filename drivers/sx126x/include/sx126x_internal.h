/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sx126x_internal SX1261/2/8 and LLCC68 internal functions
 * @ingroup     drivers_sx126x
 * @brief       Internal functions for the SX1261/2/8 and LLCC68
 *
 * @{
 *
 * @file
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef SX126X_INTERNAL_H
#define SX126X_INTERNAL_H

#include "sx126x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     Check the variant of sx126x
 *
 * @warning   This only works if @ref SX126X_SINGLE is not set! This means,
 *            more than one variant is present. If @ref SX126X_SINGLE is set,
 *            this functions asserts.
 *
 * @param[in] dev                       Device descriptor.
 * @param[in] type                      Variant of the driver to compare to.
 *
 * @return    true if @p type matches the variant type
 * @return    false otherwise
 */
static inline bool sx126x_check_type(sx126x_t *dev, sx126x_type_t type)
{
#if SX126X_SINGLE
    (void) dev;
    (void) type;
    assert(false);
    return false;
#else
    return dev->params->type == type;
#endif
}

/**
 * @brief   Check whether the device model is sx1261
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return    true if the device is sx1261
 * @return    false otherwise
 */
static inline bool sx126x_is_sx1261(sx126x_t *dev)
{
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX1261);
    }
    else {
        return sx126x_check_type(dev, SX126X_SX1261);
    }
}

/**
 * @brief   Check whether the device model is sx1262
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return    true if the device is sx1262
 * @return    false otherwise
 */
static inline bool sx126x_is_sx1262(sx126x_t *dev)
{
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX1262);
    }
    else {
        return sx126x_check_type(dev, SX126X_SX1262);
    }
}

/**
 * @brief   Check whether the device model is llcc68
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return    true if the device is llcc68
 * @return    false otherwise
 */
static inline bool sx126x_is_llcc68(sx126x_t *dev)
{
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_LLCC68);
    }
    else {
        return sx126x_check_type(dev, SX126X_LLCC68);
    }
}

/**
 * @brief   Check whether the device model is sx1268
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @return    true if the device is sx1268
 * @return    false otherwise
 */
static inline bool sx126x_is_sx1268(sx126x_t *dev)
{
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX1268);
    }
    else {
        return sx126x_check_type(dev, SX126X_SX1268);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* SX126X_INTERNAL_H */
/** @} */
