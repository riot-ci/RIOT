/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dcf77
 *
 * @{
 * @file
 * @brief       Default configuration for DCF77 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef DCF77_PARAMS_H
#define DCF77_PARAMS_H

#include "board.h"
#include "dcf77.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the DCF77 devices
 * @{
 */
#ifndef DCF77_PARAM_PIN
#define DCF77_PARAM_PIN               (GPIO_PIN(PB, 22))
#endif
#ifndef DCF77_PARAM_PULL
#define DCF77_PARAM_PULL              (GPIO_IN)
#endif
#ifndef DCF77_PARAMS
#define DCF77_PARAMS                  { .pin     = DCF77_PARAM_PIN, \
                                        .in_mode = DCF77_PARAM_PULL }
#endif
#ifndef DCF77_SAULINFO
#define DCF77_SAULINFO                { .name = "dcf77" }
#endif
/**@}*/

/**
 * @brief   Configure DHT devices
 */
static const dcf77_params_t dcf77_params[] =
{
    DCF77_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
static const saul_reg_info_t dcf77_saul_info[] =
{
    DCF77_SAULINFO
};

#ifdef __cplusplus
}
#endif

#endif /* DCF77_PARAMS_H */
/** @} */
