/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ata8520
 * @brief       Default initialization parameters for the ATA8520 device
 * @{
 * @file
 * @brief       Default initialization parameters for the ATA8520 device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef ATA8520_PARAMS_H
#define ATA8520_PARAMS_H

#include "board.h"
#include "ata8520.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the ATA8520 driver
 * @{
 */
#ifndef ATA8520_PARAM_SPI
#define ATA8520_PARAM_SPI         (SPI_DEV(0))
#endif
#ifndef ATA8520_PARAM_SPI_CLK
#define ATA8520_PARAM_SPI_CLK     (SPI_CLK_100KHZ)
#endif
#ifndef ATA8520_PARAM_CS
#define ATA8520_PARAM_CS          (GPIO_PIN(0, 0))
#endif
#ifndef ATA8520_PARAM_INT
#define ATA8520_PARAM_INT         (GPIO_PIN(0, 1))
#endif
#ifndef ATA8520_PARAM_SLEEP
#define ATA8520_PARAM_SLEEP       (GPIO_PIN(0, 2))
#endif
#ifndef ATA8520_PARAM_RESET
#define ATA8520_PARAM_RESET       (GPIO_PIN(0, 3))
#endif

#define ATA8520_PARAMS_DEFAULT    {.spi = ATA8520_PARAM_SPI,         \
                                   .spi_clk = ATA8520_PARAM_SPI_CLK, \
                                   .cs_pin = ATA8520_PARAM_CS,       \
                                   .int_pin = ATA8520_PARAM_INT,     \
                                   .sleep_pin = ATA8520_PARAM_SLEEP, \
                                   .reset_pin = ATA8520_PARAM_RESET}
/**@}*/

/**
 * @brief   ATA8520 configuration
 */
static const ata8520_params_t ata8520_params[] =
{
#ifdef ATA8520_PARAMS_BOARD
    ATA8520_PARAMS_BOARD,
#else
    ATA8520_PARAMS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* ATA8520_PARAMS_H */
