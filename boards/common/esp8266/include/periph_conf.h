/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp8266 ESP8266 Common
 * @{
 *
 * @file
 * @brief       Configurations of MCU periphery that are common for all ESP8266 boards
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "c_types.h"

/* include board.h to make board specific configuration visible in any case */
#include "board.h"

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
#define LED0_ON         led_on_off (LED0_PIN, 1)
#define LED0_OFF        led_on_off (LED0_PIN, 0)
#define LED0_TOGGLE     led_toggle (LED0_PIN)
#endif

#ifdef LED1_PIN
#define LED1_MASK       (BIT(LED1_PIN))
#define LED1_ON         led_on_off (LED1_PIN, 1)
#define LED1_OFF        led_on_off (LED1_PIN, 0)
#define LED1_TOGGLE     led_toggle (LED1_PIN)
#endif

#if defined(SW_TIMER_USED)

/* software timer */
#define TIMER_NUMOF        (1)
#define TIMER_CHANNELS     (10)

#define XTIMER_OVERHEAD    (0)
#define XTIMER_BACKOFF     (100)
#define XTIMER_ISR_BACKOFF (100)

#else /* SW_TIMER_USED */

/* hardware timer */
#define TIMER_NUMOF     (1)
#define TIMER_CHANNELS  (1)

#define XTIMER_OVERHEAD (0)

#endif /* SW_TIMER_USED */

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
