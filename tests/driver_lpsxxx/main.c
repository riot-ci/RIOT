/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2018 Inria
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
 * @brief       Test application for the LPS331AP/LPS25HB pressure sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "lpsxxx.h"
#include "lpsxxx_params.h"

int main(void)
{
    lpsxxx_t dev;

    puts("Test application for %s pressure sensor\n\n", LPSXXX_SAUL_NAME);
    printf("Initializing %s sensor\n", LPSXXX_SAUL_NAME);
    if (lpsxxx_init(&dev, &lpsxxx_params[0]) != LPSXXX_OK) {
        puts("Initialization failed");
        return 1;
    }

    uint16_t pres;
    int16_t temp;
    while (1) {
        lpsxxx_enable(&dev);
        xtimer_sleep(1); /* wait a bit for the measurements to complete */

        lpsxxx_read_temp(&dev, &temp);
        lpsxxx_read_pres(&dev, &pres);
        lpsxxx_disable(&dev);

        int pres_abs = pres / 1000;
        pres -= pres_abs * 1000;
        int temp_abs = temp / 100;
        temp -= temp_abs * 100;

        printf("Pressure value: %2i.%03i bar - Temperature: %2i.%02i °C\n",
               pres_abs, pres, temp_abs, temp);

        xtimer_sleep(1);
    }

    return 0;
}
