/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ztimer_convert
 * @ingroup     sys_ztimer
 * @brief       ztimer frequency conversion modules
 *
 * @{
 * @file
 * @brief       ztimer frequency conversion base module
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ZTIMER_CONVERT_H
#define ZTIMER_CONVERT_H

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief base type for ztimer convert modules
 *
 * This type is supposed to be extended. It provides common fields for a ztimer
 * clock that has a parent clock.
 * */
typedef struct {
    ztimer_clock_t super;       /**< ztimer_clock super class       */
    ztimer_clock_t *lower;      /**< lower clock device             */
    ztimer_t lower_entry;       /**< timer entry in parent clock     */
} ztimer_convert_t;

/**
 * @brief    Initialization function for ztimer_convert_t
 *
 * @p max_value needs to be set to the maximum value that can be converted
 * without overflowing. E.g., if the conversion module slows down a lower
 * clock by factor X, max_value needs to be set to 0xffffffff / X.
 *
 * @param[in,out]   ztimer_convert  object to initialize
 * @param[in]       lower           lower ztimer clock
 * @param[in]       max_value       maximum value for this clock's set()
 */
void ztimer_convert_init(ztimer_convert_t *ztimer_convert,
                         ztimer_clock_t *lower, uint32_t max_value);

/**
 * @brief   ztimer_convert common cancel() op
 *
 * Used by some conversion modules as ztimer_clock_t::ops.cancel().
 *
 * @param[in]   ztimer  ztimer device to operate on
 */
void ztimer_convert_cancel(ztimer_clock_t *ztimer);

#ifdef __cplusplus
}
#endif

#endif /* ZTIMER_CONVERT_H */
/** @} */
