/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Architecture specific GPIO functions ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef GPIO_ARCH_H
#define GPIO_ARCH_H

#include "periph/gpio.h"

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of possible GPIO usage types (for internal use only)
 */
typedef enum
{
    _GPIO = 0,  /**< pin used as standard GPIO */
    _ADC,       /**< pin used as ADC input */
    _DAC,       /**< pin used as DAC output */
    _I2C,       /**< pin used as I2C signal */
    _PWM,       /**< pin used as PWM output */
    _SPI,       /**< pin used as SPI interface */
    _SPIF,      /**< pin used as SPI flash interface */
    _UART,      /**< pin used as UART interface */
    _NOT_EXIST  /**< pin cannot be used at all */
} _gpio_pin_usage_t;

/**
 * @brief   Table of the usage type of each GPIO pin
 */
extern _gpio_pin_usage_t _gpio_pin_usage [];

/**
 * @brief   String representation of usage types
 */
extern const char* _gpio_pin_usage_str[];

/**
 * @brief   Table of GPIO to IOMUX register mappings
 */
extern const uint32_t _gpio_to_iomux_reg[];
#define GPIO_PIN_MUX_REG _gpio_to_iomux_reg

/**
 * @brief   Disable the pullup of the pin
 */
void gpio_pullup_dis (gpio_t pin);

/**
 * @brief   Returns the RTCIO pin number or -1 if the pin is not an RTCIO pin
 */
int8_t gpio_is_rtcio (gpio_t pin);

/**
  * @brief  Configure sleep mode for an GPIO pin if the pin is an RTCIO pin
  * @param  pin     GPIO pin
  * @param  mode    active in sleep mode if true
  * @param  input   as input if true, as output otherwise
  * @return 0 success
  * @return -1 on invalid pin
  */
int gpio_config_sleep_mode (gpio_t pin, bool sleep_mode, bool input);

/**
 * @brief   extern declaratoin of ROM functions to avoid compilation conflicts
 */
void gpio_matrix_in(uint32_t gpio, uint32_t signal_idx, bool inv);
void gpio_matrix_out(uint32_t gpio, uint32_t signal_idx, bool out_inv, bool oen_inv);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* GPIO_ARCH_H */
