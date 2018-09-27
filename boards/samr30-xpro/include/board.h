/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_saml21-xpro Atmel SAM L21 Xplained Pro
 * @ingroup     boards
 * @brief       Support for the Atmel SAM L21 Xplained Pro board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmel SAM L21 Xplained Pro board.
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @name NG_AT86RF212B configuration
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAM_CS         GPIO_PIN(PB, 31)
#define AT86RF2XX_PARAM_INT        GPIO_PIN(PB, 0)
#define AT86RF2XX_PARAM_SLEEP      GPIO_PIN(PA, 20)
#define AT86RF2XX_PARAM_RESET      GPIO_PIN(PB, 15)
#define AT86RF2XX_PARAM_SPI        SPI_DEV(0)
#define AT86RF2XX_PARAM_SPI_CLK    SPI_CLK_5MHZ

/** @}*//*
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED_PORT                PORT->Group[0]

#define LED0GREEN_PIN            GPIO_PIN(0, 18)
#define LED0GREEN_MASK           (1 << 18)
#define LED0GREEN_ON             (LED_PORT.OUTCLR.reg = LED0GREEN_MASK)
#define LED0GREEN_OFF            (LED_PORT.OUTSET.reg = LED0GREEN_MASK)
#define LED0GREEN_TOGGLE         (LED_PORT.OUTTGL.reg = LED0GREEN_MASK)

#define LED1ORANGE_PIN            GPIO_PIN(0, 19)
#define LED1ORANGE_MASK           (1 << 19)
#define LED1ORANGE_ON             (LED_PORT.OUTCLR.reg = LED1ORANGE_MASK)
#define LED1ORANGE_OFF            (LED_PORT.OUTSET.reg = LED1ORANGE_MASK)
#define LED1ORANGE_TOGGLE         (LED_PORT.OUTTGL.reg = LED1ORANGE_MASK)
/** @} */

/**
 * @name SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PORT           PORT->Group[PA]
#define BTN0_PIN            GPIO_PIN(PA, 6)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
