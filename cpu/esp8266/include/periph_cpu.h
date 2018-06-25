/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       CPU specific definitions and functions for peripheral handling
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>

#include "eagle_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
#define PROVIDES_PM_RESTART
#define PROVIDES_PM_OFF
/** @} */


/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (4U)

/**
 * @brief   Available ports on the ESP8266
 * @{
 */
#define PORT_GPIO 0       /**< port GPIO */
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF (GPIO_ID_NONE)

/**
 * @brief   Define CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)  ((x << 4) | y)

/**
 * @brief   Define CPU specific number of GPIO pins
 */
#define GPIO_PIN_NUMOF  GPIO_PIN_COUNT+1

/**
 * @name   Predefined GPIO names
 * @{
 */
#define GPIO0       (GPIO_PIN(PORT_GPIO,0))
#define GPIO1       (GPIO_PIN(PORT_GPIO,1))
#define GPIO2       (GPIO_PIN(PORT_GPIO,2))
#define GPIO3       (GPIO_PIN(PORT_GPIO,3))
#define GPIO4       (GPIO_PIN(PORT_GPIO,4))
#define GPIO5       (GPIO_PIN(PORT_GPIO,5))
#define GPIO6       (GPIO_PIN(PORT_GPIO,6))
#define GPIO7       (GPIO_PIN(PORT_GPIO,7))
#define GPIO8       (GPIO_PIN(PORT_GPIO,8))
#define GPIO9       (GPIO_PIN(PORT_GPIO,9))
#define GPIO10      (GPIO_PIN(PORT_GPIO,10))
#define GPIO11      (GPIO_PIN(PORT_GPIO,11))
#define GPIO12      (GPIO_PIN(PORT_GPIO,12))
#define GPIO13      (GPIO_PIN(PORT_GPIO,13))
#define GPIO14      (GPIO_PIN(PORT_GPIO,14))
#define GPIO15      (GPIO_PIN(PORT_GPIO,15))
#define GPIO16      (GPIO_PIN(PORT_GPIO,16))
/** @} */

#ifndef DOXYGEN
#define GPIO0_MASK  (BIT(0))
#define GPIO1_MASK  (BIT(1))
#define GPIO2_MASK  (BIT(2))
#define GPIO3_MASK  (BIT(3))
#define GPIO4_MASK  (BIT(4))
#define GPIO5_MASK  (BIT(5))
#define GPIO6_MASK  (BIT(6))
#define GPIO7_MASK  (BIT(7))
#define GPIO8_MASK  (BIT(8))
#define GPIO9_MASK  (BIT(9))
#define GPIO10_MASK (BIT(10))
#define GPIO11_MASK (BIT(11))
#define GPIO12_MASK (BIT(12))
#define GPIO13_MASK (BIT(13))
#define GPIO14_MASK (BIT(14))
#define GPIO15_MASK (BIT(15))
#define GPIO16_MASK (BIT(16))

/**
 * @brief   Override flank selection values
 *
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_NONE    = 0,
    GPIO_RISING  = 1,        /**< emit interrupt on rising flank  */
    GPIO_FALLING = 2,        /**< emit interrupt on falling flank */
    GPIO_BOTH    = 3,        /**< emit interrupt on both flanks   */
    GPIO_LOW     = 4,        /**< emit interrupt on low level     */
    GPIO_HIGH    = 5         /**< emit interrupt on low level     */
} gpio_flank_t;
/** @} */

#endif /* DOXYGEN */

/**
 * @name   SPI configuration
 *
 * Since SPI(0) is used for on-board/on-chip flash memory, it is not
 * possible to use it for peripharals. Therefore the only usable SPI
 * interface is SPI(1) which is mapped to RIOT's SPI_DEV(0).
 *
 * SPI pin configuration is fixed. Only CS signals can be defined
 * separatly.
 *
 * SPI_DEV(0).MISO    GPIO12
 * SPI_DEV(0).MOSI    GPIO13
 * SPI_DEV(0).SCK     GPIO14
 * SPI_DEV(0).CSx     GPIOn with n = 0|2|4|5|15 (+9|10 in DOUT flash mode)
 * @{
 */
#if defined(SPI_USED)

#define SPI_DEV(x)  ((unsigned int)(x+1))
#define SPI_NUMOF   1

#define SPI_MISO_GPIO   GPIO12
#define SPI_MOSI_GPIO   GPIO13
#define SPI_SCK_GPIO    GPIO14
#define SPI_CS0_GPIO    GPIO15

#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS

#endif /* SPI_USED */
/** @} */

/**
 * @name   UART cofiguration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#define UART_NUMOF   1
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
