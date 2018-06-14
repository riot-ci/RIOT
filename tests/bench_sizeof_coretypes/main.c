/*
 * Copyright (C) 2018 Freie Universität Berlin
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
 * @brief       Print size of core types
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "cib.h"
#include "clist.h"
#include "panic.h"
#include "kernel_types.h"
#include "list.h"
#include "mbox.h"
#include "msg.h"
#include "mutex.h"
#include "priority_queue.h"
#include "ringbuffer.h"
#include "rmutex.h"
#ifdef MODULE_CORE_THREAD_FLAGS
#include "thread_flags.h"
#endif
#include "thread.h"


int main(void)
{
    puts("Sizeof RIOT core types\n");

    puts("Type\t\t\tSize [bytes]");

    printf("sizeof(cib_t):                  %u\n", sizeof(cib_t));
    printf("sizeof(clist_node_t):           %u\n", sizeof(clist_node_t));
    printf("sizeof(core_panic_t):           %u\n", sizeof(core_panic_t));
    printf("sizeof(kernel_pid_t):           %u\n", sizeof(kernel_pid_t));
    printf("sizeof(list_node_t):            %u\n", sizeof(list_node_t));
    printf("sizeof(mbox_t):                 %u\n", sizeof(mbox_t));
    printf("sizeof(msg_t):                  %u\n", sizeof(msg_t));
    printf("sizeof(mutex_t):                %u\n", sizeof(mutex_t));
    printf("sizeof(priority_queue_node_t):  %u\n", sizeof(priority_queue_node_t));
    printf("sizeof(priority_queue_t):       %u\n", sizeof(priority_queue_t));
    printf("sizeof(ringbuffer_t):           %u\n", sizeof(ringbuffer_t));
    printf("sizeof(rmutex_t):               %u\n", sizeof(rmutex_t));
#ifdef MODULE_CORE_THREAD_FLAGS
    printf("sizeof(thread_flags_t):         %u\n", sizeof(thread_flags_t));
#else
    puts("sizeof(thread_flags_t):         0   (not enabled)");
#endif
    printf("sizeof(thread_t):               %u"
           "  (see test/sizeof_tcb for more details)\n",
           sizeof(thread_t));

    puts("\n[SUCCESS]");
    return 0;
}
