/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp32
 * @brief       Common declarations and functions for all ESP32 boards.
 *
 * This file contains default declarations and functions that are used
 * for all ESP32 boards.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "sdk_conf.h"

#ifdef __cplusplus
 extern "C" {
#endif

#ifndef UART_STDIO_BAUDRATE
/** baudrate for stdio */
#define UART_STDIO_BAUDRATE      (CONFIG_CONSOLE_UART_BAUDRATE)
#endif

/**
 * @brief Initialize board specific hardware
 *
 * Since all features of ESP32 boards are provided by the SOC, almost all
 * initializations are done during the CPU initialization that is called from
 * boot loader. Therefore, this function only initializes SPI interfaces.
 */
void board_init (void);

/**
  * @brief Print the board configuration in a human readable format
  */
void esp_print_board_config (void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_COMMON_H */
/** @} */
