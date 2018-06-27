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
#include "common.h"
#include "log.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
 extern "C" {
#endif

void board_init(void)
{
    for (int i = 0; i < SPI_NUMOF; i++)
        spi_init (SPI_DEV(i));
}

static uint32_t leds1_initialized = 0x0; /* GPIOs 0  ... 31 */
static uint32_t leds2_initialized = 0x0; /* GPIOs 32 ... 39 */

static void _led_init(uint8_t led)
{
    uint32_t  led_mask = (led < 32) ? BIT(led) : BIT(led - 32);
    uint32_t* led_init = (led < 32) ? &leds1_initialized : &leds2_initialized;

    if (!(*led_init & led_mask))
    {
        gpio_init (led, GPIO_OUT);
        *led_init |= led_mask;
    }
}

void led_on_off (uint8_t led, uint8_t value)
{
    _led_init(led);
    gpio_write(led, value ? 0 : 1);
}

void led_toggle (uint8_t led)
{
    _led_init(led);
    gpio_toggle(led);
}

extern void adc_print_config(void);
extern void pwm_print_config(void);
extern void i2c_print_config(void);
extern void spi_print_config(void);
extern void uart_print_config(void);

void print_board_config (void)
{
    adc_print_config();
    pwm_print_config();
    i2c_print_config();
    spi_print_config();
    uart_print_config();

    LOG_INFO("LED: pins=[ ");
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
