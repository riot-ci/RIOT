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
    dcf77_data_t data;


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
    if(dcf77_read(&sensor, &data)!= DCF77_OK){
      puts("###Error### Poor reception...? Cables checked...?");
    }else{
      printf("Received Minutes: %d\n",data.minute);
      printf("Received Hours: %d\n",data.hour);
      printf("Received Weekday: %d\n",data.weekday);
      printf("Received Calenderday: %d\n",data.calenderday);
      printf("Received Month: %d\n",data.month);
      printf("Received Year: %d\n",data.year);
      printf("Received mesz: %d\n",data.mesz );
    }
    data.minute=0;
    data.hour=0;
    data.weekday=0;
    data.calenderday=0;
    data.month=0;
    data.year=0;
    data.mesz=0;
    }


    return 0;
}
