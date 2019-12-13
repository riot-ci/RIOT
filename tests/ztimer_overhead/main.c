/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       ztimer overhead test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "ztimer.h"
#include "ztimer/periph.h"

#define BASE    1000
#define SAMPLES 1024

static ztimer_periph_t _ztimer_periph;

int main(void)
{
    ztimer_periph_init(&_ztimer_periph, 0, 1000000LU, 0xffffffff);
    _ztimer_periph.min = 10;
    _ztimer_periph.adjust = 0;

    uint32_t total = 0;

    uint16_t min = 0xFFFF;
    uint16_t max = 0;

    unsigned n = SAMPLES;
    while (n--) {
        unsigned overhead = ztimer_overhead(&_ztimer_periph.super, BASE);
        total += overhead;
        if (overhead < min) {
            min = overhead;
        }
        else if (overhead > max) {
            max = overhead;
        }
    }

    printf("min=%u max=%u avg=%" PRIu32 "\n", min, max, (total / SAMPLES));

    return 0;
}
