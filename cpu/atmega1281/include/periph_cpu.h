/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega1281
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph_cpu_common.h"
#include "periph_conf.h" /* <- Allow overwriting timer config from board */

/**
 * @brief   Available ports on the ATmega1281 family
 */
enum {
    PORT_A = 0,       /**< port A */
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3,       /**< port D */
    PORT_E = 4,       /**< port E */
    PORT_F = 5,       /**< port F */
    PORT_G = 6,       /**< port G */
};

/**
 * @brief   Available external interrupt pins on the ATmega1281 family
 *
 * In order of their interrupt number.
 */
#define CPU_ATMEGA_EXT_INTS    { GPIO_PIN(PORT_D, 0), \
                                 GPIO_PIN(PORT_D, 1), \
                                 GPIO_PIN(PORT_D, 2), \
                                 GPIO_PIN(PORT_D, 3), \
                                 GPIO_PIN(PORT_E, 4), \
                                 GPIO_PIN(PORT_E, 5), \
                                 GPIO_PIN(PORT_E, 6), \
                                 GPIO_PIN(PORT_E, 7) }

/**
 * @name   Defines for the I2C interface
 * @{
 */
#define I2C_PORT_REG            PORTD
#define I2C_PIN_MASK            (1 << PORTD0) | (1 << PORTD1)
/** @} */

/**
 * @name    EEPROM configuration
 * @{
 */
#define EEPROM_SIZE                (4096U)  /* 4kB */
/** @} */

/**
 * @name    Default timer configuration
 */
#ifndef TIMER_NUMOF
#define TIMER_NUMOF         (2U)
#define TIMER_CHANNELS      (3)

#define TIMER_0             MEGA_TIMER1
#define TIMER_0_MASK        &TIMSK1
#define TIMER_0_FLAG        &TIFR1
#define TIMER_0_ISRA        TIMER1_COMPA_vect
#define TIMER_0_ISRB        TIMER1_COMPB_vect
#define TIMER_0_ISRC        TIMER1_COMPC_vect

#define TIMER_1             MEGA_TIMER4
#define TIMER_1_MASK        &TIMSK4
#define TIMER_1_FLAG        &TIFR4
#define TIMER_1_ISRA        TIMER4_COMPA_vect
#define TIMER_1_ISRB        TIMER4_COMPB_vect
#define TIMER_1_ISRC        TIMER4_COMPC_vect
#endif TIMER_NUMOF
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
