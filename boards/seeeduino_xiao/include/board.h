/*
 * Copyright (C) 2021 Franz Freitag, Justus Krebs, Nick Weiler
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_seeeduino_xiao
 * @brief       Support for the Seeeduino XIAO board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Seeeduino XIAO
 *
 * @author      Franz Freitag <franz.freitag@st.ovgu.de>
 * @author      Justus Krebs <justus.krebs@st.ovgu.de>
 * @author      Nick Weiler <nick.weiler@st.ovgu.de>
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED_PORT            PORT->Group[PA]

#define LED0_PIN            GPIO_PIN(PA, 18)
#define LED0_MASK           (1 << 18)
#define LED0_NAME           "LED(BLUE_RX)"

#define LED0_OFF            (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_ON             (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)

#define LED1_PIN            GPIO_PIN(PA, 19)
#define LED1_MASK           (1 << 19)
#define LED1_NAME           "LED(BLUE_TX)"

#define LED1_OFF            (LED_PORT.OUTSET.reg = LED1_MASK)
#define LED1_ON             (LED_PORT.OUTCLR.reg = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT.OUTTGL.reg = LED1_MASK)

#define LED2_PIN            GPIO_PIN(PA, 17)
#define LED2_MASK           (1 << 17)
#define LED2_NAME           "LED(YELLOW_USER)"

#define LED2_OFF            (LED_PORT.OUTSET.reg = LED2_MASK)
#define LED2_ON             (LED_PORT.OUTCLR.reg = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT.OUTTGL.reg = LED2_MASK)
/** @} */

/**
 * @name Seeeduino XIAO NOR flash hardware configuration
 * @{
 */
#define SEEEDUINO_XIAO_NOR_PAGE_SIZE          (256)
#define SEEEDUINO_XIAO_NOR_PAGES_PER_SECTOR   (16)
#define SEEEDUINO_XIAO_NOR_SECTOR_COUNT       (1024)
#define SEEEDUINO_XIAO_NOR_FLAGS              (SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K)
#define SEEEDUINO_XIAO_NOR_SPI_DEV            SPI_DEV(0)
#define SEEEDUINO_XIAO_NOR_SPI_CLK            SPI_CLK_10MHZ
#define SEEEDUINO_XIAO_NOR_SPI_CS             GPIO_PIN(PA, 15)
#define SEEEDUINO_XIAO_NOR_SPI_MODE           SPI_MODE_3
/** @} */

/**
 * @name MTD configuration
 * @{
 */
extern mtd_dev_t *mtd0;
#define MTD_0 mtd0
/** @} */

/**
 * @name USB configuration
 * @{
 */
#define INTERNAL_PERIPHERAL_VID         (0x239A)
#define INTERNAL_PERIPHERAL_PID         (0x0057)
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
