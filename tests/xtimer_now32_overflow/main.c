/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdio.h>

#include "fmt.h"
#include "msg.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "xtimer.h"

#define MAIN_MSG_QUEUE_SIZE     (4U)

msg_t _main_msg_queue[MAIN_MSG_QUEUE_SIZE];

int main(void)
{
    xtimer_t timer1, timer2;
    msg_t msg1 = { .content = { .value = 1U } };
    msg_t msg2 = { .content = { .value = 2U } };

    msg_init_queue(_main_msg_queue, MAIN_MSG_QUEUE_SIZE);
    /* ensure that xtimer_now64() is greater than UINT32_MAX */
    _xtimer_current_time = (1LLU << 32U);
    xtimer_init();
    expect(xtimer_now64().ticks64 > UINT32_MAX);
    print_str("Setting 2 timers:\n");
    print_str(" #1 in 1 sec\n");
    print_str(" #2 in 3 sec\n\n");
    xtimer_set_msg64(&timer1, 1 * US_PER_SEC, &msg1, thread_getpid());
    xtimer_set_msg64(&timer2, 3 * US_PER_SEC, &msg2, thread_getpid());
    print_str("now=");
    print_u64_dec(xtimer_now64().ticks64);
    print_str("\n");
    expect(timer1.long_start_time > 0);
    expect(timer2.long_start_time > 0);
    while (1) {
        msg_t msg;

        msg_receive(&msg);
        print_str("#");
        print_u32_dec(msg.content.value);
        print_str(":now=");
        print_u64_dec((uint64_t)xtimer_now64().ticks64);
        print_str("\n");
        switch (msg.content.value) {
            case 1U:
                /* timer1 expired */
                expect(timer1.long_start_time == 0);
                /* upper half of timer2's start_time stays above 1 as it is
                 * based on xtimer_now64() during timer1's callback execution */
                expect(timer2.long_start_time > 0);
                break;
            case 2U:
                /* timer1 expired */
                expect(timer1.long_start_time == 0);
                expect(timer2.long_start_time == 0);
                break;
            default:
                break;
        }
    }
    return 0;
}

/** @} */
