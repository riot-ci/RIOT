/*
 * Copyright (C)  2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-explorer
 *
 * @file
 * @brief       Board common implementations for the SODAQ ExpLoRer board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the on-board LED */
    gpio_init(LED0_PIN, GPIO_OUT);
}
