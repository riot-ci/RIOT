/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_esp8266-esp-12x ESP8266 - ESP-12x based modules
 * @ingroup     boards
 * @brief       Support for ESP-12x based modules.
 *
 * Compatible modules: Wemos D1 mini, NodeMCU, HUZZAH ESP8266 Breakout, ...
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ESP-12x based modules.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Pin of first LED
 */
#define LED0_PIN    GPIO2

/**
 * @brief   Number of ADC inputs
 */
#define ADC_NUMOF   1

/**
 * @name   I2C configuration
 * @{
 */
#define I2C_NUMOF   1
#define I2C_SDA_0   GPIO4
#define I2C_SCL_0   GPIO5
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
