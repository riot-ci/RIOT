/*
 * Copyright (C)  2016 Freie Universität Berlin
 *                2016 Inria
 *                2017 Tom Keddie
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sparkfun-samd-mini
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie  <alexandre.abadie@inria.fr>
 * @author      Tom Keddie <github@bronwenandtom.com>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "arduino_pinmap.h"

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
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
