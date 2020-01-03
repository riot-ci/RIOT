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
 * @brief       testing xtimer_now_usec function with irq disabled
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include "xtimer.h"
#include "irq.h"
#include "test_utils/interactive_sync.h"

#define TEST_COUNT 4

int main(void)
{
    test_utils_interactive_sync();
    puts("xtimer_now_irq test application.\n");

    while (TEST_COUNT) {
        unsigned int state = irq_disable();
        uint32_t t1 = xtimer_now_usec();
        xtimer_spin(xtimer_ticks_from_usec((uint32_t)(~XTIMER_MASK) / 2));
        uint32_t t2 = xtimer_now_usec();
        irq_restore(state);
        if (t2 < t1) {
            puts("Error");
            return -1;
        }
        puts("OK");
    }
    puts("SUCCESS");
    return 0;
}
