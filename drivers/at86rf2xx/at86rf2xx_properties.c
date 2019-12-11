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
 *
 * @file
 * @brief       Device properties of AT86RF2XX transceivers
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include "kernel_defines.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf212b.h"
#include "at86rf231.h"
#include "at86rf232.h"
#include "at86rf233.h"
#include "at86rfa1.h"
#include "at86rfr2.h"

/**
 * @brief RSSI base values for each AT86RF2XX transceiver type
 */
const int8_t at86rf2xx_rssi_base_values[AT86RF2XX_DEV_TYPE_NUM_OF] = {
#if IS_USED(MODULE_AT86RF212B)
    AT86RF212B_RSSI_BASE_VAL,
#endif
#if IS_USED(MODULE_AT86RF231)
    AT86RF231_RSSI_BASE_VAL,
#endif
#if IS_USED(MODULE_AT86RF232)
    AT86RF232_RSSI_BASE_VAL,
#endif
#if IS_USED(MODULE_AT86RF233)
    AT86RF233_RSSI_BASE_VAL,
#endif
#if IS_USED(MODULE_AT86RFA1)
    AT86RFA1_RSSI_BASE_VAL,
#endif
#if IS_USED(MODULE_AT86RFR2)
    AT86RFR2_RSSI_BASE_VAL,
#endif
};

/**
 * @brief Part numbers for each AT86RF2XX transceiver type
 */
const uint8_t at86rf2xx_partnums[AT86RF2XX_DEV_TYPE_NUM_OF] = {
#if IS_USED(MODULE_AT86RF212B)
    AT86RF212B_PARTNUM,
#endif
#if IS_USED(MODULE_AT86RF231)
    AT86RF231_PARTNUM,
#endif
#if IS_USED(MODULE_AT86RF232)
    AT86RF232_PARTNUM,
#endif
#if IS_USED(MODULE_AT86RF233)
    AT86RF233_PARTNUM,
#endif
#if IS_USED(MODULE_AT86RFA1)
    AT86RFA1_PARTNUM,
#endif
#if IS_USED(MODULE_AT86RFR2)
    AT86RFR2_PARTNUM,
#endif
};

/**
 * @brief Minimum channel for each AT86RF2XX transceiver type
 */
const uint8_t at86rf2xx_min_channels[AT86RF2XX_DEV_TYPE_NUM_OF] = {
#if IS_USED(MODULE_AT86RF212B)
    AT86RF212B_MIN_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RF231)
    AT86RF231_MIN_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RF232)
    AT86RF232_MIN_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RF233)
    AT86RF233_MIN_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RFA1)
    AT86RFA1_MIN_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RFR2)
    AT86RFR2_MIN_CHANNEL,
#endif
};

/**
 * @brief Maximum channel for each AT86RF2XX transceiver type
 */
const uint8_t at86rf2xx_max_channels[AT86RF2XX_DEV_TYPE_NUM_OF] = {
#if IS_USED(MODULE_AT86RF212B)
    AT86RF212B_MAX_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RF231)
    AT86RF231_MAX_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RF232)
    AT86RF232_MAX_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RF233)
    AT86RF233_MAX_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RFA1)
    AT86RFA1_MAX_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RFR2)
    AT86RFR2_MAX_CHANNEL,
#endif
};

/**
 * @brief Default channel for each AT86RF2XX transceiver type
 */
const uint8_t at86rf2xx_default_channels[AT86RF2XX_DEV_TYPE_NUM_OF] = {
#if IS_USED(MODULE_AT86RF212B)
    AT86RF212B_DEFAULT_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RF231)
    AT86RF231_DEFAULT_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RF232)
    AT86RF232_DEFAULT_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RF233)
    AT86RF233_DEFAULT_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RFA1)
    AT86RFA1_DEFAULT_CHANNEL,
#endif
#if IS_USED(MODULE_AT86RFR2)
    AT86RFR2_DEFAULT_CHANNEL,
#endif
};

/**
 * @brief State transition time from SLEEP to TRX_OFF
 *        for each AT86RF2XX transceiver type
 */
const uint16_t at86rf2xx_wakeup_delays[AT86RF2XX_DEV_TYPE_NUM_OF] = {
#if IS_USED(MODULE_AT86RF212B)
    AT86RF212B_WAKEUP_DELAY,
#endif
#if IS_USED(MODULE_AT86RF231)
    AT86RF231_WAKEUP_DELAY,
#endif
#if IS_USED(MODULE_AT86RF232)
    AT86RF232_WAKEUP_DELAY,
#endif
#if IS_USED(MODULE_AT86RF233)
    AT86RF233_WAKEUP_DELAY,
#endif
#if IS_USED(MODULE_AT86RFA1)
    AT86RFA1_WAKEUP_DELAY,
#endif
#if IS_USED(MODULE_AT86RFR2)
    AT86RFR2_WAKEUP_DELAY,
#endif
};
