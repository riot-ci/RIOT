/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_arduino-mkrzero Arduino MKRZERO
 * @ingroup     boards
 * @brief       Support for the Arduino MKRZERO board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino MKRZERO
 *              board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   xtimer configuration
 * @{
 */
#define XTIMER              TIMER_0
#define XTIMER_CHAN         (0)
/** @} */

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 20)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 20)

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)
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
