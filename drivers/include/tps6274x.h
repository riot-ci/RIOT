/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_tps6274x TPS6274x
 * @ingroup     drivers_power
 * @brief       Device driver interface for the TPS6274x DC-DC Converter
 * @{
 *
 * @file
 * @brief       Device driver interface for the TPS6274x DC-DC Converter
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef TPS6274X_H
#define TPS6274X_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <inttypes.h>
#include "periph/gpio.h"

/**
 * @brief   TPS6274x Configuration struct
 */
typedef struct {
    gpio_t vsel[4];         /**< select line pin mapping */
    gpio_t ctrl_pin;        /**< ctrl pin  mapping */
} tps6274x_config_t;

/**
 * @brief   init converter
 *
 * @param[in] voltage   Voltage to set in mV (needs to be between 1.8V-3.3V
 * @return              set voltage in mV
 */
unsigned int tps6274x_init(unsigned int voltage);

/**
 * @brief   switch to different voltage level
 *
 * @param[in] voltage   Voltage to set in mV (needs to be between 1.8V-3.3V
 * @return              the voltage that was set in mV
 */
unsigned int tps6274x_switch_voltage(unsigned int voltage);

/**
 * @brief   sets ctrl pin high to power a subsystem connected on the load pin
 *
 * @param[in] status    0 will disable the load, everything else will activate it
 */
void tps6274x_load_ctrl(unsigned int status);

#ifdef __cplusplus
}
#endif
#endif /* TPS6274X_H */
/** @} */
