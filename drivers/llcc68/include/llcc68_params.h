/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_llcc68
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LLCC68_PARAMS_H
#define LLCC68_PARAMS_H

#include "board.h"
#include "llcc68.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters (default adapted to nucleo64 boards)
 * @{
 */
#ifndef LLCC68_PARAM_SPI
#define LLCC68_PARAM_SPI                    SPI_DEV(0)
#endif

#ifndef LLCC68_PARAM_SPI_NSS
#define LLCC68_PARAM_SPI_NSS                GPIO_PIN(0, 8)  /* D7 */
#endif

#ifndef LLCC68_PARAM_RESET
#define LLCC68_PARAM_RESET                  GPIO_PIN(0, 0)  /* A0 */
#endif

#ifndef LLCC68_PARAM_BUSY
#define LLCC68_PARAM_BUSY                   GPIO_PIN(1, 3)  /* D3 */
#endif

#ifndef LLCC68_PARAM_DIO1
#define LLCC68_PARAM_DIO1                   GPIO_PIN(1, 4)  /* D5 */
#endif

#define LLCC68_PARAMS             { .spi = LLCC68_PARAM_SPI,      \
                                    .nss_pin = LLCC68_PARAM_SPI_NSS,  \
                                    .reset_pin = LLCC68_PARAM_RESET,    \
                                    .busy_pin = LLCC68_PARAM_BUSY,      \
                                    .dio1_pin = LLCC68_PARAM_DIO1 }
/**@}*/

/**
 * @brief   Configuration struct
 */
static const llcc68_params_t llcc68_params[] =
{
    LLCC68_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* LLCC68_PARAMS_H */
/** @} */
