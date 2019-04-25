/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_common_kw41z
 * @{
 *
 * @file
 * @name        Default SPI configuration for KW41Z based boards
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef CFG_SPI_DEFAULT_H
#define CFG_SPI_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @name SPI configuration
* @{
*/
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI0,
        .pin_miso = GPIO_PIN(PORT_C, 18),
        .pin_mosi = GPIO_PIN(PORT_C, 17),
        .pin_clk  = GPIO_PIN(PORT_C, 16),
        .pin_cs   = {
            GPIO_PIN(PORT_C, 19),
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF
        },
        .pcr      = GPIO_AF_2,
        .simmask  = SIM_SCGC6_SPI0_MASK
    },
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_SPI_DEFAULT_H */
/** @} */
