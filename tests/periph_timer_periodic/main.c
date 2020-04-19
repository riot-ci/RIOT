/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
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
 * @brief       Periodic timer test application
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "board.h"

#include "mutex.h"
#include "periph/timer.h"

/* the timer device to use */
#define TIMER_CYCL  (0)

static unsigned count[TIMER_CHANNELS];

static void cb(void *arg, int chan)
{
    int c = count[chan]++;

    if ((c & 0x3) == 0) {
        printf("[%d] tick\n", chan);
    }

    if (c > 20) {
        timer_stop(TIMER_CYCL);
        mutex_unlock(arg);
    }
}

static const char* _print_ok(int chan, bool *succeeded)
{
    if (chan == 0 && count[chan] > 0) {
        return "OK";
    }

    if (chan > 0 && count[chan] == 0) {
        return "OK";
    }

    *succeeded = false;
    return "ERROR";
}

int main(void)
{
    mutex_t lock = MUTEX_INIT_LOCKED;
    const unsigned timer_hz = 62500;
    const unsigned steps = (250UL * timer_hz) / 1000; /* 250 ms */

    printf("\nRunning Timer %d at %u Hz.\n", TIMER_CYCL, timer_hz);
    printf("One counter cycle is %u ticks or 250 ms\n", steps);
    puts("Will print 'tick' every second / every 4 cycles.\n");

    timer_init(TIMER_CYCL, timer_hz, cb, &lock);

    puts("TEST START");

    /* Only the first channel should trigger and reset the counter */
    /* If subsequent channels trigger this is an error. */
    timer_set_periodic(TIMER_CYCL, 1, 2 * steps, TIM_FLAG_RESET_ON_SET);
    timer_set_periodic(TIMER_CYCL, 0, steps, TIM_FLAG_RESET_ON_MATCH);

    mutex_lock(&lock);

    puts("\nCycles:");

    bool succeeded = true;
    for (unsigned i = 0; i < TIMER_CHANNELS; ++i) {
        printf("channel %u = %02u\t[%s]\n", i, count[i], _print_ok(i, &succeeded));
    }

    if (succeeded) {
        puts("TEST SUCCEEDED");
    } else {
        puts("TEST FAILED");
    }

    return 0;
}
