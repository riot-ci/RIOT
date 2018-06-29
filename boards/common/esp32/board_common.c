/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_esp32 ESP32 Board Commons
 * @ingroup     boards_common
 * @brief       Common definitions for all ESP32 boards
 * @{
 *
 * @file
 * @brief       Common declarations and functions for all ESP32 boards.
 *
 * This file contains default declarations and functions that are valid
 * for all ESP32 boards.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board_common.h"
#include "esp_common.h"
#include "log.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
 extern "C" {
#endif

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
    #ifdef LED2_PIN
    gpio_init (LED2_PIN, GPIO_OUT);
    LED2_OFF;
    #endif
}

void esp_led_on_off (uint8_t led, uint8_t value)
{
    gpio_write(led, value ? 0 : 1);
}

void esp_led_toggle (uint8_t led)
{
    gpio_toggle(led);
}

extern void adc_print_config(void);
extern void pwm_print_config(void);
extern void i2c_print_config(void);
extern void spi_print_config(void);
extern void uart_print_config(void);

void esp_print_board_config (void)
{
    adc_print_config();
    pwm_print_config();
    i2c_print_config();
    spi_print_config();
    uart_print_config();

    LOG_TAG_INFO("led","pins=[ ");
    #ifdef LED0_PIN
    LOG_INFO("%d ", LED0_PIN);
    #endif
    #ifdef LED1_PIN
    LOG_INFO("%d ", LED1_PIN);
    #endif
    #ifdef LED2_PIN
    LOG_INFO("%d ", LED2_PIN);
    #endif
    LOG_INFO("]\n");
}

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
