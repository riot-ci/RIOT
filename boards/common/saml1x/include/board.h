/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_saml1x Microchip SAML1X
 * @ingroup     boards
 * @brief       Support for SAML10 and SAML11 boards
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Microchip
 *              SAML10 & SAML11 Xplained Pro board.
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    PORT selection macros
 * @{
 */
#ifdef CPU_FAM_SAML11
#define _PORT PORT_SEC
#else
#define _PORT PORT
#endif
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 7)

#define LED_PORT            _PORT->Group[PA]
#define LED0_MASK           (1 << 7)

#define LED0_ON             (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PORT           _PORT->Group[PA]
#define BTN0_PIN            GPIO_PIN(PA, 27)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_BACKOFF      (40)
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
