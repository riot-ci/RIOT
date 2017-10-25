/*
 * Copyright (C) 2017 RWTH Aachen, Josua Arndt, Steffen Robertz
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
 * @brief       test aplication for the LC709203F Battery fuel gauge by on semiconductors
 *
 * @author	Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author	Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include "board.h"
#include "periph_conf.h"
#include "lc709203f.h"
#include "xtimer.h"

#ifndef LC709203F_INT_PIN
    #define LC709203F_INT_PIN GPIO_PIN(PORT_E, 6)
#endif

static void _gauge_cb(void *arg)
{
    printf("\n ALARM: Low RSOC \n");
}

int main(void)
{
    puts("LC709203F Fuel Gauge test application");
    lc709203f_init(I2C_0, LC709203F_INT_PIN, _gauge_cb, NULL);

    puts("The application will trigger an low_rsoc interrupt in 2s");
    xtimer_sleep(2);
    lc709203f_set_alarm_low_rsoc(I2C_0, 0);
    lc709203f_set_alarm_low_rsoc(I2C_0, 100);
    puts("Read low rsoc threshold level. Should be 100");
    printf("Alarm Low RSOC level: %u \n", lc709203f_get_alarm_low_rsoc(I2C_0));
    puts("Voltage, RSOC, ITE, ID and Cell temperature will be printed every 5s");
    while (1) {
        xtimer_sleep(5);
        printf("Voltage in mV: %u \n", lc709203f_get_voltage(I2C_0));
        printf("RSOC: %u \n", lc709203f_get_rsoc(I2C_0));
        printf("Ite in 0.1: %u \n", lc709203f_get_ite(I2C_0));
        printf("ID: %u \n", lc709203f_get_id(I2C_0));
        printf("Cell Temp in 0.1C: %u \n", lc709203f_get_cell_temp(I2C_0));
    }
    return 0;
}
