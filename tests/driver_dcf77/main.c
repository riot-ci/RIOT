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
#include <inttypes.h>
#include "thread.h"
#include "xtimer.h"

#include "dcf77_params.h"
#include "dcf77.h"
int main(void)
{
    dcf77_t sensor;
    struct tm time={0};
    char outstr[200];

    printf("DCF77 test application\n");

    /* initialize the sensor with default configuration parameters */
    if (dcf77_init (&sensor, &dcf77_params[0]) != DCF77_OK) {
        puts("Initialization failed\n");
        return -1;
    }
    printf("DCF77 Module initialized \n");

while(1){
    printf("Wait for a complete cycle... \n");
    dcf77_read(&sensor,&time);
    strftime(outstr, sizeof(outstr), "%x - %I:%M%p", &time);
    strftime(outstr, sizeof(outstr), "%c", &time);
    printf("Formatted date & time : |%s|\n", outstr);

    }
    return 0;
}
