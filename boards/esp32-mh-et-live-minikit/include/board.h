/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_esp32_mh_et_live ESP32 - MH-ET LIVE MiniKit
 * @ingroup     boards
 * @brief       Board defitions for MH-ET LIVE MiniKit for ESP32
 * @{
 *
 *
 * The MH-ET LIVE MiniKit for ESP32 uses the ESP32-WROOM module. It is a
 * very interesting development kit as it is available in the stackable
 * Wemos D1 Mini format. Thus, all shields for Wemos D1 mini (ESP8266
 * platform) can also be used with ESP32. All GPIOs are broken so that
 * it can be configured very flexibly.
 *
 * Board configuration defined in this file is:
 *
 * Pin    | Defined Function         | Remarks / Prerequisites
 * :------|:-------------------------|:----------------------------------
 * GPIO0  | PWM_DEV(0):0             | |
 * GPIO2  | PWM_DEV(0):1 / LED blue  | |
 * GPIO4  | PWM_DEV(0):2             | |
 * GPIO22 | I2C_DEV(0):SCL           | defined in ```periph_cpu.h```
 * GPIO21 | I2C_DEV(0):SDA           | defined in ```periph_cpu.h```
 * GPIO18 | SPI_DEV(0):SCK           | defined in ```periph_cpu.h```
 * GPIO19 | SPI_DEV(0):MISO          | defined in ```periph_cpu.h```
 * GPIO23 | SPI_DEV(0):MOSI          | defined in ```periph_cpu.h```
 * GPIO5  | SPI_DEV(0):CS0           | defined in ```periph_cpu.h```
 * GPIO1  | UART_DEV(0):TxD          | Console (cannot be changed)
 * GPIO3  | UART_DEV(0):RxD          | Console (cannot be changed)
 * GPIO9  | UART_DEV(1):TxD          | defined in ```periph_cpu.h```
 * GPIO10 | UART_DEV(1):RxD          | defined in ```periph_cpu.h```
 * GPIO34 | ADC:0                    | |
 * GPIO35 | ADC:1                    | |
 * GPIO36 | ADC:2                    | |
 * GPIO39 | ADC:3                    | |
 * GPIO25 | DAC:0                    | |
 * GPIO26 | DAC:1                    | used as CS for MRF24J40
 * GPIO12 | Digital In/Out           | used as CS for ENC28J60
 * GPIO13 | Digital In/Out           | |
 * GPIO14 | Digital In/Out           |
 * GPIO15 | Digital In/Out           |
 * GPIO16 | Digital In/Out           | used as RESET for MRF24J40
 * GPIO17 | Digital In/Out           | used as INT for MRF24J40
 * GPIO19 | Digital In/Out           | |
 * GPIO27 | Digital In/Out           | used as RESET for ENC28J60
 * GPIO32 | Digital In/Out           | used as INT for ENC28J60
 * GPIO33 | Digital In/Out           | |
 *
 * **PLEASE NOTE:**
 * Most of the board defitions can be overriden by an application specific
 * board configuration file. For that purpose, a header file located in
 * application directory can be specified using the BOARD_APP_CONF make
 * variable at command line, for example:
 *
 *          BOARD_APP_CONF=esp32_mh_et_live_minikit_app_conf.h
 *
 * If BOARD_APP_CONF is defined, the given file is included from the
 * application source directory.
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/*
 * If BOARD_APP_CONF is defined, include the given application specific board
 * configuration file from the application source directory
 */
#ifdef BOARD_APP_CONF
#include BOARD_APP_CONF
#endif

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    ADC and DAC channel configuration
 * @{
 */
#if !defined(ADC_GPIOS) && !defined(ADC_GPIOS_NOT_AVAILABLE)
#define ADC_GPIOS   { GPIO34, GPIO35, GPIO36, GPIO39 }
#endif

#if !defined(DAC_GPIOS) && !defined(DAC_GPIOS_NOT_AVAILABLE)
#define DAC_GPIOS   { GPIO25, GPIO26 }
#endif
/** @} */


/**
 * @name    PWM channel configuration
 */
#if !defined(PWM0_GPIOS) && !defined(PWM0_GPIOS_NOT_AVAILABLE)
/** GPIOS that can be used with PWM_DEV(0) as PWM channels */
#define PWM0_GPIOS { GPIO0, GPIO2, GPIO4 }
#endif

#if !defined(PWM1_GPIOS) && !defined(PWM1_GPIOS_NOT_AVAILABLE)
/**
 * By default, PWM_DEV(1) is not used with this board. This can be changed
 * by an application-specific board configuration file located in the
 * application source directory specified by the BOARD_APP_CONF make
 * variable at command line.
 */
#define PWM1_GPIOS_NOT_AVAILABLE
#endif

/**
 * @name    Declaration of on-board LEDs
 * @{
 */
#define LED_BLUE_PIN    GPIO2

#define LED0_PIN        GPIO2
#define LED_STATE_ON    0   /* LED is low active */
#define LED_STATE_OFF   1
/** @} */

/**
 * @name    SPI_DEV(0) / VPSI configuration
 *
 * The default configuration is used.
 * @{
 */
#if !defined(SPI0_SCK) && !defined(SPI0_MISO) && !defined(SPI0_MOSI) && !defined(SPI0_CS0)
#define SPI0_SCK    GPIO18
#define SPI0_MISO   GPIO19
#define SPI0_MOSI   GPIO23
#define SPI0_CS0    GPIO5
#endif
/** @} */

/**
 * @name    SPI_DEV(1) / HSPI configuration
 *
 * SD card shield uses SPI_DEV(0) interface. Therefore, the interface is
 * declared as not available.
 * @{
 */
#if !defined(SPI1_NOT_AVAILABLE) && !defined(SPI1_SCK)
#define SPI1_NOT_AVAILABLE
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * UART_DEV(0) and UART_DEV(1) use the default configuration as defined
 * in ```periph_cpu.h```.
 * @{
 */
#if !defined(UART2_NOT_AVAILABLE) && !defined(UART2_TXD) && !defined(UART2_RXD)
/** UART_DEV(2) interface is not used with this board. */
#define UART2_NOT_AVAILABLE
#endif
/** @} */

/**
 * @name   SD card interface configuration
 *
 * SD card interface uses SPI_DEV(0) on this board to be compatible with the
 * Wemos D1 mini micro SD card shield.
 */
#if (defined(MODULE_SDCARD_SPI) && !defined(SDCARD_SPI_PARAM_SPI)) || DOXYGEN
#define SDCARD_SPI_PARAM_SPI        SPI_DEV(0)
#define SDCARD_SPI_PARAM_CS         SPI0_CS0
#define SDCARD_SPI_PARAM_CLK        SPI0_SCK
#define SDCARD_SPI_PARAM_MOSI       SPI0_MOSI
#define SDCARD_SPI_PARAM_MISO       SPI0_MISO
#define SDCARD_SPI_PARAM_POWER      GPIO_UNDEF
#endif

/**
 * @name    MRF24J40 module configuration using VPSI / SPI_DEV(0)
 * @{
 */
#if (defined(MODULE_MRF24J40) && !defined(MRF24J40_PARAM_SPI)) || DOXYGEN
#define MRF24J40_PARAM_SPI          SPI_DEV(0)
#define MRF24J40_PARAM_SPI_CLK      SPI_CLK_1MHZ
#define MRF24J40_PARAM_CS           GPIO26
#define MRF24J40_PARAM_RESET        GPIO16
#define MRF24J40_PARAM_INT          GPIO17
#endif
/** @} */

/**
 * @name    ENC28J60 module configuration using VSPI / SPI_DEV(0)
 * @{
 */
#if (defined(MODULE_ENC28J60) && !defined(ENC28J60_PARAM_SPI)) || DOXYGEN
#define ENC28J60_PARAM_SPI          SPI_DEV(0)
#define ENC28J60_PARAM_CS           GPIO12
#define ENC28J60_PARAM_RESET        GPIO27
#define ENC28J60_PARAM_INT          GPIO32
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
