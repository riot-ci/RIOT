/*
 * Copyright (C)  2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-feather-m0
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Adafruit Feather M0 boards
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();
    /* initialize the on-board red LED on pin PA17 */
    gpio_init(LED0_PIN, GPIO_OUT);
}
