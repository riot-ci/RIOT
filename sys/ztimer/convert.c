/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer_convert
 * @{
 *
 * @file
 * @brief       ztimer frequency conversion module common code implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "ztimer/convert.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void ztimer_convert_cancel(ztimer_clock_t *ztimer)
{
    ztimer_convert_t *ztimer_convert = (ztimer_convert_t *)ztimer;

    ztimer_remove(ztimer_convert->lower, &ztimer_convert->lower_entry);
}

void ztimer_convert_init(ztimer_convert_t *ztimer_convert, ztimer_clock_t *lower,
                         uint32_t max_value)
{
    ztimer_convert_t tmp = {
        .lower = lower,
        .lower_entry = {
            .callback = (void (*)(void *))ztimer_handler,
            .arg = ztimer_convert,
        },
        .super.max_value = max_value,
    };

    *ztimer_convert = tmp;

    DEBUG("ztimer_convert_init() max_value=%" PRIu32 "\n",
          ztimer_convert->super.max_value);
}
