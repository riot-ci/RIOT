/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ds3231 DS3231 Real Time Clock
 * @ingroup     drivers_sensors
 * @brief       Driver for the Maxim DS3231 extremely accurate RTC
 *
 * # About
 * This module implements a device driver for Maxim DS3231 RTC.
 *
 * # Implementation status
 * The current implementation does only support reading and setting of time
 * registers as well as reading the temperature register and configuring the
 * aging offset.
 *
 * Setting alarms and configuring the square wave output is not yet supported.
 *
 * @{
 * @file
 * @brief       Interface definition for the Maxim DS3231 RTC
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef DS3231_H
#define DS3231_H

#include <time.h>
#include <errno.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default address of DS3231 sensors
 */
#define DS3231_I2C_ADDR             0x68

/**
 * @brief   Configuration options
 */
enum {
    DS3231_OPT_BAT_ENABLE = 0x01,   /* enable backup battery on startup */
    DS2321_OPT_32KHZ_ENABLE = 0x02, /* enable 32KHz output */
};

typedef enum {
    DS2321_AL1_TRIG_PER_S = 0x0F, /* alarm once per second */
    DS2321_AL1_TRIG_S = 0x0E, /* alarm when second match */
    DS2321_AL1_TRIG_M_S = 0x0C, /* alarm when minutes and seconds match */
    DS2321_AL1_TRIG_H_M_S = 0x08, /* alarm when hours minutes seconds match */
    DS2321_AL1_TRIG_D_H_M_S = 0x00, /* alarm when D/H/M/S match */
} ds3231_alm_1_mode_t;

typedef enum {
    DS2321_AL2_TRIG_PER_M = 0x07,  /* alarm once per minute */
    DS2321_AL2_TRIG_M = 0x06,  /* alarm when minutes match */
    DS2321_AL2_TRIG_H_M = 0x04,  /* alarm when hours and minutes match */
    DS2321_AL2_TRIG_D_H_M_S = 0x00, /* alarm when D/H/M match */
} ds3231_alm_2_mode_t;

/**
 * @brief   Device descriptor for DS3231 devices
 */
typedef struct {
    i2c_t bus;          /**< I2C bus the device is connected to */
#if IS_USED(MODULE_DS3231_INT)
    gpio_t int_pin;     /**< alarm interrupt pin */
#endif /* MODULE_DS3231_INT */
} ds3231_t;

/**
 * @brief   Set of configuration parameters for DS3231 devices
 */
typedef struct {
    i2c_t bus;          /**< I2C bus the device is connected to */
    uint8_t opt;        /**< additional options */
#if IS_USED(MODULE_DS3231_INT)
    gpio_t int_pin;     /**< alarm interrupt pin */
#endif /* MODULE_DS3231_INT */
} ds3231_params_t;

#if IS_USED(MODULE_DS3231_INT)
typedef void (*ds3231_alarm_cb_t)(void *);

#endif /* MODULE_DS3231_INT */

/**
 * @brief   Initialize the given DS3231 device
 *
 * @param[out] dev      device descriptor of the targeted device
 * @param[in] params    device configuration
 *
 * @return  0 on success
 * @return  -EIO if no DS3231 device was found
 */
int ds3231_init(ds3231_t *dev, const ds3231_params_t *params);

#if IS_USED(MODULE_DS3231_INT)
int ds3231_init_int(ds3231_t *dev, ds3231_alarm_cb_t cb, void *arg);
#endif /* MODULE_DS3231_INT */

/**
 * @brief   Get date and time from the device
 *
 * @param[in] dev       DS3231 device descriptor
 * @param[out] time     current date and time from on device
 *
 * @return  0 on success
 * @return  -EIO on I2C communication error
 */
int ds3231_get_time(const ds3231_t *dev, struct tm *time);

/**
 * @brief   Set date and time of the device
 *
 * @param[in] dev       DS3231 device descriptor
 * @param[in] time      target date and time
 *
 * @return  0 on success
 * @return  -EIO on I2C communication error
 */
int ds3231_set_time(const ds3231_t *dev, const struct tm *time);

int ds3231_set_alarm_1(const ds3231_t *dev, struct tm *time,
                       ds3231_alm_1_mode_t trigger);

int ds3231_set_alarm_2(const ds3231_t *dev, struct tm *time,
                       ds3231_alm_2_mode_t trigger);

int ds3231_clear_alarm_1_flag(const ds3231_t *dev);

int ds3231_clear_alarm_2_flag(const ds3231_t *dev);

int ds3231_get_alarm_1_flag(const ds3231_t *dev, bool *flag);

int ds3231_get_alarm_2_flag(const ds3231_t *dev, bool *flag);

int ds3231_toggle_alarm_1(const ds3231_t *dev, bool enable);

int ds3231_toggle_alarm_2(const ds3231_t *dev, bool enable);

/**
 * @brief   Get the configured aging offset (see datasheet for more information)
 *
 * @param[in] dev       DS3231 device descriptor
 * @param[out] offset   aging offset
 *
 * @return  0 on success
 * @return  -EIO on I2C communication error
 */
int ds3231_get_aging_offset(const ds3231_t *dev, int8_t *offset);

/**
 * @brief   Set the aging offset (see datasheet for more information)
 *
 * @param[in] dev       DS3231 device descriptor
 * @param[in] offset    aging offset
 *
 * @return  0 on success
 * @return  -EIO on I2C communication error
 */
int ds3231_set_aging_offset(const ds3231_t *dev, int8_t offset);

/**
 * @brief   Get temperature from the device
 *
 * @param[in] dev       DS3231 device descriptor
 * @param[out] temp     current value of the temperature register [in centi °C]
 *
 * @return  0 on success
 * @return  -EIO on I2C communication error
 */
int ds3231_get_temp(const ds3231_t *dev, int16_t *temp);

/**
 * @brief   Enable the backup battery
 *
 * @param[in] dev       DS3231 device descriptor
 *
 * @return  0 on success
 * @return  -EIO on I2C communication error
 */
int ds3231_enable_bat(const ds3231_t *dev);

/**
 * @brief   Disable the backup battery
 *
 * @param[in] dev       DS3231 device descriptor
 *
 * @return  0 on success
 * @return  -EIO on I2C communication error
 */
int ds3231_disable_bat(const ds3231_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* DS3231_H */
/** @} */
