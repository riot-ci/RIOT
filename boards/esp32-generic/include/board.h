/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_esp32_generic ESP32 - Generic Board
 * @ingroup     boards
 * @brief       Board defitions for generic ESP32 boards
 * @{
 *
 * This board definition can be used for all ESP32 boards that simply break
 * out all GPIOs to external pads without having any special hardware or
 * interfaces on-board. These are usually boards that use the ESP32
 * WROOM module like Espressif's EPS32-DEVKIT or ESP-32 NodeMCU and a large
 * number of clones.
 *
 * Therefore the board definition is simply a wrapper arround the common ESP32
 * board definition.
 *
 * **PLEASE NOTE:**
 * Most of the board defitions can be overriden by an application specific
 * board configuration file. For that purpose, a header file located in
 * application directory can be specified using the BOARD_APP_CONF make
 * variable at command line, for example:
 *
 *          BOARD_APP_CONF=esp32_generic_conf.h
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/* include the application specific board configuration file from application
   source directory if specified */
#ifdef BOARD_APP_CONF
#include BOARD_APP_CONF
#endif

/* define all board specific definitions here */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
