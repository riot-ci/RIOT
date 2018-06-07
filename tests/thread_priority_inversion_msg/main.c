/*
 * Copyright (C) 2017 Technische Universität Berlin
 *               2017,2018 Freie Universität Berlin
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
 * @brief       Test application for testing priority inheritance when using
 *              nested msg_send_receive calls
 *
 * @author      Thomas Geithner <thomas.geithner@dai-labor.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "thread.h"
#include "msg.h"
#include "xtimer.h"

#define TICK_LEN            (50UL * US_PER_MS)
#define EXPECTED_RESULT     (-3)

#define T_NUMOF             (3U)
#define MSG_TYPE            (0xabcd)

static char _stacks[T_NUMOF][THREAD_STACKSIZE_MAIN];
static kernel_pid_t _pids[T_NUMOF];
static const char *_names[] = { "t1", "t2", "t3" };

static int _result = 0;
static int _res_addsub = 1;

static inline void delay(unsigned ticks)
{
    xtimer_usleep(ticks * TICK_LEN);
}

static inline void event(int num, const char *t_name, const char *msg)
{
    /* record event */
    _result += (_res_addsub * num);
    _res_addsub *= -1;

    printf("Event %2i: %7s - %s\n", num, t_name, msg);
}

static void *t1(void *arg)
{
    (void)arg;
    msg_t m;
    msg_t rply;

    m.type = MSG_TYPE;
    m.content.value = (uint32_t)'M';

    delay(2);

    event(3, "t1", "sending msg to t3 (msg_send_receive)");
    msg_send_receive(&m, &rply, _pids[2]);
    event(6, "t1", "received reply");

    return NULL;
}

static void *t2(void *arg)
{
    (void)arg;

    delay(1);

    event(2, "t2", "starting infinite loop, potentially starving others");
    while (1) {
        thread_yield_higher();
    }

    return NULL;
}

static void *t3(void *arg)
{
    (void)arg;
    msg_t m;
    msg_t rply;

    rply.type = MSG_TYPE;
    rply.content.value = (uint32_t)'m';

    event(1, "t3", "waiting for incoming message");
    msg_receive(&m);
    event(4, "t3", "received message");

    event(5, "t3", "sending reply");
    msg_reply(&m, &rply);

    return NULL;
}

static thread_task_func_t _handlers[] = { t1, t2, t3 };

int main(void)
{
    puts("Test for showing priority inversion when using msg_send_receive\n");
    puts("If this tests succeeds, you should see 6 events appearing in order.\n"
         "The expected output should look like this:\n"
         "Event  1:      t3 - waiting for incoming message\n"
         "Event  2:      t2 - starting infinite loop, potentially starving others\n"
         "Event  3:      t1 - sending msg to t3 (msg_send_receive)\n"
         "Event  4:      t3 - received message\n"
         "Event  5:      t3 - sending reply\n"
         "Event  6:      t1 - received reply\n");
    puts("TEST OUTPUT:");

    /* create threads */
    for (unsigned i = 0; i < T_NUMOF; i++) {
        _pids[i] = thread_create(_stacks[i], sizeof(_stacks[i]),
                                 (THREAD_PRIORITY_MAIN + 1 + i),
                                 THREAD_CREATE_WOUT_YIELD,
                                 _handlers[i], NULL,
                                 _names[i]);
    }

    delay(3);

    if (_result == EXPECTED_RESULT) {
        puts("\n[SUCCESS]");
    }
    else {
        puts("\n[FAILED]");
    }

    return 0;
}
