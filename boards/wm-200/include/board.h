/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_wm-200
 * @{
 *
 * @file
 * @brief       Board specific definitions for the WM-200 Module board.
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (32000000ul)

/**
 * @brief Use the UART1 for STDIO on this board
 */
#define STDIO_UART_DEV      UART_DEV(1)

/**
 * @name   Baudrate for STDIO terminal
 *
 * The standard configuration for STDIO in cpu/atxmega/periph/uart.c
 * is to use double speed.
 *
 * For 32MHz F_CPU following Baudrate have good error rates
 *  115200
 *
 * Matches this with BAUD in Board/Makefile.include
 *
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (115200U)
#endif
/** @} */

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_A, PIN6_bp)
#define LED0_MASK           (PIN6_bm)
#define LED0_ON             (LED_PORT.OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL = LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_A, PIN3_bp)
#define LED1_MASK           (PIN3_bm)
#define LED1_ON             (LED_PORT.OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT.OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT.OUTTGL = LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_A, PIN2_bp)
#define LED2_MASK           (PIN2_bm)
#define LED2_ON             (LED_PORT.OUTCLR = LED2_MASK)
#define LED2_OFF            (LED_PORT.OUTSET = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT.OUTTGL = LED2_MASK)

#define LED_PORT_MASK       (LED0_MASK | LED1_MASK | LED2_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_F, PIN5_bm)
#define BTN0_MODE           (GPIO_IN | GPIO_OPC_PU | GPIO_SLEW_RATE)
#define BTN0_INT_FLANK      (GPIO_ISC_FALLING | GPIO_LVL_LOW)
/** @} */

/**
 * @name xtimer configuration values
 * if XTIMER_HZ > 1MHz then (XTIMER_HZ != (1000000ul << XTIMER_SHIFT))
 * if XTIMER_HZ < 1MHz then ((XTIMER_HZ << XTIMER_SHIFT) != 1000000ul)
 *
 * 32MHz Core Clock
 * XTIMER_HZ 4000000 (clkdiv 8 )    XTIMER_SHIFT 2
 * XTIMER_HZ 1000000 ()             XTIMER_SHIFT 0
 * XTIMER_HZ  500000 (clkdiv 64)    XTIMER_SHIFT 1
 * XTIMER_HZ  250000 (clkdiv 128)   XTIMER_SHIFT 2
 * XTIMER_HZ   31250 (clkdiv 1024)  XTIMER_SHIFT 5
 *
 * @{
 */

#define XTIMER_DEV          TIMER_DEV(0)
#define XTIMER_CHAN         (0)
#define XTIMER_WIDTH        (16)
#define XTIMER_HZ           (4000000UL)
#define XTIMER_BACKOFF      (150)
/** @} */
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

/**
 * @brief   Initialize board specific hardware LEDs
 */
void led_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
