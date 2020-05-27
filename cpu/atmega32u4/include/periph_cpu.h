/*
 * Copyright (C) 2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega32u4
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Thomas Perrot <thomas.perrot@tupi.fr>
 *
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_conf.h" /* <- Allow overwriting timer config from board */
#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available ports on the ATmega32u4 family
 */
enum {
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3,       /**< port D */
    PORT_E = 4,       /**< port E */
    PORT_F = 5        /**< port F */
};

/**
 * @brief   Available external interrupt pins on the ATmega32u4 family
 *
 * In order of their interrupt number.
 */
#define CPU_ATMEGA_EXT_INTS    { GPIO_PIN(PORT_D, 0), \
                                 GPIO_PIN(PORT_D, 1), \
                                 GPIO_PIN(PORT_D, 2), \
                                 GPIO_PIN(PORT_D, 3) }

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
#define EEPROM_SIZE                (1024U)  /* 1kB */
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

#define TIMER_1             MEGA_TIMER3
#define TIMER_1_MASK        &TIMSK3
#define TIMER_1_FLAG        &TIFR3
#define TIMER_1_ISRA        TIMER3_COMPA_vect
#define TIMER_1_ISRB        TIMER3_COMPB_vect
#define TIMER_1_ISRC        TIMER3_COMPC_vect
#endif /* TIMER_NUMOF */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
