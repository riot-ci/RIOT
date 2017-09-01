/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_adafruit-feather-m0 Adafruit Feather M0
 * @ingroup     boards
 * @brief       Support for the Adafruit Feather M0.
 *
 * ### General information
 *
 * Feather M0 boards are development boards shipped by
 * [Adafruit](https://learn.adafruit.com/adafruit-feather-m0-basic-proto/).
 *
 * All the feather M0 boards are built based on the same Atmel SAMD21G18A
 * microcontroller. See @ref cpu_samd21.
 *
 * Several types of Feather M0 boards exist:
 * * [Feather M0 WiFi](https://learn.adafruit.com/adafruit-feather-m0-wifi-atwinc1500/)
 * * [Feather M0 BLE](https://learn.adafruit.com/adafruit-feather-m0-bluefruit-le/overview)
 * * [Feather M0 Adalogger](https://learn.adafruit.com/adafruit-feather-m0-adalogger/)
 * * [Feather M0 LoRa](https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module)
 *
 * The different modules used to differenciate the boards (ATWINC1500 WiFi,
 * Bluefruit LE, SD card, LoRa) are connected via SPI (SPI_DEV(0)) to the
 * SAMD21 mcu.
 *
 * ### Pinout
 *
 * <img src="https://cdn-learn.adafruit.com/assets/assets/000/030/921/original/adafruit_products_2772_pinout_v1_0.png"
 *      alt="Adafruit Feather M0 proto pinout" style="width:800px;"/>
 *
 * ### Flash the board
 *
 * 1. Put the board in bootloader mode by double tapping the reset button.<br/>
 *    When the board is in bootloader mode, the user led (red) oscillates smoothly.
 *
 *
 * 2. Use `BOARD=feather-m0` with the `make` command.<br/>
 *    Example with `hello-world` application:
 * ```
 *      make BOARD=feather-m0 -C examples/hello-world flash
 * ```
 *
 * ### Accessing STDIO via UART
 *
 * To access the STDIO of RIOT, a FTDI to USB converted needs to be plugged to
 * the RX/TX pins on the board.
 *
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Adafruit Feather M0
 *              board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 17)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 17)

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
