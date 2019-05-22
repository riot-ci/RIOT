/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Low-level test for backup RAM
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>

#include "periph/rtc.h"
#include "pm_layered.h"
#include "xtimer.h"

#ifndef SLEEP_SEC
#define SLEEP_SEC 1
#endif

int main(void)
{
    /* We keep a copy of counter in the .noinit section (normal RAM).
     * In the deepest sleep normal RAM is not retained, so if it matches
     * counter anyway after wakeup, we did not sleep properly.
     */
    static int counter_noinit __attribute__((section(".noinit")));
    static int counter __attribute__((section(".backup.bss")));

    if (counter == 0) {
        puts("\nBackup RAM test\n");
        printf("This test will increment the counter by 1, "
               "then enter deep sleep for %ds\n", SLEEP_SEC);
    } else if (counter_noinit == counter) {
        puts("WARNING: non-backup memory retained - did we really enter deep sleep?");
    }

    printf("counter: %d\n", ++counter);
    counter_noinit = counter;

    /* Some tools have trouble flashing MCUs in deep sleep.
     * Wait a bit to make re-flashing / debugging easier.
     */
    xtimer_sleep(3);

#ifndef CPU_BACKUP_RAM_NOT_RETAINED

    /* schedule RTC wake in SLEEP_SEC s */
    struct tm time;
    rtc_get_time(&time);
    time.tm_sec += SLEEP_SEC;
    mktime(&time);
    rtc_set_alarm(&time, NULL, NULL);

    /* put the device in deep sleep */
    pm_set(0);

    puts("would sleep now - YOU SHOULD NOT SEE THIS!");

#else
    puts("low power RAM not retained during sleep - aborting test.");
#endif

    return 0;
}
