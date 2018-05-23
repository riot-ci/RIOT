/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_esp8266 ESP8266 Common
 * @ingroup     boards_common
 * @brief       Common files for the esp8266 board.
 * @{
 *
 * @file
 * @brief       Definitions for all esp8266 board.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "espressif/eagle_soc.h"
#include "board_common.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
 extern "C" {
#endif

static uint32_t leds_initialized = 0x0;

void led_on_off (uint8_t led, uint8_t value)
{
    uint32_t led_mask = BIT(led);
    if (!(leds_initialized & led_mask)) {
        gpio_init (led, GPIO_OUT);
        leds_initialized |= led_mask;
    }
    GPIO_REG_WRITE((value ? GPIO_OUT_W1TC_ADDRESS : GPIO_OUT_W1TS_ADDRESS), led_mask);
}

void led_toggle (uint8_t led)
{
    uint32_t led_mask = BIT(led);
    led_on_off (led, ((GPIO_REG_READ(GPIO_OUT_ADDRESS) & led_mask)) ? 1 : 0);
}

void board_init(void)
{
    #if defined(SPI_USED)
    for (int i = 0; i < SPI_NUMOF; i++) {
        spi_init (SPI_DEV(i));
    }
    #endif
}

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
