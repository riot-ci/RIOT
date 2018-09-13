/*
 * Copyright (C)  2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_wemos-lolin-d32-pro
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The on-board LED is connected to pin 2 on this board
 */
#define ARDUINO_LED         (10)

/**
 * @brief   Look-up table for the Arduino's digital pins
 */
static const gpio_t arduino_pinmap[] = {
    GPIO3,      /* ARDUINO_PIN_0 (RxD) */
    GPIO1,      /* ARDUINO_PIN_1 (TxD) */

    GPIO4,      /* ARDUINO_PIN_2 */
    GPIO0,      /* ARDUINO_PIN_3 (PWM) */
    GPIO13,     /* ARDUINO_PIN_4 */
    GPIO2,      /* ARDUINO_PIN_5 (PWM) */
    GPIO15,     /* ARDUINO_PIN_6 (PWM) */
    GPIO25,     /* ARDUINO_PIN_7 */
    GPIO26,     /* ARDUINO_PIN_8 */
    GPIO32,     /* ARDUINO_PIN_9 (PWM) */

    GPIO5,      /* ARDUINO_PIN_10 (CS0 / PWM)  */
    GPIO23,     /* ARDUINO_PIN_11 (MOSI / PWM) */
    GPIO19,     /* ARDUINO_PIN_12 (MISO) */
    GPIO18,     /* ARDUINO_PIN_13 (SCK)  */

    GPIO36,     /* ARDUINO_PIN_A0 */
    GPIO39,     /* ARDUINO_PIN_A1 */
    GPIO34,     /* ARDUINO_PIN_A2 */
    GPIO35,     /* ARDUINO_PIN_A3 */

    GPIO21,     /* ARDUINO_PIN_A4 (SDA) */
    GPIO22,     /* ARDUINO_PIN_A5 (SCL) */
};

/**
 * @brief   Look-up table for the Arduino's analog pins
 */
static const adc_t arduino_analog_map[] = {
    GPIO36,     /* ARDUINO_PIN_A0 */
    GPIO39,     /* ARDUINO_PIN_A1 */
    GPIO34,     /* ARDUINO_PIN_A2 */
    GPIO35,     /* ARDUINO_PIN_A3 */

    GPIO27,     /* ARDUINO_PIN_A4 (SDA) */
    GPIO33,     /* ARDUINO_PIN_A5 (SCL) */
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
