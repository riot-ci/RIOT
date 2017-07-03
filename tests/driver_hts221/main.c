/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
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
 * @brief       Test application for the HDC1000 sensor driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdio.h>

#include "hts221.h"
#include "hts221_params.h"
#include "xtimer.h"

#define SLEEP       (1000 * 1000U)
#define WAIT        (100U * US_PER_MS)

int main(void)
{
    hts221_t dev;

    puts("HDC1000 Temperature and Humidity Sensor driver test application\n");
    printf("Initializing HTS221 sensor at I2C_DEV(%i) ... ",
            (int)hts221_params[0].i2c);
    if (hts221_init(&dev, &hts221_params[0]) != HTS221_OK) {
        puts("[FAILED]");
        return 1;
    }
    puts("[OK]\n");
    if (hts221_power_on(&dev) != HTS221_OK) {
        puts("[FAILED] to set power on!");
        return 2;
    }
    /*
    if (hts221_reboot(&dev) != HTS221_OK) {
        puts("[FAILED] reboot!");
        return 2;
    }
    */
    /*
    if (hts221_one_shot(&dev) != HTS221_OK) {
        puts("[FAILED] one shot!");
        return 3;
    }
    puts("[OK] one shot.");
    */
    if (hts221_set_rate(&dev, dev.p.rate) != HTS221_OK) {
        puts("[FAILED] set rate for continuous mode!");
        return 3;
    }
    puts("[OK] set continuous.");

    while(1) {
        int state = 0;

        do {
            state = hts221_get_state(&dev);
            xtimer_usleep(WAIT);
        }
        while (!state);
        printf("[SUCCESS] state=%x\n", state);
        uint16_t hum = 0;
        int16_t temp = 0;
        if (hts221_read_humidity(&dev, &hum) != HTS221_OK) {
            puts(" -- failed to humidity!");
        }
        if (hts221_read_temperature(&dev, &temp) != HTS221_OK) {
            puts(" -- failed to temperature!");
        }
        printf("H: %u, T: %i\n", hum, temp);
        xtimer_sleep(2);
    }
    return 0;
}
