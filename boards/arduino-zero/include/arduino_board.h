/*
 * Copyright (C)  2016 Freie Universität Berlin
 *                2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-zero
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie  <alexandre.abadie@inria.fr>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "arduino_pinmap.h"
#include "periph/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The on-board LED is connected to pin 13 on this board
 */
#define ARDUINO_LED         (13)

/**
 * @brief   Look-up table for the Arduino's digital pins
 */
static const gpio_t arduino_pinmap[] = {
    ARDUINO_PIN_0,
    ARDUINO_PIN_1,
    ARDUINO_PIN_2,
    ARDUINO_PIN_3,
    ARDUINO_PIN_4,
    ARDUINO_PIN_5,
    ARDUINO_PIN_6,
    ARDUINO_PIN_7,
    ARDUINO_PIN_8,
    ARDUINO_PIN_9,
    ARDUINO_PIN_10,
    ARDUINO_PIN_11,
    ARDUINO_PIN_12,
    ARDUINO_PIN_13,
    ARDUINO_PIN_A0,
    ARDUINO_PIN_A1,
    ARDUINO_PIN_A2,
    ARDUINO_PIN_A3,
    ARDUINO_PIN_A4,
    ARDUINO_PIN_A5,
};

/**
 * @brief   Look-up table for the Arduino's analog pins
 */
static const adc_t arduino_analog_map[] = {
    ARDUINO_A0,
    ARDUINO_A1,
    ARDUINO_A2,
    ARDUINO_A3,
    ARDUINO_A4,
    ARDUINO_A5,
};

/**
 * @brief   RIOT GPIO mapping between Arduino pin, PWM device and channel
 */
typedef struct {
    int pin;        /**< Arduino pin number */
    int dev;        /**< PWM device index of pin */
    int chan;       /**< PWM channel index */
} arduino_pwm_t;

/**
 * @brief   List of PWM GPIO mappings
 */
static const arduino_pwm_t arduino_pwm_list[] = {
    { .pin = 3, .dev = PWM_DEV(0), .chan = 1 },
    { .pin = 4, .dev = PWM_DEV(0), .chan = 0 },
    { .pin = 8, .dev = PWM_DEV(1), .chan = 0 },
    { .pin = 9, .dev = PWM_DEV(1), .chan = 1 },
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
