/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_lorwan-explorer Sodaq LoRaWAN Explorer
 * @ingroup     boards
 * @brief       Support for the Sodaq LoRaWAN Explorer board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Sodaq LoRaWAN Explorer board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The on-board LED is connected to pin 6 on this board
 */
#define ARDUINO_LED         (13U)

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 21)

#define LED0_PORT           PORT->Group[PA]
#define LED0_MASK           (1 << 21)

#define LED0_ON             (LED0_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED0_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @brief   User button
 */
#define BTN0_PIN            GPIO_PIN(PA, 14)
#define BTN0_MODE           GPIO_IN_PU

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
