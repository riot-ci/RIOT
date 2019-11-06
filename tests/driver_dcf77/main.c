/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the DCF77 device driver
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include "xtimer.h"

#include "dcf77_params.h"
#include "dcf77.h"
int main(void)
{
    dcf77_t sensor;
    struct tm time;

    printf("DCF77 test application\n");

    /* initialize the sensor with default configuration parameters */
    if (dcf77_init (&sensor, &dcf77_params[0]) != DCF77_OK) {
        puts("Initialization failed\n");
        return -1;
    }
    memset(&time, 0, sizeof(time));
    printf("DCF77 Module initialized \n");

    while (1) {
        dcf77_get_time(&sensor, &time);
        printf("%d %d.%d.%d %d:%d", time.tm_wday, time.tm_mday,
               (time.tm_mon + 1), (time.tm_year + 1900), time.tm_hour,
               time.tm_min);
        xtimer_sleep(20);
    }
    return 0;
}
