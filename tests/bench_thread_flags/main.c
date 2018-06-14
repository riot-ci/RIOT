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
 * @brief   thread flags benchmark test application
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"

#include "thread_flags.h"
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

static char stack[THREAD_STACKSIZE_MAIN];
static void *second_thread(void *arg)
{
    (void) arg;
    while(1) {
        thread_flags_wait_any(0x0-1);
    }

    return NULL;
}

int main(void)
{
    printf("main starting\n");

    kernel_pid_t other = thread_create(stack,
                  sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  second_thread,
                  NULL,
                  "second_thread");

    thread_t *tcb = (thread_t*)sched_threads[other];

    xtimer_t timer;
    timer.callback = _timer_callback;

    unsigned n = 0;
    xtimer_set(&timer, TEST_DURATION);
    while(!flag) {
        thread_flags_set(tcb, 0x1);
        n++;
    }
    flag = 0;
    printf("Test complete. n=%u\n", n);

    return 0;
}
