/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for srf04 ultra sonic range finder driver
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "srf04_params.h"
#include "srf04.h"
#include "xtimer.h"

#define SAMPLE_PERIOD       (50U * US_PER_MS)

int main(void)
{
    puts("SRF04 range finder example");

    srf04_t dev;

    srf04_init(&dev);

    while (1) {
        srf04_trigger(&dev);

        xtimer_usleep(SAMPLE_PERIOD);

        printf("D: %d mm\n", srf04_read_distance(&dev));
    }

    return 0;
}
