/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp32
 * @brief       Common declarations of ESP32 periphery for all ESP32 boards
 *
 * This file contains default declarations that are valid for all ESP32. These
 * default configurations can be overriden in *board.h* with board specific
 * declarations.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* load default periphery configurations provided by the CPU first */
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

#if defined(MODULE_MRF24J40) && !defined(MRF24J40_PARAM_SPI)
/** default configuration for MRF24J40 using VSPI / SPI_DEV(0) and GPIO2 as CS */
#define MRF24J40_PARAM_SPI          SPI_DEV(0)
#define MRF24J40_PARAM_SPI_CLK      SPI_CLK_1MHZ
#define MRF24J40_PARAM_CS           GPIO2
#define MRF24J40_PARAM_RESET        GPIO32
#define MRF24J40_PARAM_INT          GPIO34
#endif /* defined(MODULE_MRF24J40) && !defined(MRF24J40_PARAM_SPI) */

#if defined(MODULE_ENC28J60) && !defined(ENC28J60_PARAM_SPI)
/** default configuration for ENC28J60 using VSPI / SPI_DEV(0) and GPIO2 as CS */
#define ENC28J60_PARAM_SPI          SPI_DEV(0)
#define ENC28J60_PARAM_CS           GPIO4
#define ENC28J60_PARAM_RESET        GPIO33
#define ENC28J60_PARAM_INT          GPIO35
#endif /* defined(MODULE_ENC28J60) && !defined(ENC28J60_PARAM_SPI) */

#if defined(MODULE_SDCARD_SPI) && !defined(SDCARD_SPI_PARAM_SPI)
/** default configuration for SDCARD module using HSPI / SPI_DEV(1) with default CS */
#define SDCARD_SPI_PARAM_SPI        SPI_DEV(1)
#define SDCARD_SPI_PARAM_CS         SPI1_CS0
#define SDCARD_SPI_PARAM_CLK        SPI1_SCK
#define SDCARD_SPI_PARAM_MOSI       SPI1_MOSI
#define SDCARD_SPI_PARAM_MISO       SPI1_MISO
#define SDCARD_SPI_PARAM_POWER      GPIO_UNDEF
#endif /* defined(MODULE_SDCARD_SPI) && !defined(SDCARD_SPI_PARAM_SPI) */

/**
 * @name   LED configuration (three predefined LEDs at maximum)
 * @{
 */
extern void gpio_write (uint32_t pin, int value);
extern void gpio_toggle(uint32_t pin);
#ifdef LED0_PIN
/** if LED0 pin defined, declare according macros for handling */
#define LED0_MASK       (BIT(LED0_PIN))
#define LED0_ON         gpio_write(LED0_PIN, LED_STATE_ON)
#define LED0_OFF        gpio_write(LED0_PIN, LED_STATE_OFF)
#define LED0_TOGGLE     gpio_toggle(LED0_PIN)
#endif

#ifdef LED1_PIN
/** if LED1 pin defined, declare according macros for handling */
#define LED1_MASK       (BIT(LED1_PIN))
#define LED1_ON         gpio_write(LED1_PIN, LED_STATE_ON)
#define LED1_OFF        gpio_write(LED1_PIN, LED_STATE_OFF)
#define LED1_TOGGLE     gpio_toggle(LED1_PIN)
#endif

#ifdef LED2_PIN
/** if LED2 pin defined, declare according macros for handling */
#define LED2_MASK       (BIT(LED2_PIN))
#define LED2_ON         gpio_write(LED2_PIN, LED_STATE_ON)
#define LED2_OFF        gpio_write(LED2_PIN, LED_STATE_OFF)
#define LED2_TOGGLE     gpio_toggle(LED2_PIN)
#endif
/** @} */

#endif  /* !DOXYGEN */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H */
/** @} */
