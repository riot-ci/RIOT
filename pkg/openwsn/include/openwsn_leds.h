/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       Provides an adaption of OpenWSN led handling
 *              to RIOTs handling of LEDs and/or GPIOs
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */
#ifndef __OPENWSN_LEDS_H
#define __OPENWSN_LEDS_H

#include "periph/gpio.h"

/**
 * Holds a configuration of LED pins for debugging OpenWSN
 */
typedef struct ledpins_config {
    gpio_t error;
    gpio_t sync;
    gpio_t radio;
    gpio_t debug;
    uint8_t led_on;
} ledpins_config_t;

/**
 * @brief   Led on state values
 */
enum {
    GPIO_LED_LOW = 0,
    GPIO_LED_HIGH
};

/**
 * Sets the led pins for a specific board for OpenWSN
 *
 * @param[in] user_config A configuration of GPIO pins used for debugging.
 *
 * @note      Unused pins need to be defined as GPIO_UNDEF
 */
void ledpins_riot_init(const ledpins_config_t *user_config);

#endif /* __OPENWSN_LEDS_H */
