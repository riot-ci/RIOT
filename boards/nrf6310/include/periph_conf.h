/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_nrf6310
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nRF51 board nrf6310
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "periph_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 *
 * @note    The radio will not work with the internal RC oscillator!
 *
 * @{
 */
#define CLOCK_HFCLK         (16U)           /* set to  0: internal RC oscillator
                                                      16: 16MHz crystal
                                                      32: 32MHz crystal */
#define CLOCK_LFCLK         (1)             /* set to  0: internal RC oscillator
                                             *         1: 32.768 kHz crystal
                                             *         2: derived from HFCLK */
/** @} */

/**
 * @name    UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART pin configuration */
#define UART_PIN_RX       16
#define UART_PIN_TX       17
#define UART_HWFLOWCTRL   0
#define UART_PIN_RTS      19
#define UART_PIN_CTS      18
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPI0,
        .sclk = 23,
        .mosi = 22,
        .miso = 20
    },
    {
        .dev  = NRF_SPI1,
        .sclk = 16,
        .mosi = 17,
        .miso = 18
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name    ADC configuration
 *
 * The configuration consists simply of a list of channels that should be used
 * @{
 */
#define ADC_NUMOF          (0)
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
