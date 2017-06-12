/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the Grove ledbar
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>

#include "log.h"
#include "xtimer.h"
#include "grove_ledbar.h"
#include "grove_ledbar_params.h"

#define RUNS                (5U)
#define STEP                (5U)
#define WAIT                (42*US_PER_MS)

int main(void)
{
    grove_ledbar_t dev;
    /* init display */
    puts("[START]");
    if (grove_ledbar_init(&dev, &grove_ledbar_params[0]) != 0) {
        puts("[FAILED]");
        return 1;
    }
    LOG_INFO(" stepwise increase LED bar to 100%% and then decrease to 0%%.\n\n");
    for (unsigned r = 0; r < RUNS; ++r) {
        LOG_INFO(" >>> round %u\n", (r+1));
        uint8_t lvl = 0;
        while (lvl < GROVE_LEDBAR_MAX - STEP) {
            grove_ledbar_set(&dev, lvl);
            lvl += STEP;
            xtimer_usleep(WAIT);
        }
        grove_ledbar_set(&dev, GROVE_LEDBAR_MAX);
        /* turn all off */
        xtimer_usleep(WAIT);
        lvl = GROVE_LEDBAR_MAX;
        while (lvl > STEP) {
            grove_ledbar_set(&dev, lvl);
            lvl -= STEP;
            xtimer_usleep(WAIT);
        }
        /* turn all off */
        grove_ledbar_clear(&dev);
    }
    puts("[SUCCESS]");

    return 0;
}
