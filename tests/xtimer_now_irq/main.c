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
 * @brief       testing xtimer_mutex_lock_timeout function
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include "xtimer.h"
#include "irq.h"

int main(void)
{
    puts("xtimer_now_irq test application.\n");

    while (true) {
        unsigned int state = irq_disable();
        uint32_t t1 = xtimer_now_usec();
        xtimer_spin(xtimer_ticks_from_usec(XTIMER_BACKOFF));
        uint32_t t2 = xtimer_now_usec();
        irq_restore(state);
        if (t2 < t1) {
            printf("Error\n");
        }
    }
    return 0;
}
