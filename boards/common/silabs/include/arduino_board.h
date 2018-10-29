/*
 * Copyright (C)  2018 Federico Pellegrin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_silabs
 * @brief       SiLabs Boards configuration for the Arduino API
 * @file
 * @author      Federico Pellegrin <fede@evolware.org>
 * @{
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "periph_cpu.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Arduino's digital pins mappings
 */
static const gpio_t arduino_pinmap[] = {
    GPIO_PIN(PE, 2),  // LED 0
    GPIO_PIN(PE, 3),  // LED 1
    GPIO_PIN(PB, 9),  // PB0
    GPIO_PIN(PB, 10), // PB1
};

/**
 * @brief   Arduino's analog pins mappings
 */
static const adc_t arduino_analog_map[] = {
    ADC_LINE(0)
};

/**
 * @brief   On-board LED mapping
 */
#define ARDUINO_LED         (0)

/**
 * @brief   On-board serial port mapping
 */
#define ARDUINO_UART_DEV         UART_DEV(0)


#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
