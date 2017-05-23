/*
 * Copyright (C)  2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-mkr1000
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for Arduino MKR1000 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "periph_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = &SERCOM1->SPI,
        .miso_pin = GPIO_PIN(PA, 19),   /* ARDUINO_PIN_8, SERCOM1-MISO */
        .mosi_pin = GPIO_PIN(PA, 16),   /* ARDUINO_PIN_10, SERCOM1-MOSI */
        .clk_pin  = GPIO_PIN(PA, 17),   /* ARDUINO_PIN_9, SERCOM1-SCK */
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1
    },
    {
        .dev      = &SERCOM2->SPI,
        .miso_pin = GPIO_PIN(PA, 15),
        .mosi_pin = GPIO_PIN(PA, 12),
        .clk_pin  = GPIO_PIN(PA, 13),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
