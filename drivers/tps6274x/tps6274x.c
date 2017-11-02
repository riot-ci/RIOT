/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tps6274x
 * @{
 *
 * @file
 * @brief       Device driver implementation for the TPS6274x family DC/DC-converter.
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */

#include "tps6274x.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef TPS6274X_CONFIG
const tps6274x_config_t converter_config = TPS6274X_CONFIG;
#else
/* Default device parameters are undefined */
#error "No config struct for module tps6274x available"
#endif



unsigned int tps6274x_init(unsigned int voltage)
{
    for (uint8_t i = 0; i < 4; i++) {
        if (converter_config.vsel[i] != GPIO_UNDEF) {
            gpio_init(converter_config.vsel[i], GPIO_OUT);
        }
    }
    if (converter_config.ctrl_pin != GPIO_UNDEF) {
        gpio_init(converter_config.ctrl_pin, GPIO_OUT);
    }
    return tps6274x_switch_voltage(voltage);
}

unsigned int tps6274x_switch_voltage(unsigned int voltage)
{
    if (voltage < 1800) {
        voltage = 1800;
    }
    else if (voltage > 3300) {
        voltage = 3300;
    }
    uint8_t vsel = (voltage - 1800) / 100;
    uint8_t vsel_set = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (converter_config.vsel[i] != GPIO_UNDEF) {
            gpio_write(converter_config.vsel[i], (vsel & (0x01 << i)));
            /* mark pins that could and had to be set */
            vsel_set |= vsel & (1 << i);
        }
        else {
            DEBUG("[tps6274x] Pin vsel%u is not connected but is required for selected voltage level\n", i + 1);
        }
    }
    return ((unsigned int)vsel_set) * 100 + 1800;
}

void tps6274x_load_ctrl(unsigned int status)
{
    if (converter_config.ctrl_pin != GPIO_UNDEF) {
        gpio_write(converter_config.ctrl_pin, status);
    }
    else {
        DEBUG("[TPS6274x] CTRL Pin not defined, no load activation possible\n");
    }
}
