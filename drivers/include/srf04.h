/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_srf04 srf04 ultra sonic range finder
 * @ingroup     drivers_sensors
 * @brief       Device driver for the srf04 ultra sonic range finder
 * @{
 *
 * @file
 * @brief       Device driver for the srf04 ultra sonic range finder
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifndef SRF04_H
#define SRF04_H

#include <stdint.h>
#include <stdio.h>

#include "periph/gpio.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Status and error return codes
 */
enum {
    SRF04_OK      =  0,              /**< exit without error */
    SRF04_GPIO    = -1,              /**< error initializing gpio*/
    SRF04_INT     = -2,              /**< error initializing gpio interrupt*/
};

typedef enum {
    SRF04_MEASURING,
    SRF04_IDLE,
} srf04_state_t;

typedef struct {
    srf04_state_t state;
    gpio_t trigger;
    gpio_t echo;
    uint32_t distance;
    uint32_t time;
} srf04_params_t;

/**
 * @brief   GPIO pins for hc-sr04_t device
 */
typedef struct {
    srf04_params_t p;
} srf04_t;

/**
 * @brief   Initialize gpio and interrupt
 *
 * @param[out] dev      device descriptor of sensor to initialize
 * @param[in]  trigger  gpio pin of trigger
 * @param[in]  echo     gpio pin of echo
 *
 * @return              SRF04_OK on success
 * @return              SRF04_GPIO on gpio init failure
 */
int srf04_init(srf04_t *dev, const gpio_t trigger, const gpio_t echo);

/**
 * @brief   Triggers measurement
 *
 * @param[in]  dev      device descriptor of sensor
 *
 * @return              SRF04_OK
 */
int srf04_trigger(const srf04_t *dev);

/**
 * @brief   Returns time of flight in ms
 *
 * @note    should not be invoked within 50 ms after triggering
 *
 * @param[in]  dev      device descriptor of sensor
 *
 * @return              time of flight in ms
 */
int srf04_read(const srf04_t* dev);


#ifdef __cplusplus
}
#endif

#endif /* SRF04_H */
/** @} */
