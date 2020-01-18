/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f3
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
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
#define CPUID_ADDR          (0x1ffff7ac)

/**
 * @brief   Available ports on the STM32F3 family
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
    PORT_EXT = 8            /**< first GPIO expander port */
};

/**
 * @brief   Available ports on the STM32F3 family as GPIO register definitions
 */
#define GPIO_CPU_PORTS \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_A) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_B) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_C) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_D) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_E) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_F) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_G) }, \
    { .reg = (gpio_reg_t)GPIO_CPU_PORT_ADDR(PORT_H) },

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
