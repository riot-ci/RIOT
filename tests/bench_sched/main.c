/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief   scheduler benchmark test application
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"

#include "xtimer.h"

#ifndef TEST_DURATION
#define TEST_DURATION 1000000U
#endif

volatile unsigned flag = 0;

static void _timer_callback(void*arg)
{
    (void)arg;
    flag = 1;
}

int main(void)
{
    printf("main starting\n");

    xtimer_t timer;
    timer.callback = _timer_callback;

    unsigned n = 0;
    xtimer_set(&timer, TEST_DURATION);
    while(!flag) {
        thread_yield();
        n++;
    }
    flag = 0;
    printf("Test complete. n=%u\n", n);

    return 0;
}
