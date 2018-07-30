/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_esp32_wrover_kit ESP32 - ESP-WROVER-KIT V3
 * @ingroup     boards
 * @brief       Board defitions for Espressif ESP-WROVER-KIT V3
 * @{
 *
 *
 * The Espressif ESP-WROVER-KIT is a development board that uses the
 * ESP32-WROVER module which includes a built-in 4 MByte SPI RAM. Most
 * important features of the board are
 *
 * - Micro-SD card interface
 * - OV7670 camera interface
 * - 3.2" SPI LCD panel
 * - RGB LED
 *
 * Furthermore, many GPIOs are broken out for extension. The USB bridge
 * based on FDI FT2232HL provides a JTAG interface for debugging through
 * the USB interface.
 *
 * Board configuration defined in this file is:
 *
 * Pin    | Defined Function         | Prerequisites / Remarks
 * :------|:-------------------------|:----------------------------------
 * GPIO0  | LED red   / PWM_DEV(0):0 | Camera not connected
 * GPIO2  | LED green / PWM_DEV(0):1 | SD Card interface not used
 * GPIO4  | LED blue  / PWM_DEV(0):2 | Camera not connected
 * GPIO5  | I2C_DEV(0):SCL           | Camera not connected
 * GPIO27 | I2C_DEV(0):SDA           | Camera not connected
 * GPIO19 | SPI_DEV(0):SCK           | Camera not connected
 * GPIO23 | SPI_DEV(0):MOSI          | Camera not connected
 * GPIO25 | SPI_DEV(0):MISO          | Camera not connected
 * GPIO22 | SPI_DEV(0):CS0           | Camera not connected
 * GPIO1  | UART_DEV(0):TxD          | Console (cannot be changed)
 * GPIO3  | UART_DEV(0):RxD          | Console (cannot be changed)
 * GPIO26 | DAC:0                    | Camera not connected
 * GPIO12 | ADC:0                    | |
 * GPIO36 | ADC:1                    | Camera not connected
 * GPIO39 | ADC:2                    | Camera not connected
 * GPIO9  | Digital In/Out           | SPI RAM not used, used as CS for MRF24J40, ENC28J60
 * GPIO10 | Digital In/Out           | SPI RAM not used, used as RESET for MRF24J40, ENC28J60
 * GPIO13 | Digital In/Out           | SD Card interface not used
 * GPIO14 | Digital In/Out           | SD Card interface not used
 * GPIO15 | Digital In/Out           | SD Card interface not used
 * GPIO34 | Digital In               | used as INT for MRF24J40, ENC28J60
 * GPIO35 | Digital In               | |
 *
 * **PLEASE NOTE:**
 * The LCD display and the camera can not be used at the same time since
 * both devices use a number of same signals. Furthermore, the availability
 * of some peripherals depend on the use of the camera and the SD card
 * interface, respectively.
 *
 * **PLEASE NOTE:**
 * Most of the board defitions can be overriden by an application specific
 * board configuration file. For that purpose, a header file located in
 * application directory can be specified using the BOARD_APP_CONF make
 * variable at command line, for example:
 *
 *          BOARD_APP_CONF=esp32_wrover_kit_app_conf.h
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
 * Set this definition to 1 when the camera is connected.
 * It can be changed during make by adding the CONFIGS make variable at
 * command line: CONFIGS='-DESP_CAMERA_PLUGGED_IN=1'. Alternatively, it
 * can be defined in the application specific board configuration file located
 * in application directory can be specified using the BOARD_APP_CONF make
 * variable at command line.
 */
#ifndef ESP_CAMERA_PLUGGED_IN
#define ESP_CAMERA_PLUGGED_IN   0
#endif

/**
 * @name ADC channel configuration
 * @{
 */
#if !defined(ADC_GPIOS) && !defined(ADC_GPIOS_NOT_AVAILABLE)
#if !ESP_CAMERA_PLUGGED_IN
/** These ADC Channels can only be used if the camera is not plugged in. */
#define ADC_GPIOS   { GPIO12, GPIO36, GPIO39 }
#else
#define ADC_GPIOS_NOT_AVAILABLE
#endif /* ESP_CAMERA_PLUGGED_IN == 0 */
#endif /* ADC_GPIOS */
/** @} */

/**
 * @name DAC channel configuration
 * @{
 */
#if !defined(DAC_GPIOS) && !defined(DAC_GPIOS_NOT_AVAILABLE)
#if !ESP_CAMERA_PLUGGED_IN
/** These DAC Channels can only be used if the camera is not plugged in. */
#define DAC_GPIOS   { GPIO26 }
#else
#define DAC_GPIOS_NOT_AVAILABLE
#endif /* ESP_CAMERA_PLUGGED_IN == 0 */
#endif /* DAC_GPIOS */
/** @} */

/**
 * @name   I2C configuration
 *
 * If the camera is connected, this interface is not available because the
 * camera interface uses some of the signals.
 * @{
 */
#if !defined(I2C0_NOT_AVAILABLE) && !defined(I2C0_SCL) && !defined(I2C0_SDA)
#if !ESP_CAMERA_PLUGGED_IN
#define I2C0_SCL   GPIO5
#define I2C0_SDA   GPIO27
#else
#define I2C0_NOT_AVAILABLE
#endif /* ESP_CAMERA_PLUGGED_IN == 0 */
#endif /* !defined(I2C0_NOT_AVAILABLE) && !defined(I2C0_SCL) && !defined(I2C0_SDA) */
/** @} */


/**
 * @name    SPI_DEV(0) / VPSI configuration
 *
 * The interface is used by the on-board LCD interface. It can also be used
 * to connect other peripherals with different CS signals.
 *
 * If the camera is connected, this interface is not available because the
 * camera interface uses some of the signals.
 * @{
 */
#if !defined(SPI0_NOT_AVAILABLE) && !defined(SPI0_SCK)
#if !ESP_CAMERA_PLUGGED_IN
#define SPI0_CS0        GPIO22  /**< LCD CS   */
#define SPI0_SCK        GPIO19  /**< LCD SCL, can be used to connect peripherals */
#define SPI0_MOSI       GPIO23  /**< LCD MOSI, can be used to connect peripherals */
#define SPI0_MISO       GPIO25  /**< LCD MISO, can be used to connect peripherals */
#else
#define SPI0_NOT_AVAILABLE
#endif /* ESP_CAMERA_PLUGGED_IN == 0 */
#endif /* !defined(SPI0_NOT_AVAILABLE) && !defined(SPI0_SCK) */
/** @} */

/**
 * @name    SPI_DEV(1) / HSPI configuration
 *
 * The interface is used by the on-board SD card interface. In this case the
 * green LED cannot be used.
 * @{
 */
#if !defined(SPI1_NOT_AVAILABLE) && !defined(SPI1_SCK)
#if MODULE_SDCARD_SPI || DOXYGEN
#define SPI1_SCK        GPIO14  /**< SD card interface CLK  */
#define SPI1_MISO       GPIO2   /**< SD card interface MISO */
#define SPI1_MOSI       GPIO15  /**< SD card interface_MOSI */
#define SPI1_CS0        GPIO13  /**< SD card interface CS   */
#else
#define SPI1_NOT_AVAILABLE
#endif /* MODULE_SDCARD_SPI */
#endif /* !defined(SPI1_NOT_AVAILABLE) && !defined(SPI1_SCK) */
/** @} */

/**
 * @name   PWM channel configuration
 */
#if !defined(PWM0_GPIOS) && !defined(PWM0_GPIOS_NOT_AVAILABLE)
#if (!ESP_CAMERA_PLUGGED_IN && defined(SPI1_NOT_AVAILABLE)) || DOXYGEN
/** LED pins are used with PWM_DEV(0) as PWM channels */
#define PWM0_GPIOS { GPIO0, GPIO2, GPIO4 }
#elif !ESP_CAMERA_PLUGGED_IN && !defined(SPI1_NOT_AVAILABLE)
#define PWM0_GPIOS { GPIO0, GPIO4 }
#elif ESP_CAMERA_PLUGGED_IN && defined(SPI1_NOT_AVAILABLE)
#define PWM0_GPIOS { GPIO2 }
#else
#define PWM0_GPIOS_NOT_AVAILABLE
#endif
#endif /* !defined(PWM0_GPIOS) && !defined(PWM0_GPIOS_NOT_AVAILABLE) */

#if !defined(PWM1_GPIOS) && !defined(PWM1_GPIOS_NOT_AVAILABLE)
/** PWM_DEV(1) is not used. */
#define PWM1_GPIOS_NOT_AVAILABLE
#endif

/**
 * @name   UART configuration
 *
 * There are no additional UART interfaces defined here. If addtional
 * UART interface are neede, they have to be defined in an application
 * specific board definition file.
 */
#if !defined(UART1_NOT_AVAILABLE) && !defined(UART1_TXD) && !defined(UART1_RXD)
#define UART1_NOT_AVAILABLE
#endif

#if !defined(UART2_NOT_AVAILABLE) && !defined(UART2_TXD) && !defined(UART2_RXD)
#define UART2_NOT_AVAILABLE
#endif

/**
 * @name LED (on-board) configuration
 * @{
 */
#define LED_STATE_ON    1   /* LEDs are high active */
#define LED_STATE_OFF   0

#if !ESP_CAMERA_PLUGGED_IN || DOXYGEN /* camera is not plugged in */
#define LED0_PIN        GPIO0
#define LED2_PIN        GPIO4
#define LED_RED_PIN     GPIO0
#define LED_BLUE_PIN    GPIO4
#endif

#if defined(SPI1_NOT_AVAILABLE) || DOXYGEN /* SD card interface is not used */
#define LED1_PIN        GPIO2
#define LED_GREEN_PIN   GPIO2
#endif

/** @} */

/**
 * @name MRF24J40 module configuration using VPSI / SPI_DEV(0)
 *
 * Please note: It uses the same CS, RESET and INT configuration as module
 * ENC28J60. If both modules are used simultaneously, the configuration of one
 * module has to be changed in an application-specific board configuration
 * file located in the application source directory specified by the
 * BOARD_APP_CONF make variable at command line.
 * @{
 */
#if (defined(MODULE_MRF24J40) && !defined(MRF24J40_PARAM_SPI)) || DOXYGEN
#if SPI_RAM_USED
#error GPIO9 and GPIO10 are not availabl when SPI RAM is activated
#else
#define MRF24J40_PARAM_SPI          SPI_DEV(0)
#define MRF24J40_PARAM_SPI_CLK      SPI_CLK_1MHZ
#define MRF24J40_PARAM_CS           GPIO9
#define MRF24J40_PARAM_RESET        GPIO10
#define MRF24J40_PARAM_INT          GPIO34
#endif /* SPI_RAM_USED */
#endif /* defined(MODULE_MRF24J40) && !defined(MRF24J40_PARAM_SPI) */
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
#if SPI_RAM_USED
#error GPIO9 and GPIO10 are not availabl when SPI RAM is activated
#else
#define ENC28J60_PARAM_SPI          SPI_DEV(0)
#define ENC28J60_PARAM_CS           GPIO9
#define ENC28J60_PARAM_RESET        GPIO10
#define ENC28J60_PARAM_INT          GPIO34
#endif /* SPI_RAM_USED */
#endif /* defined(MODULE_ENC28J60) && !defined(ENC28J60_PARAM_SPI) */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
