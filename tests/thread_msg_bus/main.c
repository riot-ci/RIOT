/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
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
 * @brief       Message bus test application
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "msg.h"
#include "msg_bus.h"

char t1_stack[THREAD_STACKSIZE_MAIN];
char t2_stack[THREAD_STACKSIZE_MAIN];
char t3_stack[THREAD_STACKSIZE_MAIN];

kernel_pid_t p_main, p1, p2, p3;

void *thread1(void *arg)
{
    const uint16_t events[] = {
        MSG_BUS_ID(0, 0x23),
        MSG_BUS_ID(0, 0x24),
    };

    msg_bus_entry_t sub = {
        .events = events,
        .num_events = ARRAY_SIZE(events),
    };

    puts("THREAD 1 start");

    msg_bus_subscribe(arg, &sub);

    msg_t msg;
    msg_receive(&msg);
    printf("T1 recv: %s (type=%x.%x)\n",
          (char*) msg.content.ptr, msg_bus_class(&msg), msg_bus_type(&msg));

    msg_bus_unsubscribe(arg);

    return NULL;
}

void *thread2(void *arg)
{
    const uint16_t events[] = {
        MSG_BUS_ID(0, 0x24),
    };

    msg_bus_entry_t sub = {
        .events = events,
        .num_events = ARRAY_SIZE(events),
    };

    puts("THREAD 2 start");

    msg_bus_subscribe(arg, &sub);

    msg_t msg;
    msg_receive(&msg);
    printf("T2 recv: %s (type=%x.%x)\n",
          (char*) msg.content.ptr, msg_bus_class(&msg), msg_bus_type(&msg));

    msg_bus_unsubscribe(arg);

    return NULL;
}

void *thread3(void *arg)
{
    const uint16_t events[] = {
        MSG_BUS_ID(0, 0x23),
    };

    msg_bus_entry_t sub = {
        .events = events,
        .num_events = ARRAY_SIZE(events),
    };

    puts("THREAD 3 start");

    msg_bus_subscribe(arg, &sub);

    msg_t msg;
    msg_receive(&msg);
    printf("T3 recv: %s (type=%x.%x)\n",
          (char*) msg.content.ptr, msg_bus_class(&msg), msg_bus_type(&msg));

    msg_bus_unsubscribe(arg);

    return NULL;
}

int main(void)
{
    list_node_t my_bus = {0};

    p_main = sched_active_pid;
    p1 = thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 3,
                       THREAD_CREATE_STACKTEST, thread1, &my_bus, "nr1");
    p2 = thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN - 2,
                       THREAD_CREATE_STACKTEST, thread2, &my_bus, "nr2");
    p3 = thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN - 1,
                       THREAD_CREATE_STACKTEST, thread3, &my_bus, "nr3");
    puts("THREADS CREATED");

    const char hello[] = "Hello Threads!";

    for (int id = 0x22; id < 0x25; ++id) {
        int woken = msg_bus_post(&my_bus, 0, id, (void*)hello);
        printf("Posted event 0x%x to %d threads\n", id, woken);
    }

    puts("SUCCESS");

    return 0;
}
