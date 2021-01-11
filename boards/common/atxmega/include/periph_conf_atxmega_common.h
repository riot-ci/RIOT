/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2016 RWTH Aachen, Josua Arndt
 *               2016 INRIA, Francisco Acosta
 *               2017 HAW Hamburg, Dimitri Nahm
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *               2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_atxmega
 * @{
 *
 * @file
 * @brief       Common configuration of MCU periphery for ATxmega boards
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */

#ifndef PERIPH_CONF_ATXMEGA_COMMON_H
#define PERIPH_CONF_ATXMEGA_COMMON_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#ifndef CLOCK_CORECLOCK
#define CLOCK_CORECLOCK     (32000000UL)
#endif /* CLOCK_CORECLOCK */
/** @} */

/**
 * @name    UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */

#ifndef UART_NUMOF
    #define UART_NUMOF      (0U)
#endif /* UART_NUMOF */
/** @} */

/**
 * @name    SPI configuration
 *
 * The SS pin must be configured as output for the SPI device to work as
 * master correctly, though we do not use it for now (as we handle the chip
 * select externally for now)
 *
 * @{
 */
#ifndef SPI_NUMOF
#define SPI_NUMOF           (1U)
#endif /* SPI_NUMOF */
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
#ifndef I2C_NUMOF
#define I2C_BUS_SPEED       I2C_SPEED_FAST
#define I2C_NUMOF           (1U)
#endif /* I2C_NUMOF */
/** @} */

/**
 * @name ADC Configuration
 *
 * The number of ADC channels of the atmega328p depends on the package:
 *  - 6-channel 10-bit ADC in PDIP package
 *  - 8-channel 10-bit ADC in TQFP and QFN/MLF package
 * Arduino UNO / Duemilanove has thereby 6 channels. But only 5 channels can be
 * used for ADC, because the pin of ADC5 emulate a software triggered interrupt.
 * @{
 */
#ifndef ADC_NUMOF
    #define ADC_NUMOF       (0U)
#endif /* ADC_NUMOF */
/** @} */

/**
 * @name   PWM configuration
 *
 * @{
 */
#ifndef PWM_NUMOF
    #define PWM_NUMOF       (0U)
#endif /* PWM_NUMOF */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_ATXMEGA_COMMON_H */
