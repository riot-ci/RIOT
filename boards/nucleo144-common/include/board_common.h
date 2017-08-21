/*
 * Copyright (C) 2017 Inria
 *               2017 OTAKeys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo144-common STM Nucleo-144 Common
 * @ingroup     boards_nucleo
 * @brief       Common files for STM Nucleo-144 boards
 * @{
 *
 * @file
 * @brief       Common pin definitions and board configuration options
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include "cpu.h"
#include "periph_conf.h"
#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#if defined(CPU_MODEL_STM32L496ZG)
#define LED0_PORT           PORT_C
#define LED0_GPIO           GPIOC
#define LED0_PIN_NUM        7
#else
#define LED0_PORT           PORT_B
#define LED0_GPIO           GPIOB
#define LED0_PIN_NUM        0
#endif
#define LED0_PIN            GPIO_PIN(LED0_PORT, LED0_PIN_NUM)
#define LED0_MASK           (1 << LED0_PIN_NUM)
#define LED0_ON             (LED0_GPIO->BSRR = LED0_MASK)
#define LED0_OFF            (LED0_GPIO->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (LED0_GPIO->ODR  ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_B, 7)
#define LED1_MASK           (1 << 7)
#define LED1_ON             (GPIOB->BSRR = LED1_MASK)
#define LED1_OFF            (GPIOB->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (GPIOB->ODR  ^= LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_B, 14)
#define LED2_MASK           (1 << 14)
#define LED2_ON             (GPIOB->BSRR = LED2_MASK)
#define LED2_OFF            (GPIOB->BSRR = (LED2_MASK << 16))
#define LED2_TOGGLE         (GPIOB->ODR  ^= LED2_MASK)
/** @} */

/**
 * @brief   User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)
#define BTN0_MODE           GPIO_IN_PD
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_COMMON_H */
/** @} */
