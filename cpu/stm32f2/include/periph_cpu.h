/*
 * Copyright (C) 2015 Engineering-Spirit
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f2
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author          Aurelien Gonce <aurelien.gonce@altran.fr>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Starting address of the CPU ID
 */
#define CPUID_ADDR          (0x1fff7a10)

/**
 * @brief   Available ports on the STM32F2 family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
    PORT_H = 7,             /**< port H */
    PORT_I = 8,             /**< port I */
    PORT_EXT = 9            /**< first GPIO expander port */
};

/**
 * @brief   Available ports on the STM32F2 family as GPIO register definitions
 */
#define GPIO_CPU_PORTS \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_A) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_B) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_C) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_D) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_E) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_F) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_G) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_H) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_I) },

/**
 * @brief   Available number of ADC devices
 */
#define ADC_DEVS            (2U)

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
    uint8_t dev;            /**< ADCx - 1 device used for the channel */
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

#ifndef DOXYGEN
/**
 * @brief   Override the ADC resolution configuration
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0x03000000,  /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = 0x02000000,  /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = 0x01000000,  /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = 0x00000000,  /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 1,           /**< ADC resolution: 14 bit (not supported) */
    ADC_RES_16BIT = 2            /**< ADC resolution: 16 bit (not supported)*/
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
