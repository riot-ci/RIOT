/*
 * Copyright (C) 2017 RWTH Aachen, Josua Arndt, Steffen Robertz
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
 * @brief       Test application for the tps6274x Step-Down converter
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */
#include <stdio.h>
#include "board.h"
#include "tps6274x.h"
#include "xtimer.h"

int main(void)
{
    puts("This application will test the tps6274x step down converter by switching through all voltages.");
    puts("Every voltage will be active for 3s and can be verified with a multimeter");
    tps6274x_init(1800);
    for (unsigned int voltage = 1800; voltage <= 3300; voltage += 100) {
        printf("%u mV \n", voltage);
        if (voltage != tps6274x_switch_voltage(voltage)) {
            printf("Not all Selector lines are connected in order to set a level of %umV.", voltage);
        }
        xtimer_sleep(1);
        puts("Load PIN will be enabled for 2s");
        tps6274x_load_ctrl(1);
        xtimer_sleep(2);
        puts("Load PIN will be shut off");
        tps6274x_load_ctrl(0);
    }
    printf("Test Done");
    return 0;
}
