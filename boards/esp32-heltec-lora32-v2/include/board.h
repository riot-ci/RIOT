/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_heltec-lora32-v2
 * @brief       Board specific definitions for Heltec WiFi LoRa 32 V2 board
 * @{
 *
 * Heltec WiFi LoRa 32 V2 is an ESP32 development board with 8 MB Flash that
 * uses the EPS32 chip directly. It integrates a SemTech SX1276 or SX1278 for
 * LoRaWAN communication in the 433 MHz or the 868/915 MHz band, respectively.
 * Additionally, it has an OLED display connected via I2C on board.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_comm_periph "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_app_spec_conf
 * "application-specific board configuration".
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

/**
 * @name    Button pin definitions
 * @{
 */
#define BUTTON0_PIN     GPIO0
/** @} */

/**
 * @name    LED (on-board) configuration
 *
 * @{
 */
#define LED0_PIN        GPIO25
#define LED0_ACTIVE     (1)     /**< LED is high active */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/**
 * @name        SX127X
 *
 * SX127X configuration.
 * @{
 */
#define SX127X_PARAM_SPI                (SPI_DEV(0))
#define SX127X_PARAM_SPI_NSS            GPIO18
#define SX127X_PARAM_RESET              GPIO14
#define SX127X_PARAM_DIO0               GPIO26
#define SX127X_PARAM_DIO1               GPIO_UNDEF /* GPIO35 has no pulldown, leads to init error */
#define SX127X_PARAM_DIO2               GPIO_UNDEF /* GPIO34 has no pulldown, leads to init error */
#define SX127X_PARAM_DIO3               GPIO_UNDEF
#define SX127X_PARAM_DIO_MULTI          GPIO_UNDEF
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
