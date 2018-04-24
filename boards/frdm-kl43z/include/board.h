/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_frdm-kl43z NXP FRDM-KL43Z Board
 * @ingroup     boards
 * @brief       Support for the NXP FRDM-KL43Z
 * @{
 *
 * @file
 * @brief       Board specific definitions for the FRDM-KL43Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
/* LEDs are named LED1, LED2 in the original board schematics, we remap the LEDs
 * to 0-indexed: NXP LED1 -> RIOT LED0, NXP LED2 -> RIOT LED1 */
#define LED0_PIN            GPIO_PIN(PORT_D,  5)
#define LED1_PIN            GPIO_PIN(PORT_E, 31)

#define LED0_MASK           (1 <<  5)
#define LED1_MASK           (1 << 31)

#define LED0_ON            (GPIOD->PCOR = LED0_MASK)
#define LED0_OFF           (GPIOD->PSOR = LED0_MASK)
#define LED0_TOGGLE        (GPIOD->PTOR = LED0_MASK)

#define LED1_ON            (GPIOE->PCOR = LED1_MASK)
#define LED1_OFF           (GPIOE->PSOR = LED1_MASK)
#define LED1_TOGGLE        (GPIOE->PTOR = LED1_MASK)
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
