/*
 * Copyright (C) 2021  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-wl55jc
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Nucleo-wl55jc board
 *
 *
 * @author      Akshai M <akshai.m@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    board_common_nucleo_init();

    if (IS_USED(MODULE_SX126X_STM32WL)) {
        /* Initialize the GPIO control for RF 3-port switch (SP3T) */
        gpio_init(FE_CTRL1, GPIO_OUT);
        gpio_init(FE_CTRL2, GPIO_OUT);
        gpio_init(FE_CTRL3, GPIO_OUT);
    }
}
