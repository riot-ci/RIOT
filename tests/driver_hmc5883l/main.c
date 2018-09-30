/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the HMC5883L 3-axis digital compass
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application demonstrates the use of the HMC5883L. It uses the
 * default configuration parameters.
 *
 * - Continuous measurement at a Data Output Rate (DOR) of 15 Hz
 * - Normal mode, no biasing
 * - Gain 1090 LSb/Gs
 * - No averaging of data samples
 *
 * The application can use the different approaches to get new data:
 *
 * - using the #hmc5883l_read function at a lower rate than the the DOR
 * - using the data-ready interrupt (**DRDY**)
 *
 * The data-ready interrupt (**DRDY) of the sensor is enabled permanently.
 * The application has only to configure and initialize the GPIO to which the
 * interrupt signal is connected. This is done by defining
 * ```USE_HMC5883L_DRDY``` and overrding the default configuration
 * parameter ```HMC5883L_PARAM_DRDY``` if necessary, for example:
 *
 * ```
 * CFLAGS="-DUSE_HMC5883L_DRDY -DHMC5883L_PARAM_DRDY=GPIO12" \
 * make flash -C tests/driver_hmc5883l BOARD=...
 * ```
*/

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

#include "hmc5883l.h"
#include "hmc5883l_params.h"

#define SLEEP   (100 * US_PER_MS)

kernel_pid_t p_main;

#if USE_HMC5883L_DRDY
static void hmc5883l_isr_data_ready (void *arg)
{
    (void)arg;
    /* send a message to trigger main thread to handle the interrupt */
    msg_t msg;
    msg_send(&msg, p_main);
}
#endif


int main(void)
{
    hmc5883l_t dev;

    p_main = sched_active_pid;

    puts("HMC5883L magnetometer driver test application\n");
    puts("Initializing HMC5883L sensor");

    /* initialize the sensor with default configuration parameters */
    if (hmc5883l_init(&dev, &hmc5883l_params[0]) == HMC5883L_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    #if USE_HMC5883L_DRDY
    /* init INT2/DRDY signal pin and enable the interrupt */
    gpio_init_int(hmc5883l_params[0].drdy, GPIO_IN, GPIO_FALLING,
                  hmc5883l_isr_data_ready, 0);
    #endif /* USE_HMC5883L_DRDY */

    while (1) {
        #if USE_HMC5883L_DRDY
        /* wait for data ready interrupt */
        msg_t msg;
        msg_receive(&msg);
        #else
        /* wait longer than period of HMC5883L DOR */
        xtimer_usleep(SLEEP);
        #endif

        /* read and print data in any case */
        hmc5883l_data_t data;
        if (hmc5883l_read(&dev, &data) == HMC5883L_OK) {
            printf("mag [uGs] x: %+8" PRIi32 " y: %+8" PRIi32 " z: %+8" PRIi32 "\n",
                    data.x, data.y, data.z);
        }
    }

    return 0;
}
