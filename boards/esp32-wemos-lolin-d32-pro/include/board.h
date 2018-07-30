/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_esp32_wemos_lolin_d32_pro ESP32 - Wemos LOLIN D32 Pro
 * @ingroup     boards
 * @brief       Board defitions for Wemos LOLIN D32 Pro
 * @{
 *
 *
 * The Wemos D32 Pro is a development board that uses the ESP32-WROVER
 * module which includes a built-in 4 MByte SPI RAM. Most important
 * features of the board are
 *
 * - Micro-SD card interface
 * - LCD interface
 *
 * Furthermore, many GPIOs are broken out for extension.
 *
 * Board configuration defined in this file is:
 *
 * Pin    | Defined Function         | Remarks / Prerequisites
 * :------|:-------------------------|:----------------------------------
 * GPIO5  | PWM_DEV(0):0 / LED       | |
 * GPIO22 | I2C_DEV(0):SCL           | defined in ```periph_cpu.h```
 * GPIO21 | I2C_DEV(0):SDA           | defined in ```periph_cpu.h```
 * GPIO18 | SPI_DEV(0):SCK           | |
 * GPIO19 | SPI_DEV(0):MISO          | |
 * GPIO23 | SPI_DEV(0):MOSI          | |
 * GPIO4  | SPI_DEV(0):CS0           | |
 * GPIO1  | UART_DEV(0):TxD          | Console (cannot be changed)
 * GPIO3  | UART_DEV(0):RxD          | Console (cannot be changed)
 * GPIO35 | ADC:0                    | VBat (not broken out)
 * GPIO34 | ADC:1                    | |
 * GPIO36 | ADC:2                    | |
 * GPIO39 | ADC:3                    | |
 * GPIO32 | ADC:4                    | available if LCD is not connected
 * GPIO33 | ADC:5                    | available if LCD is not connected
 * GPIO25 | DAC:0                    | |
 * GPIO26 | DAC:1                    | |
 * GPIO0  | Digital In/Out           | used as CS for MRF24J40, ENC28J60
 * GPIO2  | Digital In/Out           | used as RESET for MRF24J40, ENC28J60
 * GPIO13 | Digital In/Out           | used as INT for MRF24J40, ENC28J60
 * GPIO15 | Digital In/Out           | |
 * GPIO12 | Digital In/Out           | available if LCD is not connected
 * GPIO14 | Digital In/Out           | available if LCD is not connected
 * GPIO27 | Digital In/Out           | available if LCD is not connected
 *
 * **PLEASE NOTE:**
 * Most of the board defitions can be overriden by an application specific
 * board configuration file. For that purpose, a header file located in
 * application directory can be specified using the BOARD_APP_CONF make
 * variable at command line, for example:
 *
 *          BOARD_APP_CONF=esp32_wwemos_lolin_d32_pro_app_conf.h
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

/* Board specific definitions */

/**
 * Set this definition to 1 when LCD is connected.
 * It can be changed during make by adding the CONFIGS make variable at
 * command line: CONFIGS='-DESP_LCD_PLUGGED_IN=1'. Alternatively, it
 * can be defined in an application specific board configuration file located
 * in application directory can be specified using the BOARD_APP_CONF make
 * variable at command line.
 */
#ifndef ESP_LCD_PLUGGED_IN
#define ESP_LCD_PLUGGED_IN   1
#endif

/**
 * @name    ADC and DAC channel configuration
 * @{
 */
#if !defined(ADC_GPIOS) && !defined(ADC_GPIOS_NOT_AVAILABLE)
#if !ESP_LCD_PLUGGED_IN
#define ADC_GPIOS   { GPIO35, GPIO34, GPIO36, GPIO39, GPIO32, GPIO33 }
#else
#define ADC_GPIOS   { GPIO35, GPIO34, GPIO36, GPIO39 }
#endif
#endif

#if !defined(DAC_GPIOS) && !defined(DAC_GPIOS_NOT_AVAILABLE)
#define DAC_GPIOS   { GPIO25, GPIO26 }
#endif
/** @} */

/**
 * @name   PWM channel configuration
 */
#if !defined(PWM0_GPIOS) && !defined(PWM0_GPIOS_NOT_AVAILABLE)
/** GPIOS that can be used with PWM_DEV(0) as PWM channels */
#define PWM0_GPIOS { GPIO5 }
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
 * @name LED (on-board) configuration
 * @{
 */
#define LED0_PIN        GPIO5
#define LED_STATE_ON    0   /* LED is low active */
#define LED_STATE_OFF   1
/** @} */

/**
 * @name    SPI_DEV(0) / VPSI configuration
 *
 * SPI_DEV(0) is used for SD card and LCD interface. It can also be used
 * by other peripherals with different CS signals.
 * @{
 */
#if !defined(SPI0_SCK) && !defined(SPI0_MISO) && !defined(SPI0_MOSI) && !defined(SPI0_CS0)
#define SPI0_SCK    GPIO18
#define SPI0_MISO   GPIO19
#define SPI0_MOSI   GPIO23
#define SPI0_CS0    GPIO4
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
 * UART_DEV(0) uses the default configuration as defined
 * in ```periph_cpu.h```.
 * @{
 */
#if !defined(UART1_NOT_AVAILABLE) && !defined(UART1_TXD) && !defined(UART1_RXD)
/** UART_DEV(1) interface is not used with this board. */
#define UART1_NOT_AVAILABLE
#endif

#if !defined(UART2_NOT_AVAILABLE) && !defined(UART2_TXD) && !defined(UART2_RXD)
/** UART_DEV(2) interface is not used with this board. */
#define UART2_NOT_AVAILABLE
#endif
/** @} */

/**
 * @name   SD card interface configuration
 *
 * SD card interface uses SPI_DEV(0) on this board.
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
 *
 * Please note: It uses the same CS, RESET and INT configuration as module
 * ENC28J60. If both modules are used simultaneously, the configuration of one
 * module has to be changed in an application-specific board configuration
 * file located in the application source directory specified by the
 * BOARD_APP_CONF make variable at command line.
 * @{
 */
#if (defined(MODULE_MRF24J40) && !defined(MRF24J40_PARAM_SPI)) || DOXYGEN
#define MRF24J40_PARAM_SPI          SPI_DEV(0)
#define MRF24J40_PARAM_SPI_CLK      SPI_CLK_1MHZ
#define MRF24J40_PARAM_CS           GPIO0
#define MRF24J40_PARAM_RESET        GPIO2
#define MRF24J40_PARAM_INT          GPIO13
#endif
/** @} */

/**
 * @name ENC28J60 module configuration using VSPI / SPI_DEV(0)
 *
 * Please note: If both modules are used simultaneously, the configuration of one
 * module has to be changed in an application-specific board configuration
 * file located in the application source directory specified by the
 * BOARD_APP_CONF make variable at command line.
 * @{
 */
#if (defined(MODULE_ENC28J60) && !defined(ENC28J60_PARAM_SPI)) || DOXYGEN
#define ENC28J60_PARAM_SPI          SPI_DEV(0)
#define ENC28J60_PARAM_CS           GPIO0
#define ENC28J60_PARAM_RESET        GPIO2
#define ENC28J60_PARAM_INT          GPIO13
#endif
/** @} */


#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
