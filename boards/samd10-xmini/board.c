/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samd10-xmini
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Atmel SAM D10 Xplained
 *              Mini board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

static void _gpio_init_high(gpio_t pin)
{
    gpio_init(pin, GPIO_OUT);
    gpio_set(pin);
}

void board_init(void)
{
    /* initialize the on-board LED */
    gpio_init(LED0_PIN, GPIO_OUT);

    /* initialize the on-board button */
    gpio_init(BTN0_PIN, BTN0_MODE);

    /* If the PA24 and PA25 pins are not connected, it is recommended
     * to enable a pull-up on PA24 and PA25 through input GPIO mode.
     * (those have external pull-ups on the board that would leak current)
     */
    _gpio_init_high(GPIO_PIN(PA, 25));
    _gpio_init_high(GPIO_PIN(PA, 24));

    /* initialize the CPU */
    cpu_init();
}
