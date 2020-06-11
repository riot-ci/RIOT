/*
 * Copyright (C) 2020 Inria
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
 * @brief       Test to figure out RTT_MIN_VALUE
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdatomic.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/rtt.h"

#include "xtimer.h"

#define MAX_WAIT_US     (100)
#define US_PER_TICK     (US_PER_SEC / RTT_FREQUENCY)

#ifndef SAMPLES
#define SAMPLES 1024LU
#endif

static atomic_bool cb_triggered;

void cb(void *arg)
{
    (void)arg;
    cb_triggered = true;
}

int main(void)
{
    uint32_t value = 0;

    rtt_init();

    printf("Evaluate RTT_MIN_VALUE over %" PRIu32 " samples\n",
            (uint32_t)SAMPLES);

    for(unsigned i = 0;  i < SAMPLES; i++) {
        uint32_t offset = 0;
        cb_triggered = false;
        while (cb_triggered == false) {
            offset++;
            uint32_t now = rtt_get_counter();
            rtt_set_alarm((now + offset) % RTT_MAX_VALUE, cb, 0);
            xtimer_usleep(offset * US_PER_TICK + MAX_WAIT_US);
        }
        if(offset > value) {
            value = offset;
        }
        printf(".");
        fflush(stdout);
    }
    printf("\n");

    printf("RTT_MIN_VALUE for %s: %" PRIu32 "\n", RIOT_BOARD, value);

    return 0;
}
