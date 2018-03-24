/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @defgroup    boards_mega-xplained
 * @ingroup     boards
 * @brief       Support for the Mega Xplained board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Mega Xplained board.
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "mega-xplained_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    STDIO configuration
 *
 * As the CPU is too slow to handle 115200 baud, we set the default
 * baudrate to 9600 for this board
 * @{
 */
#ifndef UART_STDIO_BAUDRATE
#define UART_STDIO_BAUDRATE (9600U)
#endif
/** @} */

/**
 * @brief   Use the UART 1 for STDIO on this board
 */
#define UART_STDIO_DEV       (UART_DEV(1))

/**
 * @brief   Context swap defines
 *
 * Setup to use PD7 which is pin change interrupt 31 (PCINT31)
 * This emulates a software triggered interrupt
 */
#define AVR_CONTEXT_SWAP_INIT do { \
            DDRD |= (1 << PD7); \
            PCICR |= (1 << PCIE3); \
            PCMSK3 |= (1 << PCINT31); \
} while (0)
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT  PCINT3_vect
#define AVR_CONTEXT_SWAP_TRIGGER   PORTD ^= (1 << PD7)

/**
 * @name    xtimer configuration values
 *
 * Xtimer runs at 8MHz / 64 = 125kHz
 * @{
 */
#define XTIMER_DEV                  (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_WIDTH                (16)
#define XTIMER_HZ                   (125000UL)
#define XTIMER_BACKOFF              (40)
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
