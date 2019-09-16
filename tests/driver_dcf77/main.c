/*
 * Copyright (C) 2019 Michel Gerlach
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
 * The test application demonstrates the use of the DCF77 using
 *
 *
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "xtimer.h"

#include "dcf77_params.h"
#include "dcf77.h"
int main(void)
{
    dcf77_t sensor;
    uint8_t minute;
    uint8_t hour;
    uint8_t weekday;
    uint8_t calenderday;
    uint8_t month;
    uint8_t year;
    uint8_t mesz;


    printf("DCF77 test application\n");

    /* initialize the sensor with default configuration parameters */
    if (dcf77_init (&sensor, &dcf77_params[0]) != DCF77_OK) {
        puts("Initialization failed\n");
        return -1;
    }
    printf("DCF77 Module initialized \n");

while(1){
    gpio_init(sensor.params.pin, sensor.params.in_mode);

    printf("\n+--------Starting Measurements--------+\n");
    if(dcf77_read(&sensor, &minute, &hour, &weekday,
      &calenderday,&month, &year, &mesz)!= DCF77_OK){
      puts("###Error### Poor reception...? Cables checked...?");
    }else{
      printf("Received Minutes: %d\n",minute);
      printf("Received Hours: %d\n",hour);
      printf("Received Weekday: %d\n",weekday);
      printf("Received Calenderday: %d\n",calenderday);
      printf("Received Month: %d\n",month);
      printf("Received Year: %d\n",year);
      printf("Received mesz: %d\n",mesz );
    }
    }


    return 0;
}
