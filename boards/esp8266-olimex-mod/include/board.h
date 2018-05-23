/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_esp8266-olimex-mod Olimex MOD-WIFI-ESP8266-DEV
 * @ingroup     boards
 * @brief       Support for the Olimex MOD-WIFI-ESP8266-DEV module.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Olimex MOD-WIFI-ESP8266-DEV module.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "board_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Pin of first LED
 */
#define LED0_PIN    GPIO1

/**
 * @brief   Number of ADC inputs
 */
#define ADC_NUMOF   1

/**
 * @name   I2C configuration
 * @{
 */
#define I2C_NUMOF   1
#define I2C_SDA_0   GPIO2
#define I2C_SCL_0   GPIO14
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
