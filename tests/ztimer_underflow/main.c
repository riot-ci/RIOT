/*
 * Copyright (C) 2020 SSV Software Systems GmbH
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
 * @brief    ztimer test application
 *
 * @author   Juergen Fitschen <jfi@ssv-embedded.de>
 *
 * @}
 */

#include <stdio.h>

#include "ztimer.h"

#ifndef TEST_ZTIMER_CLOCK
#error Please define TEST_ZTIMER_CLOCK
#endif

#define xstr(s) str(s)
#define str(s) #s

int main(void)
{
    unsigned i = 0;

    printf("Looping over ztimer clock %s\n", xstr(TEST_ZTIMER_CLOCK));
    while (true) {
        printf("Loop %u\n", i++);
        ztimer_sleep(TEST_ZTIMER_CLOCK, 0);
    }

    return 0;
}
