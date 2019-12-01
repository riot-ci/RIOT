/*
 * Copyright (C) 2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Types of supported AT86RF2xx devices
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef AT86RF2XX_DEV_TYPES_H
#define AT86RF2XX_DEV_TYPES_H

#include "kernel_defines.h"

/**
 * @brief Supported device types.
 *
 * The types are used as indices to access arrays in
 * @ref at86rf2xx_properties.c
 */
typedef enum at86rf2xx_dev_type {
#if IS_USED(MODULE_AT86RF212B) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RF212B,  /**< Type AT86RF212B */
#endif
#if IS_USED(MODULE_AT86RF231) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RF231,   /**< Type AT86RF231 */
#endif
#if IS_USED(MODULE_AT86RF232) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RF232,   /**< Type AT86RF232 */
#endif
#if IS_USED(MODULE_AT86RF233) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RF233,   /**< Type AT86RF233 */
#endif
#if IS_USED(MODULE_AT86RFA1) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RFA1,    /**< Type AT86RFA1 */
#endif
#if IS_USED(MODULE_AT86RFR2) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RFR2,    /**< Type AT86RFR2 */
#endif
    AT86RF2XX_DEV_TYPE_NUM_OF       /**< Number of used transceiver types */
} at86rf2xx_dev_type_t;

#endif /* AT86RF2XX_DEV_TYPES_H */
/** @} */
