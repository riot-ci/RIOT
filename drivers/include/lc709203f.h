/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lc709203f LC709203F
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the LC709203F Battery Fuel Gauge
 * @{
 *
 * @file
 * @brief       Device driver interface for the LC709203F Battery Fuel Gauge
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef LC709203F_H
#define LC709203F_H

#ifdef __cplusplus 
extern "C" {
#endif

#include "periph/i2c.h"
#include "periph/gpio.h"

typedef enum {
    AUTO_MODE = 0x0,
    CHARGE_MODE = 0x1,
    DISCHARGE_MODE = 0xffff
} lc709203f_current_direction_t;

typedef enum {
    BAT_PROFILE_1 = 0,
    BAT_PROFILE_2 = 1
} lc709203f_battery_profile_t;

typedef enum {
    OPERATIONAL_MODE = 1,
    SLEEP_MODE = 2
} lc709203f_power_mode_t;

typedef enum {
    I2C_MODE = 0,
    THERMISTOR_MODE = 1
} lc709203f_temp_obtaining_mode_t;

typedef void (*gauge_cb_t)(void *arg);

typedef struct {
    gpio_t alarm_pin;
    i2c_t bus;
    uint8_t addr;
} lc709203f_params_t;

enum {
    LC709203F_OK    = 0,  /**< all went as expected */
    LC709203F_NOI2C = -1, /**< error using the I2C bus */
    LC709203F_CELL_TEMP_INVALID = -2 /**< Cell temp invalid */
};

typdef struct {
    i2c_t bus;                  /**< I2C bus to use */
    uint8_t addr;               /**< I2C Address of fuel gauge */
    uint8_t scale;              /**< scale for returned values */
    lc709203f_params_t params;  /**< param struct with static settings etc*/
    lc709203f_cb_t cb;          /**< callback method*/
    void *arg;                  /**< additional arguments for the callback method*/
} lc709203f_t

/*CRC I2C-Addresses for Read/write*/
#define LC709203F_I2C_READ              0x17
#define LC709203F_I2C_WRITE             0x16
/*Defines for Registers*/
#define LC709203F_REG_CELL_VOLTAGE      0x09
#define LC709203F_REG_RSOC              0x0d
#define LC709203F_REG_ITE               0x0f
#define LC709203F_REG_ID                0x11
#define LC709203F_REG_CELL_TEMP         0x08
#define LC709203F_REG_STATUS            0x16
#define LC709203F_REG_POWER_MODE        0x15
#define LC809203F_REG_ALARM_VOLTAGE     0x14
#define LC709203F_REG_ALARM_RSOC        0x13
#define LC709203F_REG_CHANGE_PARAMETER  0x12
#define LC709203F_REG_APT               0x0c
#define LC709203F_REG_APA               0x0b
#define LC709203F_REG_CURRENT_DIRECTION 0x0a
#define LC709203F_REG_THERMISTOR        0x06
#define LC709203F_REG_BEFORE_RSOC       0x04
#define LC709203F_REG_INITIAL_RSOC      0x07

/*Define CRC Polynomial*/
#define LC709203F_CRC_POLYNOMIAL        0x07
/**
 * @brief initializes the sensor and i2c
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] *params   pointer to lc709203f_param_t struct containing the interrupt pin and callback
 *
 * @return              LC709203F_OK on a working initialization
 * @return              LC709203F_NOI2C on undefined i2c device given in periph_conf
  */
int lc709203f_init(lc709203f_t *dev, lc709203f_params_t *params);

/**
 * @brief  reads battery voltage from Sensor
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              voltage in mV
 */
int16_t lc709203f_get_voltage(lc709203f_t *dev);

/**
 * @brief  reads battery state of charge from Sensor
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              rsoc in %
 */
int16_t lc709203f_get_rsoc(lc709203f_t *dev);

/**
 * @brief  reads battery indicator to empty from Sensor
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              ite in 0.1%
 */
int16_t lc709203f_get_ite(lc709203f_t *dev);

/**
 * @brief  reads Sensor id
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              id value
 */
int16_t lc709203f_get_id(lc709203f_t *dev);

/**
 * @brief  reads cell temperature from sensor
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              temp in 0.1°C steps
 */
int16_t lc709203f_get_cell_temp(lc709203f_t *dev);

/**
 * @brief  reads status bit register (Temperature obtaining mode)
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              THERMISTOR_MODE Thermistor mode
 * @return              I2C_MODE I2C Mode
 */
lc709203f_temp_obtaining_mode_t lc709203f_get_status_bit(lc709203f_t *dev);

/**
 * @brief  reads IC Power Mode register
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              OPERATIONAL_MODE Operational Mode
 * @return              SLEEP_MODE Sleep mode
 */
lc709203f_power_mode_t lc709203f_get_power_mode(lc709203f_t *dev);

/**
 * @brief  reads Alarm Low Cell Voltage Register
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              0 Disable
 * @return              Threshold value in 1mV steps
 */
int16_t lc709203f_get_alarm_low_voltage(lc709203f_t *dev);

/**
 * @brief  reads Alarm Low RSOC Register
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 
 * @return              0 Disable
 * @return              Threshold value in %
 */
int16_t lc709203f_get_alarm_low_rsoc(lc709203f_t *dev);

/**
 * @brief  reads Change of the Parameter register, Battery Profile selection
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              Battery profile number (1 or 0)
 */
int16_t lc709203f_get_change_of_parameter(lc709203f_t *dev);

/**
 * @brief  readsAPT (Adjustment Pack Thermistor)
 * value to adjust temperature measurement delay timing.
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              value to adjust temperature measurement delay timing.
 */
int16_t lc709203f_get_apt(lc709203f_t *dev);


/**
 * @brief  readsAPA (Adjustment Pack Application) in 1mOhm steps
 * value to adjust temperature measurement delay timing.
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              parasitic impedance
 */
int16_t lc709203f_get_apa(lc709203f_t *dev);

/**
 * @brief  reads Current Direction Register
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              AUTO_MODE Auto mode
 * @return              CHARGE_MODE charge mode
 * @return              DISCHARGE_MODE Discharge mode
 */
lc709203f_current_direction_t lc709203f_get_current_direction(lc709203f_t *dev);

/**
 * @brief  reads Thermistor-B register (Thermistor B constant to be measured)
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 *
 * @return              B Value in 1K steps
 */
int16_t lc709203f_get_thermistor_b(lc709203f_t *dev);

/**
 * @brief  Executes RSOC initialization with sampled maximum voltage
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 */
void lc709203f_set_rsoc_before(lc709203f_t *dev);

/**
 * @brief  Sets B constant of the Thermistor
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     Bconstant of thermistor
 */
void lc709203f_set_thermistor_b(lc709203f_t *dev, unsigned int value);

/**
 * @brief  Executes RSOC initialization
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 */
void lc709203f_set_rsoc_initial(lc709203f_t *dev);

/**
 * @brief  Sets cell temperature in i2c-mode
 *
 * @param[in] *dev		pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value		temp in 0.1K 0xAAC=0°C
 *
 * @return              LC709203F_CELL_TEMP_INVALID Invlid cell temp

 */
int8_t lc709203f_set_cell_temp(lc709203f_t *dev, unsigned int value);

/**
 * @brief  Sets current direction
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] direction current direction, AUTO_MODE, CHARGE_MODE, DISCHARGE_MODE
 */
void lc709203f_set_current_direction(lc709203f_t *dev, lc709203f_current_direction_t direction);

/**
 * @brief  Sets parasitic impedance (adjustment pack application)
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     impedance in 1mOhm steps
 */
void lc709203f_set_apa(lc709203f_t *dev, uint8_t value);

/**
 * @brief  Sets value to adjust temperature measurement delay timing (adjusmtent pack thermistor)
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     adjustment value
 */
void lc709203f_set_apt(lc709203f_t *dev, unsigned int value);

/**
 * @brief  Sets battery profile
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     profile (enum) BAT_PROFILE_1 BAT_PROFILE_2
 */
void lc709203f_set_change_of_parameter(lc709203f_t *dev, lc709203f_battery_profile_t value);

/**
 * @brief  Sets threshold for low rsoc alert
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     alert threshold (0-100%)
 */
void lc709203f_set_alarm_low_rsoc(lc709203f_t *dev, uint8_t value);

/**
 * @brief  Sets threshold for low cell voltage alert
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     alert threshold in 1mV steps
 */
void lc709203f_set_alarm_low_cell_voltage(lc709203f_t *dev, unsigned int value);

/**
 * @brief  Sets power mode
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     Power mode (enum) OPERATIONAL_MODE, SLEEP_MODE
 */
void lc709203f_set_power_mode(lc709203f_t *dev, lc709203f_power_mode_t value);


/**
 * @brief  Sets temperature obtaining method
 *
 * @param[in] *dev      pointer to lc709203f_t struct containing the i2c device and the address
 * @param[in] value     method to be used (enum) I2C_MODE, THERMISTOR_MODE
*/
void lc709203f_set_status_bit(lc709203f_t *dev, lc709203f_temp_obtaining_mode_t value);

#ifdef __cplusplus
}
#endif

#endif /*LC709203F_H*/
