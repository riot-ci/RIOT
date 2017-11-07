/*
 * Copyright (C) 2017 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-one
 * @{
 *
 * @file
 * @brief       Board common implementations for the SODAQ ONE board
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the on-board LEDs, switch them off to start with */
    LED_GREEN_OFF;
    gpio_init(LED_GREEN_PIN, GPIO_OUT);
    LED_RED_OFF;
    gpio_init(LED_RED_PIN, GPIO_OUT);
    LED_BLUE_OFF;
    gpio_init(LED_BLUE_PIN, GPIO_OUT);

    /* reset RN2483 (LoRa) */
    LORA_RESET_OFF;
    gpio_init(LORA_RESET_PIN, GPIO_OUT);

    GPS_ENABLE_OFF;
    gpio_init(GPS_ENABLE_PIN, GPIO_OUT);

    /* initialize the CPU */
    cpu_init();
}
