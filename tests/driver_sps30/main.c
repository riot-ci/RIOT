/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Test application for the Sensirion SPS30 device driver
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "xtimer.h"
#include "sps30.h"
#include "sps30_params.h"

#define POLL_FOR_READY_US (100000U)
#define MC_UNIT_STR  "[µg/m³]"
#define NC_UNIT_STR  "[#/cm³]"
#define TPS_UNIT_STR "[µm]"

static void _print_error(const char *msg, sps30_error_code_t ec)
{
    printf("%s_ERROR: %s\n", ec == SPS30_CRC_ERROR ? "CRC" : "I2C", msg);
}

int main(void)
{
    char str[SPS30_SER_ART_LEN];
    sps30_t dev;
    sps30_error_code_t ec;

    puts("SPS30 test application\n");

    ec = sps30_init(&dev, &sps30_params[0]);
    if (ec != SPS30_OK) {
        _print_error("init failed", ec);
        return 1;
    }
    else {
        puts("Initialization successful\n");
    }

    ec = sps30_read_article_code(&dev, str, sizeof(str));
    if (ec == SPS30_OK) {
        printf("Article code: %s\n", str);
    } else {
        _print_error("while reading article code", ec);
    }

    ec = sps30_read_serial_number(&dev, str, sizeof(str));
    if (ec == SPS30_OK) {
        printf("Serial: %s\n", str);
    } else {
        _print_error("while reading serial number", ec);
    }

    uint32_t ci = 0;
    ec = sps30_read_ac_interval(&dev, &ci);

    if (ec == SPS30_OK) {
        printf("Current auto-clean interval: %"PRIu32" seconds\n", ci);
    } else {
        _print_error("while reading article code", ec);
    }

    printf("Starting a cleaning cycle...\n");
    sps30_start_fan_clean(&dev);
    xtimer_usleep(SPS30_FAN_CLEAN_S * US_PER_SEC);

    ec = sps30_start_measurement(&dev);
    if (ec == SPS30_OK) {
        puts("started measurement: OK\n");
    } else {
        puts("started measurement: ERROR\n");
    }

    sps30_data_t data;
    while (1) {
        int err_code;
        bool ready = sps30_data_ready(&dev, &err_code);

        if (err_code != SPS30_OK) {
            _print_error("while reading ready flag", err_code);
        }

        if (!ready) {
            /* try again after some time */
            xtimer_usleep(POLL_FOR_READY_US);
            continue;
        }

        ec = sps30_read_measurement(&dev, &data);

        if (ec == SPS30_OK) {
            puts("v==== SPS30 measurements ====v");
            printf("| MC PM 1.0:   %2.3f "MC_UNIT_STR" |\n", data.mc_pm1);
            printf("| MC PM 2.5:   %2.3f "MC_UNIT_STR" |\n", data.mc_pm2_5);
            printf("| MC PM 4.0:   %2.3f "MC_UNIT_STR" |\n", data.mc_pm4);
            printf("| MC PM 10.0:  %2.3f "MC_UNIT_STR" |\n", data.mc_pm10);
            printf("| NC PM 0.5:   %2.3f "NC_UNIT_STR" |\n", data.nc_pm0_5);
            printf("| NC PM 1.0:   %2.3f "NC_UNIT_STR" |\n", data.nc_pm1);
            printf("| NC PM 2.5:   %2.3f "NC_UNIT_STR" |\n", data.nc_pm2_5);
            printf("| NC PM 4.0:   %2.3f "NC_UNIT_STR" |\n", data.nc_pm4);
            printf("| NC PM 10.0:  %2.3f "NC_UNIT_STR" |\n", data.nc_pm10);
            printf("| TPS:         %2.3f "TPS_UNIT_STR"    |\n", data.ps);
            puts("+----------------------------+");
            puts("| MC:  Mass Concentration    |");
            puts("| NC:  Number Concentration  |");
            puts("| TPS: Typical Particle Size |");
            puts("^============================^\n\n\n");
        } else {
            _print_error("while reading measurements", ec);
        }
    }

    return 0;
}
