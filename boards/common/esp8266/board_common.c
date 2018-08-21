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

#ifdef __cplusplus
extern "C" {
#endif

void esp8266_led_on_off (uint8_t led, uint8_t value)
{
    GPIO_REG_WRITE((value ? GPIO_OUT_W1TC_ADDRESS : GPIO_OUT_W1TS_ADDRESS), BIT(led));
}

void esp8266_led_toggle (uint8_t led)
{
    esp8266_led_on_off (led, ((GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT(led))) ? 1 : 0);
}

void board_init(void)
{
    #ifdef LED0_PIN
    gpio_init (LED0_PIN, GPIO_OUT);
    LED0_OFF;
    #endif
    #ifdef LED1_PIN
    gpio_init (LED1_PIN, GPIO_OUT);
    LED1_OFF;
    #endif
}

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
