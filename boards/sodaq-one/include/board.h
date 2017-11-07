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
 * @brief       Board specific definitions for the SODAQ ONE board
 *
 * @author      Kees Bakker <kees@sodaq.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED_GREEN_PIN       GPIO_PIN(PB, 10)

#define LED_GREEN_PORT      PORT->Group[PB]
#define LED_GREEN_MASK      (1 << 10)

#define LED_GREEN_OFF       (LED_GREEN_PORT.OUTSET.reg = LED_GREEN_MASK)
#define LED_GREEN_ON        (LED_GREEN_PORT.OUTCLR.reg = LED_GREEN_MASK)
#define LED_GREEN_TOGGLE    (LED_GREEN_PORT.OUTTGL.reg = LED_GREEN_MASK)

#define LED_RED_PIN         GPIO_PIN(PA, 15)

#define LED_RED_PORT        PORT->Group[PA]
#define LED_RED_MASK        (1 << 15)

#define LED_RED_OFF         (LED_RED_PORT.OUTSET.reg = LED_RED_MASK)
#define LED_RED_ON          (LED_RED_PORT.OUTCLR.reg = LED_RED_MASK)
#define LED_RED_TOGGLE      (LED_RED_PORT.OUTTGL.reg = LED_RED_MASK)

#define LED_BLUE_PIN        GPIO_PIN(PB, 11)

#define LED_BLUE_PORT       PORT->Group[PB]
#define LED_BLUE_MASK       (1 << 11)

#define LED_BLUE_OFF        (LED_BLUE_PORT.OUTSET.reg = LED_BLUE_MASK)
#define LED_BLUE_ON         (LED_BLUE_PORT.OUTCLR.reg = LED_BLUE_MASK)
#define LED_BLUE_TOGGLE     (LED_BLUE_PORT.OUTTGL.reg = LED_BLUE_MASK)
/** @} */

/**
 * @name    User button
 */
#define BTN0_PIN            GPIO_PIN(PA, 16)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @name    GPS Time Pulse
 */
#define GPS_TIMEPULSE_PIN   GPIO_PIN(PA, 14)
#define GPS_TIMEPULSE_MODE  GPIO_IN
/** @} */

/**
 * @name    GPS Enable
 * @{
 */
#define GPS_ENABLE_PIN      GPIO_PIN(PA, 18)

#define GPS_ENABLE_PORT     PORT->Group[PA]
#define GPS_ENABLE_MASK     (1 << 18)

#define GPS_ENABLE_ON       (GPS_ENABLE_PORT.OUTSET.reg = GPS_ENABLE_MASK)
#define GPS_ENABLE_OFF      (GPS_ENABLE_PORT.OUTCLR.reg = GPS_ENABLE_MASK)
/** @} */

/**
 * @name    LORA Reset
 * @{
 */
#define LORA_RESET_PIN      GPIO_PIN(PA, 4)

#define LORA_RESET_PORT     PORT->Group[PA]
#define LORA_RESET_MASK     (1 << 4)

#define LORA_RESET_OFF      (LORA_RESET_PORT.OUTSET.reg = LORA_RESET_MASK)
#define LORA_RESET_ON       (LORA_RESET_PORT.OUTCLR.reg = LORA_RESET_MASK)
#define LORA_RESET_TOGGLE   (LORA_RESET_PORT.OUTTGL.reg = LORA_RESET_MASK)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
