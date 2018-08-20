/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp8266
 * @brief       Configurations of MCU periphery that are common for all ESP8266 boards
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* #include "c_types.h" */

/* include board.h and cpu_conf.h to make them visible in any case */
#include "board.h"
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

#ifndef BIT
#define BIT(X) (1<<(X))
#endif

/**
 * @brief   Override the ADC resolution configuration
 * @{
 */
#define HAVE_ADC_RES_T
 typedef enum {
     ADC_RES_10BIT      /* only one resolution is supported */
 } adc_res_t;
/** @} */

#ifdef LED0_PIN
#define LED0_MASK       (BIT(LED0_PIN))
#define LED0_ON         esp8266_led_on_off (LED0_PIN, 1)
#define LED0_OFF        esp8266_led_on_off (LED0_PIN, 0)
#define LED0_TOGGLE     esp8266_led_toggle (LED0_PIN)
#endif

#ifdef LED1_PIN
#define LED1_MASK       (BIT(LED1_PIN))
#define LED1_ON         esp8266_led_on_off (LED1_PIN, 1)
#define LED1_OFF        esp8266_led_on_off (LED1_PIN, 0)
#define LED1_TOGGLE     esp8266_led_toggle (LED1_PIN)
#endif

#if defined(MODULE_ESP_SW_TIMER)

/* software timer */
#define TIMER_NUMOF        (1)
#define TIMER_CHANNELS     (10)

#define XTIMER_OVERHEAD    (0)
#define XTIMER_BACKOFF     (100)
#define XTIMER_ISR_BACKOFF (100)

#else /* MODULE_ESP_SW_TIMER */

/* hardware timer */
#define TIMER_NUMOF     (1)
#define TIMER_CHANNELS  (1)

#define XTIMER_OVERHEAD (0)

#endif /* MODULE_ESP_SW_TIMER */

/**
 * @brief   PWM configuration
 * @{
 */

/**
 * @brief   Number of PWM devices, can be 0 or 1.
 */
#define PWM_NUMOF           (1)

/**
 * @brief   Maximum number of channels of the PWM device.
 */
#define PWM_CHANNEL_NUM_MAX (8)

/**
 * @brief   Declaration which GPIOs can be used as PWM channels.
 *
 * Declare up to PWM_CHANNEL_NUM_MAX GPIOs as PWM channels. GPIOs with duty
 * value 0 can be used as normal GPIOs.
 */
#ifndef PWM_CHANNEL_GPIOS
#define PWM_CHANNEL_GPIOS { GPIO2, GPIO4, GPIO5 }
#endif

#endif  /* DOXYGEN */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H */
/** @} */
