/*
 * Copyright (C) 2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 * @file
 * @brief       Device properties for AT86RF2XX transceivers
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF2XX_PROPERTIES_H
#define AT86RF2XX_PROPERTIES_H

#include <stdint.h>
#include "at86rf2xx_dev_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   RSSI_BASE values of different AT86RF2XX transceivers
 */
extern const int8_t at86rf2xx_rssi_base_values[AT86RF2XX_DEV_TYPE_NUM_OF];
/**
 * @brief   Partnumbers of different AT86RF2XX transceivers
 */
extern const uint8_t at86rf2xx_partnums[AT86RF2XX_DEV_TYPE_NUM_OF];
/**
 * @brief   Minimum channel of different AT86RF2XX transceivers
 */
extern const uint8_t at86rf2xx_min_channels[AT86RF2XX_DEV_TYPE_NUM_OF];
/**
 * @brief   Maximum channel of different AT86RF2XX transceivers
 */
extern const uint8_t at86rf2xx_max_channels[AT86RF2XX_DEV_TYPE_NUM_OF];
/**
 * @brief   Default channel of different AT86RF2XX transceivers
 */
extern const uint8_t at86rf2xx_default_channels[AT86RF2XX_DEV_TYPE_NUM_OF];
/**
 * @brief   Wakw up delays of different AT86RF2XX transceivers
 */
extern const uint16_t at86rf2xx_wakeup_delays[AT86RF2XX_DEV_TYPE_NUM_OF];

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_PROPERTIES_H */
/** @} */
