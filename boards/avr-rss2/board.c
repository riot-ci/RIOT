/*
 * Copyright (C) 2019 Robert Olsson <roolss@kth.se>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_rss2-mega256rfr2
 * @{
 *
 * @file
 * @brief       Board specific LED initialization
 * @author      Robert Olsson <roolss@kth.se>
 *
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "net/eui64.h"
#include <string.h>
#include <inttypes.h>

void led_init(void)
{
    /* Ports Pins as Output */
    LED_PORT_DDR |= LED1_MASK | LED0_MASK;
    /* All LEDs OFF */
    LED_PORT |= (LED1_MASK | LED0_MASK);
}

void board_init(void)
{
     led_init();
}
