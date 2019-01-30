/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief   Thread race condition test application to reproduce
 *          https://github.com/RIOT-OS/RIOT/issues/10881
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <unistd.h>

#include "periph/timer.h"
#include "random.h"
#include "thread.h"
#include "msg.h"

#define CANARY_TYPE         (0x21fd)

#define TIMER_FREQ          (1000000LU)
#define TIMER_TIMEOUT_MIN   (1U)
#define TIMER_TIMEOUT_MAX   (100U)

static char _stack[THREAD_STACKSIZE_DEFAULT];

static kernel_pid_t _pid_main = KERNEL_PID_UNDEF;

/**
 * @brief   Schedule next timer event in TIMER_TIMEOUT_MIN to TIMER_TIMEOUT_MAX
 *          ticks.
 */
static void _sched_next(void)
{
    timer_set(TIMER_DEV(0), 0, random_uint32_range(TIMER_TIMEOUT_MIN,
                                                   TIMER_TIMEOUT_MAX));
}

/**
 * @brief   The timer interrupt
 */
static void _timer(void *arg, int channel)
{
    (void)arg;
    (void)channel;
    /* just continue rescheduling interrupt triggering at random time */
    _sched_next();
}

/**
 * @brief   The sending thread
 */
static void *_thread(void *arg)
{
    (void) arg;

    while (1) {
        msg_t msg = { .type = 0U };

        write(STDOUT_FILENO, ".", 1U);
        /* send without blocking */
        msg_try_send(&msg, _pid_main);
        thread_yield();
    }

    return NULL;
}

int main(void)
{
    kernel_pid_t pid;

    timer_init(TIMER_DEV(0), TIMER_FREQ, _timer, NULL);
    random_init(timer_read(TIMER_DEV(0)));
    puts("Test is \"successful\" if it runs forever without halting\n"
         "on any of the assertion in this file");
    _pid_main = sched_active_pid;

    /* try to trigger an interrupt at random intervals this caused
     * the `thread_yield_higher()` function to get confused and never
     * call `sched_run()` in certain conditions */
    _sched_next();
    pid = thread_create(_stack, sizeof(_stack), THREAD_PRIORITY_MAIN + 1,
                        THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                        _thread, NULL, "nr2");
    assert(pid != KERNEL_PID_UNDEF);

    while (1) {
        msg_t msg = { .type = CANARY_TYPE };

        /* receive blocked */
        msg_receive(&msg);
        /* check msg_receive() returned without blocking (i.e. the sending
         * thread did not get a chance to copy the message over) */
        assert(msg.type != CANARY_TYPE);
        write(STDOUT_FILENO, "\b", 1U);
    }
    return 0;
}
