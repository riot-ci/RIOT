/*
 * Copyright (C) 2017 HAW-Hamburg
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
 * @brief       xtimer_usleep_short test application
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @}
 */
#include <stdio.h>
#include "xtimer.h"

#define TEST_USLEEP_MIN (0)
#define TEST_USLEEP_MAX (500)

#ifdef BOARD_NATIVE
/* native can sometime take more time to respond as it is not real time */
#define TEST_XTIMER_USLEEP_SHORT_SLEEP_MARGIN_US (1000)
#else
#define TEST_XTIMER_USLEEP_SHORT_SLEEP_MARGIN_US (20)
#endif /* BOARD_NATIVE */

int main(void)
{
    xtimer_sleep(3);
    printf("This test will call xtimer_usleep for values from %d down to %d\n",
           TEST_USLEEP_MAX, TEST_USLEEP_MIN);

    printf("Expected delay margin is %d us\n",
           TEST_XTIMER_USLEEP_SHORT_SLEEP_MARGIN_US);

    uint32_t test_time = 0, sleeping_time = 0, margin_fault = 0;

    for (int i = TEST_USLEEP_MAX; i >= TEST_USLEEP_MIN; i--) {
        printf("going to sleep %d us\n", i);
        uint32_t start = xtimer_now_usec();
        xtimer_usleep(i);
        uint32_t slept = xtimer_now_usec() - start;
        printf("Slept for      %" PRIu32 " us\n", slept);

        if (slept < (unsigned int)i) {
            puts("Timeout to short");
            ++margin_fault;
        }
        else if (slept > (unsigned int)i + TEST_XTIMER_USLEEP_SHORT_SLEEP_MARGIN_US) {
            puts("Timeout longer than expected margin.");
            ++margin_fault;
        }

        sleeping_time += slept;
        test_time += i;
    }

    printf("Slept for %" PRIu32 " us expected %" PRIu32 " us\n",
           sleeping_time, test_time);

    if (margin_fault != 0) {
        printf("Sleep delay margin was not kept for %" PRIu32 " times\n", margin_fault);
        puts("[FAILED]");
        return 1;
    }
    else {
        puts("[SUCCESS]");
        return 0;
    }
}
