/*
 * Copyright (C) 2021 Inria
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
 * @brief       HM3301 driver test application
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Francisco Molina <francois-xavier.molinas@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "fmt.h"
#include "ztimer.h"
#include "timex.h"

#include "hm3301.h"
#include "hm3301_params.h"

static const char spaces[16] = "                ";

static void print_col_u32_dec(uint32_t number, size_t width)
{
    char sbuf[10]; /* "4294967295" */
    size_t slen;

    slen = fmt_u32_dec(sbuf, number);
    if (width > slen) {
        width -= slen;
        while (width > sizeof(spaces)) {
            print(spaces, sizeof(spaces));
        }
        print(spaces, width);
    }
    print(sbuf, slen);
}

int main(void)
{
    hm3301_t dev;

    print_str("HM3301 test application\n");

    print_str("+------------Initializing------------+\n");

    /* initialize the sensor with default configuration parameters */
    if (hm3301_init(&dev, &hm3301_params[0])) {
        print_str("Initialization failed\n");
        return 1;
    }

    print_str(
        "+------------------------+------------------------+----------------------------------------------+\n"
        "| Standard concentration | Atmospheric Environment|   # Particles in 0.1l air of diameter >=     |\n"
        "| PM1.0 | PM2.5 | PM10.0 | PM1.0 | PM2.5 | PM10.0 | 0.3µm | 0.5µm | 1.0µm | 2.5µm | 5.0µm | 10µm |\n"
        "+-------+-------+--------+-------+-------+--------+-------+-------+-------+-------+-------+------+\n"
    );

    hm3301_data_t data;
    while (1) {
        ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

        /* read the data and print them on success */
        if (hm3301_read(&dev, &data) == 0) {
            print("|", 1);
            print_col_u32_dec(data.mc_pm_1, 7);
            print("|", 1);
            print_col_u32_dec(data.mc_pm_2p5, 7);
            print("|", 1);
            print_col_u32_dec(data.mc_pm_10, 8);
            print("|", 1);
            print_col_u32_dec(data.amc_pm_1, 7);
            print("|", 1);
            print_col_u32_dec(data.amc_pm_2p5, 7);
            print("|", 1);
            print_col_u32_dec(data.amc_pm_10, 8);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_0p3, 7);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_0p5, 7);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_1, 7);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_2p5, 7);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_5, 7);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_10, 6);
            print("|\n", 2);
        }
        else {
            printf("Could not read data from sensor\n");
        }
    }

    return 0;
}
