/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_microbit
 * @{
 *
 * @file
 * @brief       BBC micro:bit specific LED handling
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef MICROBIT_H
#define MICROBIT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of rows of the LED matrix
 */
#define MICROBIT_MATRIX_ROWS    (5U)

/**
 * @brief   Number of columns of the LED matrix
 */
#define MICROBIT_MATRIX_COLS    (5U)

/**
 * @brief   Initialize the micro:bit's LED matrix
 */
void microbit_matrix_init(void);

/**
 * @brief   Turn on a single LED in the LED matrix
 *
 * @param[in] row       row of the LED
 * @param[in] col       column of the LED
 */
void microbit_matrix_on(uint8_t row, uint8_t col);

/**
 * @brief   Turn off a single LED in the LED matrix
 *
 * @param[in] row       row of the LED
 * @param[in] col       column of the LED
 */
void microbit_matrix_off(uint8_t row, uint8_t col);

/**
 * @brief   Write the given 'image' to the LED matrix
 *
 * In the given buffer, each byte represents one LED in the matrix, hence the
 * buffer MUST be at least 25 byte wide. A byte value of `0` turns an LED off,
 * while any other value turns it on.
 *
 * @param[in] buf       new data to display, MUST be at least 25 byte
 */
void microbit_matrix_set_raw(const uint8_t *buf);

/**
 * @brief   Write the given character to the matrix, using the Mineplex font
 *
 * @param[in] c         character to display
 */
void microbit_matrix_set_char(char c);

/**
 * @brief   Shift the given string through the LED matrix
 *
 * @param[in] str       string do display
 * @param[in] delay     delay between each step [in us]
 */
void microbit_matrix_shift_str(const char *str, uint32_t delay);

#ifdef __cplusplus
}
#endif

#endif /* MICROBIT_H */
/** @} */
