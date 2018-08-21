/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_esp8266-sparkfun-thing ESP8266 - Sparkfun ESP8266 Thing
 * @ingroup     boards
 * @brief       Support for the Sparkfun ESP8266 Thing modules.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Sparkfun ESP8266 Thing modules.
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
#define LED0_PIN    GPIO5

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

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
