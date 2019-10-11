/*
 * Copyright (C) 2019 Freie Universität Berlin,
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
 * @brief       Thread test application
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include "thread.h"
#include "ps.h"
#define TEST_THREAD_STACKSIZE ((2 * THREAD_STACKSIZE_IDLE) + \
                               THREAD_EXTRA_STACKSIZE_PRINTF)

/* stacks for testing threads */
static char t2_stack[TEST_THREAD_STACKSIZE];
static char t3_stack[TEST_THREAD_STACKSIZE];
static char t4_stack[TEST_THREAD_STACKSIZE];

/* function for testing threads */
void *second_thread(void *arg)
{
    (void)arg;
    puts("other thread\n");
    thread_zombify();
    puts("ERROR zombie runs again");
    return NULL;
}

int main(void)
{

    /* save thread count on test start */
    int current_thread_count;
    int start_thread_count = sched_num_threads;

    printf("start threads = %d\n", start_thread_count);

    /* creating threads for testing */
    kernel_pid_t first_pid =  thread_create(
        t2_stack, sizeof(t2_stack),
        THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_SLEEPING | THREAD_CREATE_STACKTEST,
        second_thread, NULL, "nr2");

    ps();
    puts("");

    kernel_pid_t second_pid =  thread_create(
        t3_stack, sizeof(t3_stack),
        THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_SLEEPING | THREAD_CREATE_STACKTEST,
        second_thread, NULL, "nr3");
    ps();
    puts("");

    kernel_pid_t third_pid =  thread_create(
        t4_stack, sizeof(t4_stack),
        THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_SLEEPING | THREAD_CREATE_STACKTEST,
        second_thread, NULL, "nr4");
    ps();
    puts("");
    current_thread_count = sched_num_threads;
    printf("threads = %d\n", current_thread_count);

    /* check if all threads got created */
    if (start_thread_count + 3 != current_thread_count) {
        puts("Error wrong thread count");
    }
    else {
        puts("OK");
    }

    /* wakeup first thread and check if the thread zombified itself */
    thread_wakeup(first_pid);
    ps();
    puts("");
    current_thread_count = sched_num_threads;
    printf("threads = %d\n", current_thread_count);
    if (thread_getstatus(first_pid) != STATUS_ZOMBIE) {
        puts("Error Wrong Status: first thread is not a zombie!");
    }
    else {
        if (start_thread_count + 3 != current_thread_count) {
            puts("Error wrong thread count");
        }
        else {
            puts("OK");
        }

    }



    /* wakeup second thread and check if the thread zombified itself */
    thread_wakeup(second_pid);
    ps();
    puts("");
    current_thread_count = sched_num_threads;
    printf("threads = %d\n", current_thread_count);

    if (thread_getstatus(first_pid) != STATUS_ZOMBIE) {
        puts("Error Wrong Status: first thread is not a zombie!");
    }
    else {
        if (thread_getstatus(second_pid) != STATUS_ZOMBIE) {
            puts("Error Wrong Status: second thread is not a zombie!");
        }
        else {
            if (start_thread_count + 3 != current_thread_count) {
                puts("Error wrong thread count");
            }
            else {
                puts("OK");
            }

        }
    }

    /* kill first zombie thread and check if the thread still exist (it should not) */
    if (thread_kill_zombie(first_pid) != 1){
        puts("Error thread_kill_zombie returned an error");
    }
    ps();
    puts("");
    current_thread_count = sched_num_threads;
    printf("threads = %d\n", current_thread_count);

    if (thread_getstatus(first_pid) != STATUS_NOT_FOUND) {
        puts("Error first Thread does still exist");
    }
    else {
        if (thread_getstatus(second_pid) != STATUS_ZOMBIE) {
            puts("Error Wrong Status: second thread is not a zombie!");
        }
        else {
            if (start_thread_count + 2 != current_thread_count) {
                puts("Error wrong thread count");
            }
            else {
                puts("OK");
            }
        }
    }

    /* wakeup third thread and check if the thread zombified itself */
    thread_wakeup(third_pid);
    ps();
    puts("");
    if (thread_getstatus(first_pid) != STATUS_NOT_FOUND) {
        puts("Error first Thread does still exist");
    }
    else {
        if (thread_getstatus(second_pid) != STATUS_ZOMBIE) {
            puts("Error Wrong Status: third thread is not a zombie!");
        }
        else {
            if (thread_getstatus(third_pid) != STATUS_ZOMBIE) {
                puts("Error Wrong Status: second thread is not a zombie!");
            }
            else {
                if (start_thread_count + 2 != current_thread_count) {
                    puts("Error wrong thread count");
                }
                else {
                    puts("OK");
                }
            }
        }
    }

    /* check if thread are still beeing created normally */
    kernel_pid_t last_pid =  thread_create(
        t2_stack, sizeof(t2_stack),
        THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_SLEEPING | THREAD_CREATE_STACKTEST,
        second_thread, NULL, "nr2");
    ps();
    puts("");
    if (thread_getstatus(last_pid) != STATUS_SLEEPING) {
        puts("Error last Thread is not sleeping");
    }
    else {
        if (last_pid != first_pid) {
            puts("Error thread did not reuse first thread pid");
        }
        else {
            if (thread_getstatus(second_pid) != STATUS_ZOMBIE) {
                puts("Error Wrong Status: third thread is not a zombie!");
            }
            else {
                if (thread_getstatus(third_pid) != STATUS_ZOMBIE) {
                    puts("Error Wrong Status: second thread is not a zombie!");
                }
                else {
                    if (start_thread_count + 2 != current_thread_count) {
                        puts("Error wrong thread count");
                    }
                    else {
                        puts("OK");
                    }
                }
            }
        }
    }

    return 0;
}
