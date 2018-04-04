/*
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *               2018 Josua Arndt
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
 * @brief       xtimer_usleep test application
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "xtimer.h"
#include "timex.h"

#define RUNS                (5U)
#define SLEEP_TIMES_NUMOF   (sizeof(sleep_times) / sizeof(sleep_times[0]))

static const uint32_t sleep_times[] = { 10000, 50000, 10234, 56780, 12122, 98765, 75000 };

#define ERROR_US 70

/*
 * To use a pin to probe the sleep times enable the gpio module and define
 * respective SLEEP_GPIO_PIN and set SLEEP_PIN to (1)
 * */

#ifdef SLEEP_PIN
#include "board.h"
#include "periph/gpio.h"
#define SLEEP_GPIO_PIN GPIO_PIN(SLEEP_PORT, SLEEP_PIN)
#endif
int main(void)
{
    uint32_t start_test, testtime;

#ifdef SLEEP_PIN
    gpio_init(SLEEP_GPIO_PIN, GPIO_OUT);
#endif

    printf("Running test %u times with %u distinct sleep times\n", RUNS,
           (unsigned)SLEEP_TIMES_NUMOF);
    puts("Please hit any key and then ENTER to continue");
    getchar();
    start_test = xtimer_now_usec();
    for (unsigned m = 0; m < RUNS; m++) {
        for (unsigned n = 0;
             n < sizeof(sleep_times) / sizeof(sleep_times[0]);
             n++) {

            uint32_t start_sleep, diff;

            start_sleep = xtimer_now_usec();

#ifdef SLEEP_PIN
            gpio_set(SLEEP_GPIO_PIN);
#endif
            xtimer_usleep(sleep_times[n]);
#ifdef SLEEP_PIN
            gpio_clear(SLEEP_GPIO_PIN);
#endif

            diff = xtimer_now_usec() - start_sleep;

            int32_t err = (diff - sleep_times[n]);

            printf("Slept for %" PRIu32 " us (expected: %" PRIu32 " us) "
                   "Offset: %d us\n", diff, sleep_times[n], err);
        } //
    }
    testtime = xtimer_now_usec() - start_test;
    printf("Test ran for %" PRIu32 " us\n", testtime);

    return 0;
}
