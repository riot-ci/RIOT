/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_esp32_olimex_evb ESP32 - Olimex ESP32-EVB
 * @ingroup     boards
 * @brief       Board defitions for Olimex ESP32-EVB (ESP32-GATEWAY)
 * @{
 *
 * This board configuration is for the Olimex ESP32-EVB that might be also
 * used for (ESP32-GATEWAY). Olimex ESP32-EVB and ESP32-GATEWAY boards use the
 * ESP32-WROOM module. Both boards have a LAN interface and a MicroSD card
 * interface. While the ESP32-EVB board is a development kit with different
 * interfaces like a CAN interface, two relays, an IR interface and a 40 pin
 * connector that can be configured for different applications, the
 * ESP32-GATEWAY is preconfigured as a gateway, where only the LAN interface
 * can be used.
 *
 * Board configuration defined in this file is:
 *
 * Pin    | Defined Function       | Remarks / Prerequisites
 * :------|:-----------------------|:------------------------------------------
 * GPIO1  | UART_DEV(0):TxD        | Console (cannot be changed)
 * GPIO3  | UART_DEV(0):RxD        | Console (cannot be changed)
 * GPIO4  | UART_DEV(1):TxD        | ESP32-EVB only (available at UEXT1)
 * GPIO36 | UART_DEV(1):TxD        | ESP32-EVB only (available at UEXT1)
 * GPIO14 | SPI_DEV(1):CLK         | SD Card interface (also available at UEXT1)
 * GPIO2  | SPI_DEV(1):MISO        | SD Card interface (also available at UEXT1)
 * GPIO15 | SPI_DEV(1):MOSI        | SD Card interface (also available at UEXT1)
 * GPIO17 | SPI_DEV(1):CS          | (available at UEXT1)
 * GPIO5  | CAN_DEV(0):TX          | ESP32-EVB only
 * GPIO35 | CAN_DEV(0):RX          | ESP32-EVB only
 * GPIO12 | IR_DEV(0):Transmit     | ESP32-EVB only
 * GPIO39 | IR_DEV(0):Receive      | ESP32-EVB only
 * GPIO13 | I2C_DEV(0):SDA         | ESP32-EVB only (available at UEXT1)
 * GPIO16 | I2C_DEV(0):SCL         | ESP32-EVB only (available at UEXT1)
 * GPIO18 | EMAC_SMI:MDIO          | LAN interface
 * GPIO23 | EMAC_SMI:MDC           | LAN interface
 * GPIO0  | EMAC_RMII:TX_CLK       | LAN interface
 * GPIO21 | EMAC_RMII:TX_EN        | LAN interface
 * GPIO19 | EMAC_RMII:TXD0         | LAN interface
 * GPIO22 | EMAC_RMII:TXD1         | LAN interface
 * GPIO25 | EMAC_RMII:RXD0         | LAN interface
 * GPIO26 | EMAC_RMII:RXD1         | LAN interface
 * GPIO27 | EMAC_RMII:RX_CRS_DRV   | LAN interface
 * GPIO32 | Relais 1 / Digital Out | ESP32-EVB only
 * GPIO33 | Relais 2 / Digital Out | ESP32-EVB only
 * GPIO33 | Button / Digital In  2 | |
 * GPIO9  | Digital In/Out         | DIO and DOUT flash mode only
 * GPIO10 | Digital In/Out         | DIO and DOUT flash mode only
 *
 * **PLEASE NOTE:**
 * Most of the board defitions can be overriden by an application specific
 * board configuration file. For that purpose, a header file located in
 * application directory can be specified using the BOARD_APP_CONF make
 * variable at command line, for example:
 *
 *          BOARD_APP_CONF=esp32_olimex_evb_app_conf.h
 *
 * If BOARD_APP_CONF is defined, the given file is included from the
 * application source directory.
 *
 * **PLEASE NOTE:**
 * To use this board definition with Olimex ESP32-GATEWAY, add the definition
 *
 *          #define OLIMEX_ESP32_GATEWAY
 *
 * to the application specific board configuration file specified by make
 * variable BOARD_APP_CONF.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
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

#ifdef OLIMEX_ESP32_GATEWAY
/*
 * If we use the board definition for Olimex EVB-GATEWAY, we have to declare
 * several interfaces as not available to avoid conflicts in GPIO
 * configuration.
 */
#define I2C0_NOT_AVAILABLE
#define UART1_NOT_AVAILABLE
#endif

/**
 * @name    ADC and DAC channel configuration
 * @{
 */
#if !defined(ADC_GPIOS) && !defined(ADC_GPIOS_NOT_AVAILABLE)
/** Olimex ESP32-EVB has no GPIOs left that might be used as ADC channels. */
#define ADC_GPIOS_NOT_AVAILABLE
#endif

#if !defined(DAC_GPIOS) && !defined(DAC_GPIOS_NOT_AVAILABLE)
/** Olimex ESP32-EVB has no GPIOs left that might be used as DAC channels. */
#define DAC_GPIOS_NOT_AVAILABLE
#endif
/** @} */

/**
 * @name   I2C configuration
 * @{
 */
#if !defined(I2C0_NOT_AVAILABLE) && !defined(I2C0_SCL) && !defined(I2C0_SDA)
#define I2C0_SCL   GPIO16   /**< also available at UEXT1 */
#define I2C0_SDA   GPIO13   /**< also available at UEXT1 */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * GPIO9 and GIO10 are available and might be used with PWM_DEV(0) as PWM
 * channels only in DOUT and DIO flash mode.
 */
#if !defined(PWM0_GPIOS) && !defined(PWM0_GPIOS_NOT_AVAILABLE)
#if defined(FLASH_MODE_DOUT) || defined(FLASH_MODE_DIO)
#define PWM0_GPIOS { GPIO9, GPIO10 }
#else
#define PWM0_GPIOS { }
#endif
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
 *
 * Only Olimex ESP32-GATEWAY has an ob-board LED.
 * @{
 */
#ifdef OLIMEX_ESP32_GATEWAY
#define LED0_PIN        GPIO2
#define LED_STATE_ON    1   /**< LED is high active */
#define LED_STATE_OFF   0
#endif
/** @} */

/**
 * @name    SPI_DEV(0) / VPSI configuration
 *
 * VSPI / SPI_DEV(0) is not available on Olimex ESP32-EVB board. Therefore,
 * the interface is declared as not available.
 * @{
 */
#if !defined(SPI0_NOT_AVAILABLE) && !defined(SPI0_SCK)
#define SPI0_NOT_AVAILABLE
#endif
/** @} */

/**
 * @name    SPI_DEV(1) / HSPI configuration
 *
 * SD card shield uses SPI_DEV(1)
 * @{
 */
#if !defined(SPI1_NOT_AVAILABLE) && !defined(SPI1_SCK)
#define SPI1_SCK    GPIO14  /**< SD card interface CLK  (also available at UEXT1) */
#define SPI1_MISO   GPIO2   /**< SD card interface MISO (also available at UEXT1) */
#define SPI1_MOSI   GPIO15  /**< SD card interface_MOSI (also available at UEXT1) */
#define SPI1_CS0    GPIO17  /**< connected to SD Card interface (available  at UEXT1) */
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
/** UART_DEV(1) interface */
#define UART1_TXD   GPIO4   /**< ESP32-EVB only (available at UEXT1) */
#define UART1_RXD   GPIO36  /**< ESP32-EVB only (available at UEXT1) */
#endif

#if !defined(UART2_NOT_AVAILABLE) && !defined(UART2_TXD) && !defined(UART2_RXD)
/** UART_DEV(2) interface is not used with this board. */
#define UART2_NOT_AVAILABLE
#endif
/** @} */

/**
 * @name SD card interface configuration
 *
 * SD card interface uses SPI_DEV(1) on this board.
 *
 * PLEASE NOTE: On Olimex ESP32-EVB, the CD/CS pin is not connected and
 * simply pulled-up. Therefore, SPI bus mode is not available and the card
 * interface can be used only in 1-bit SD bus mode. That is, SPIP SD card
 * is not working. On Olimex ESP32-GATEWAY, the CD/CS pin is connected to
 * GPIO13. The SPI SD card driver should work on this board.
 * @{
 */
#ifdef OLIMEX_ESP32_GATEWAY
#if (defined(MODULE_SDCARD_SPI) && !defined(SDCARD_SPI_PARAM_SPI)) || DOXYGEN
#define SDCARD_SPI_PARAM_SPI        SPI_DEV(1)
#define SDCARD_SPI_PARAM_CS         GPIO13
#define SDCARD_SPI_PARAM_CLK        SPI1_SCK
#define SDCARD_SPI_PARAM_MOSI       SPI1_MOSI
#define SDCARD_SPI_PARAM_MISO       SPI1_MISO
#define SDCARD_SPI_PARAM_POWER      GPIO_UNDEF
#endif
#endif
/** @} */

/**
 * @name    MRF24J40 module configuration using VPSI / SPI_DEV(0)
 *
 * The configuration might only be used in DIO or DOUT flash mode.
 * @{
 */
#if (defined(MODULE_MRF24J40) && !defined(MRF24J40_PARAM_SPI)) || DOXYGEN
#if defined(FLASH_MODE_DOUT) || defined(FLASH_MODE_DIO)
#define MRF24J40_PARAM_SPI          SPI_DEV(1)
#define MRF24J40_PARAM_SPI_CLK      SPI_CLK_1MHZ
#define MRF24J40_PARAM_CS           GPIO9
#define MRF24J40_PARAM_RESET        GPIO10
#define MRF24J40_PARAM_INT          GPIO34 /**< Button cannot be used in that case */
#else
#error GPIO9 and GPIO10 are not availabl in QOUT or QIO flash mode
#endif
#endif
/** @} */

/**
 * @name    ESP32 Ethernet (EMAC) configuration
 * @{
 */
#if !defined(EMAC_NOT_AVAILABLE) || DOXYGEN
#define EMAC_PHY_LAN8720        1                   /**< LAN8710 used as PHY interface */
#define EMAC_PHY_ADDRESS        0                   /**< PHY0 used as base address */
#define EMAC_PHY_SMI_MDC_PIN    23                  /**< SMI MDC pin */
#define EMAC_PHY_SMI_MDIO_PIN   18                  /**< SMI MDC pin */
#define EMAC_PHY_CLOCK_MODE     ETH_CLOCK_GPIO0_IN  /**< external 50 MHz clock */
#define EMAC_PHY_POWER_PIN      GPIO_UNDEF          /**< power enable pin not used */
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
