/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_esp8266 ESP8266 Common
 * @ingroup     boards_common
 * @brief       Common files for the esp8266 board.
 * @{
 *
 * @file
 * @brief       Definitions that are common for all esp8266 boards.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(MODULE_MTD) || defined(DOXYGEN)
#include "mtd.h"

/** Declare the system MTD device name. */
#define MTD_0 mtd0

/** Pointer to the system MTD device. */
extern mtd_dev_t *mtd0;
#endif

#if defined(MODULE_SPIFFS) && !defined(DOXYGEN)
#define SPIFFS_ALIGNED_OBJECT_INDEX_TABLES 1
#define SPIFFS_READ_ONLY 0
#define SPIFFS_SINGLETON 0
#define SPIFFS_HAL_CALLBACK_EXTRA 1
#define SPIFFS_CACHE 1

#if SPIFFS_SINGLETON == 1
#define SPIFFS_CFG_PHYS_SZ(ignore)        (0x70000)
#define SPIFFS_CFG_PHYS_ERASE_SZ(ignore)  (4096)
#define SPIFFS_CFG_PHYS_ADDR(ignore)      (0)
#define SPIFFS_CFG_LOG_PAGE_SZ(ignore)    (256)
#define SPIFFS_CFG_LOG_BLOCK_SZ(ignore)   (65536)
#endif /* SPIFFS_SINGLETON */
#endif /* defined(MODULE_SPIFFS) && !defined(DOXYGEN) */

#if SPIFFS_HAL_CALLBACK_EXTRA == 0
/** Declare the SPIFFS MTD device name. */
#define SPIFFS_MTD_DEV (MTD_0)
#endif

#if defined(MODULE_MRF24J40) && !defined(DOXYGEN)

#define MRF24J40_PARAM_SPI_CLK      (SPI_CLK_1MHZ)
#define MRF24J40_PARAM_CS           (GPIO_PIN(0,2))
#define MRF24J40_PARAM_INT          (GPIO_PIN(0,0))
#define MRF24J40_PARAM_RESET        (GPIO_PIN(0,15))

#endif

#if defined(MODULE_ENC28J60) && !defined(DOXYGEN)

#define ENC28J60_PARAM_CS           (GPIO_PIN(0,4))
#define ENC28J60_PARAM_INT          (GPIO_PIN(0,5))
#define ENC28J60_PARAM_RESET        (GPIO_PIN(0,9)) /* only in DIO and DOUT mode */

#endif

/**
 * @brief Initialize board specific hardware
 *
 * Since all features of ESP8266 boards are provided by the MCU, almost all
 * initializations are done during the CPU initialization that is called from
 * boot loader. Therefore, this function only initializes the SPI dependent
 * on the board configuration.
 */
extern void board_init(void);

/**
 * @brief  Swith LED on or off.
 * @note   LED outputs are supposed to be low active.
 */
extern void led_on_off (uint8_t led, uint8_t value);

/** Toggle the LED status. */
extern void led_toggle (uint8_t led);
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_COMMON_H */
/** @} */
