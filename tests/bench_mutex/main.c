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
 * @brief   simple mutex context switch benchmark
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"
#include "mutex.h"
#include "xtimer.h"

#ifndef TEST_DURATION
#define TEST_DURATION 1000000U
#endif

static mutex_t mutex = MUTEX_INIT;

static char stack[THREAD_STACKSIZE_MAIN];
static void *second_thread(void *arg)
{
    (void) arg;
    while(1) {
        mutex_lock(&mutex);
    }

    return NULL;
}

volatile unsigned flag = 0;

static void _timer_callback(void*arg)
{
    (void)arg;
    flag = 1;
}

int main(void)
{
    printf("main starting\n");

    puts("\nThis benchmark will start two threads, one with higher priority\n"
         "trying to lock a mutex in a loop, another unlocking the same mutex in a\n"
         "loop, causing a context switch at each iteration.\n"
         "After one second, the number of unlocks is printed (n=<value>)\n");

    thread_create(stack,
                  sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  second_thread,
                  NULL,
                  "second_thread");

    /* lock the mutex, then yield to second_thread */
    mutex_lock(&mutex);
    thread_yield_higher();

    xtimer_t timer;
    timer.callback = _timer_callback;

    unsigned n = 0;
    xtimer_set(&timer, TEST_DURATION);
    while(!flag) {
        mutex_unlock(&mutex);
        n++;
    }

    printf("Test complete. n=%u\n", n);

    return 0;
}
