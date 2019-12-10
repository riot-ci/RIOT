/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Hauke Petersen <hauke.petersen@fu-berlin.de>
 *               2019 OvGU Magdeburg <fabian.huessler@ovgu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 * @file
 * @brief       Default configuration for the AT86RF2xx driver
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF2XX_PARAMS_H
#define AT86RF2XX_PARAMS_H

#include "board.h"
#include "kernel_defines.h"
#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AT86RF2xx driver
 * @{
 */
#ifndef AT86RF2XX_PARAM_SPI
#define AT86RF2XX_PARAM_SPI         (SPI_DEV(0))
#endif
#ifndef AT86RF2XX_PARAM_SPI_CLK
#define AT86RF2XX_PARAM_SPI_CLK     (SPI_CLK_5MHZ)
#endif
#ifndef AT86RF2XX_PARAM_CS
#define AT86RF2XX_PARAM_CS          (GPIO_PIN(0, 0))
#endif
#ifndef AT86RF2XX_PARAM_INT
#define AT86RF2XX_PARAM_INT         (GPIO_PIN(0, 1))
#endif
#ifndef AT86RF2XX_PARAM_SLEEP
#define AT86RF2XX_PARAM_SLEEP       (GPIO_PIN(0, 2))
#endif
#ifndef AT86RF2XX_PARAM_RESET
#define AT86RF2XX_PARAM_RESET       (GPIO_PIN(0, 3))
#endif

#ifndef AT86RF2XX_PARAMS
#define AT86RF2XX_PARAMS            { .spi = AT86RF2XX_PARAM_SPI,         \
                                      .spi_clk = AT86RF2XX_PARAM_SPI_CLK, \
                                      .cs_pin = AT86RF2XX_PARAM_CS,       \
                                      .int_pin = AT86RF2XX_PARAM_INT,     \
                                      .sleep_pin = AT86RF2XX_PARAM_SLEEP, \
                                      .reset_pin = AT86RF2XX_PARAM_RESET }
#endif
/**@}*/

/* We need to distinguisch params for any at86rf2xx type because
   the number of devices is taken from the number of params.
   Potentially every type can have a different size. For static
   memory allocation we cannot just have an at86rf2xx_t[] array. */

#if IS_USED(MODULE_AT86RF212B)
#include "at86rf212b_params.h"
/**
 * @brief   AT86RF212B configuration parameters
 */
#ifndef AT86RF212B_PARAMS
#define AT86RF212B_PARAMS           { .base_params = AT86RF2XX_PARAMS }
/**
 * @brief   Array of configuration parameters for AT86RF212B transceivers
 */
static const at86rf212b_params_t at86rf212b_params[] =
{
    AT86RF212B_PARAMS
};
#endif
/**
 * @brief   Number of connected AT86RF212B transceivers
 */
#define AT86RF212B_NUM_OF   ARRAY_SIZE(at86rf212b_params)
#else
#define AT86RF212B_NUM_OF   0
#endif

#if IS_USED(MODULE_AT86RF231)
#include "at86rf231_params.h"
/**
 * @brief   AT86RF231 configuration parameters
 */
#ifndef AT86RF231_PARAMS
#define AT86RF231_PARAMS            { .base_params = AT86RF2XX_PARAMS }
#endif
/**
 * @brief   Array of configuration parameters for AT86RF231 transceivers
 */
static const at86rf231_params_t at86rf231_params[] =
{
    AT86RF231_PARAMS
};
/**
 * @brief   Number of connected AT86RF231 transceivers
 */
#define AT86RF231_NUM_OF    ARRAY_SIZE(at86rf231_params)
#else
#define AT86RF231_NUM_OF    0
#endif

#if IS_USED(MODULE_AT86RF232)
#include "at86rf232_params.h"
/**
 * @brief   AT86RF232 configuration parameters
 */
#ifndef AT86RF232_PARAMS
#define AT86RF232_PARAMS            { .base_params = AT86RF2XX_PARAMS }
#endif
/**
 * @brief   Array of configuration parameters for AT86RF232 transceivers
 */
static const at86rf232_params_t at86rf232_params[] =
{
    AT86RF232_PARAMS
};
/**
 * @brief   Number of connected AT86RF232 transceivers
 */
#define AT86RF232_NUM_OF    ARRAY_SIZE(at86rf232_params)
#else
#define AT86RF232_NUM_OF    0
#endif

#if IS_USED(MODULE_AT86RF233)
#include "at86rf233_params.h"
/**
 * @brief   AT86RF233 configuration parameters
 */
#ifndef AT86RF233_PARAMS
#define AT86RF233_PARAMS            { .base_params = AT86RF2XX_PARAMS }
#endif
/**
 * @brief   Array of configuration parameters for AT86RF233 transceivers
 */
static const at86rf233_params_t at86rf233_params[] =
{
    AT86RF233_PARAMS
};
/**
 * @brief   Number of connected AT86RF233 transceivers
 */
#define AT86RF233_NUM_OF    ARRAY_SIZE(at86rf233_params)
#else
#define AT86RF233_NUM_OF    0
#endif

#if IS_USED(MODULE_AT86RFA1)
/**
 * @brief   Number of connected AT86RFA1 transceivers
 */
#define AT86RFA1_NUM_OF     1
#else
#define AT86RFA1_NUM_OF     0
#endif
#if IS_USED(MODULE_AT86RFR2)
/**
 * @brief   Number of connected AT86RFR2 transceivers
 */
#define AT86RFR2_NUM_OF     1
#else
#define AT86RFR2_NUM_OF     0
#endif

#define AT86RF2XX_NUM               AT86RF212B_NUM_OF + \
                                    AT86RF231_NUM_OF  + \
                                    AT86RF232_NUM_OF  + \
                                    AT86RF233_NUM_OF  + \
                                    AT86RFA1_NUM_OF   + \
                                    AT86RFR2_NUM_OF

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_PARAMS_H */
/** @} */
