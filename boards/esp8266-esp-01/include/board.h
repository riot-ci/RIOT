/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_esp8266-esp-01 ESP8266 - ESP-01 module
 * @ingroup     boards
 * @brief       Support for the ESP-01 module.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ESP-01 module.
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
#define LED0_PIN    GPIO1

/**
 * @brief   Number of ADC inputs
 */
#define ADC_NUMOF   0

/**
 * @brief   I2C configuration
 */
#define I2C_NUMOF   0

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
