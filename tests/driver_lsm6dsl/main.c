/*
 * Copyright (C) 2017 OTA keys S.A.
 *               2017 HAW Hamburg
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
 * @brief       Test application for the LSM6DSL accelerometer/gyroscope driver.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "xtimer.h"
#include "lsm6dsl.h"
#include "lsm6dsl_params.h"
#include "shell.h"

#define SLEEP       (500UL * US_PER_MS)

static lsm6dsl_t dev;

static int _test(int argc, char **argv)
{
    int16_t temp_value;
    lsm6dsl_3d_data_t mag_value;
    lsm6dsl_3d_data_t acc_value;

    bool infinite = false;
    int nb = 10;
    int res = 0;

    if (argc > 1) {
        nb = atoi(argv[1]);
        if (!nb) {
            infinite = true;
        }
    }

    while (infinite || nb--) {
        if (lsm6dsl_read_acc(&dev, &acc_value) == LSM6DSL_OK) {
            printf("Accelerometer x: %i y: %i z: %i\n", acc_value.x,
                                                        acc_value.y,
                                                        acc_value.z);
        }
        else {
            puts("[ERROR] reading accelerometer!\n");
            res = 1;
        }

        if (lsm6dsl_read_gyro(&dev, &mag_value) == LSM6DSL_OK) {
            printf("Gyroscope x: %i y: %i z: %i\n", mag_value.x,
                                                    mag_value.y,
                                                    mag_value.z);
        }
        else {
            puts("[ERROR] reading gyroscope!\n");
            res = 1;
        }

        if (lsm6dsl_read_temp(&dev, &temp_value) == LSM6DSL_OK) {
            printf("Temperature [in °C x 100]: %i \n", temp_value);
        }
        else {
            puts("[ERROR] reading temperature!\n");
            res = 1;
        }

        puts("");
        xtimer_usleep(SLEEP);
    }

    return res;
}

static int _power_down(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int res = 0;

    if (lsm6dsl_acc_power_down(&dev) == LSM6DSL_OK) {
        puts("Accelerometer powered down");
    }
    else {
        puts("[ERROR] powering down accelerometer");
        res = 1;
    }

    if (lsm6dsl_gyro_power_down(&dev) == LSM6DSL_OK) {
        puts("Gyroscope powered down");
    }
    else {
        puts("[ERROR] powering down gyroscope");
        res = 1;
    }

    return res;
}

static int _power_up(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int res = 0;

    if (lsm6dsl_acc_power_up(&dev) == LSM6DSL_OK) {
        puts("Accelerometer powered up");
    }
    else {
        puts("[ERROR] powering up accelerometer");
        res = 1;
    }

    if (lsm6dsl_gyro_power_up(&dev) == LSM6DSL_OK) {
        puts("Gyroscope powered up");
    }
    else {
        puts("[ERROR] powering up gyroscope");
        res = 1;
    }

    return res;
}

static const shell_command_t commands[] = {
    { "test", "Test LSM6DSL", _test },
    { "power_down", "Power down LSM6DSL", _power_down },
    { "power_up", "Power up LSM6DSL", _power_up },
    { NULL, NULL, NULL },
};

int main(void)
{
    puts("LSM6DSL test application");
    printf("Initializing LSM6DSL sensor at I2C_%i... ", lsm6dsl_params->i2c);

    if (lsm6dsl_init(&dev, lsm6dsl_params) != LSM6DSL_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    puts("Usage:");
    puts("test [nb]");
    puts("  launch reading test nb times\n"
         "  (default value = 10, infinite test if nb == 0)");
    puts("power_down");
    puts("  power down the accelerometer and the gyroscope");
    puts("power_up");
    puts("  power up the accelerometer and the gyroscope");

    char line[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line, sizeof(line));

    return 0;
}
