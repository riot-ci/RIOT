/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Freie Universität Berlin
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_test_utils
 * @{
 *
 * @file
 * @brief       Module creating a dummy thread
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "thread.h"

static char _dummy_stack[THREAD_STACKSIZE_IDLE];

static void *_dummy_thread(void *arg)
{
    (void)arg;
    while (1) {
        thread_sleep();
    }

    return NULL;
}

void dummy_thread_create(void)
{
    thread_create(_dummy_stack, sizeof(_dummy_stack),
                  THREAD_PRIORITY_IDLE,
                  THREAD_CREATE_WOUT_YIELD \
                  | THREAD_CREATE_STACKTEST \
                  | THREAD_CREATE_SLEEPING,
                  _dummy_thread, NULL, "dummy");
}
