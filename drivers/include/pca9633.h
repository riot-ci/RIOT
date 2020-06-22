/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_pca9633 PCA9633 I2C PWM controller
 * @ingroup     drivers_actuators
 * @brief       Device driver for the NXP PCA9633
 *
 * @{
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 */

#ifndef PCA9633_H
#define PCA9633_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdbool.h"
#include "periph/i2c.h"

/* LED driver output state, LEDOUT (page 14, below table 13) */

/**
 * @brief LED driver x is off
 */
#define PCA9633_LDR_STATE_OFF       0x00

/**
 * @brief LED driver x is fully on (individual brightness and group
 *        dimming/ blinking not controlled)
 */
#define PCA9633_LDR_STATE_ON        0x01

/**
 * @brief LED driver x individual brightness can be controlled through its
 *        PWMx register
 */
#define PCA9633_LDR_STATE_IND       0x02

/**
 * @brief LED driver x individual brightness and group dimming/ blinking can be
 *        controlled through its PWMx register and the GRPPWM registers. If using
 *        PCA9633_LDR_STATE_IND_GRP the controller takes the minimum value of
 *        PWM* and GRPPWM register
 */
#define PCA9633_LDR_STATE_IND_GRP   0x03



/* Auto-Increment options (page 10, table 6) */

/**
 * @brief No Auto-Increment
 */
#define PCA9633_AI_DISABLED 0

/**
 * @brief Auto-Increment for all registers. D3, D2, D1, D0 roll over to ‘0000’
 *        after the last register (1100) is accessed.
 */
#define PCA9633_AI_ALL      1

/**
 * @brief Auto-Increment for individual brightness registers only. D3, D2, D1, D0
 *        roll over to ‘0010’ after the last register (0101) is accessed.
 */
#define PCA9633_AI_IND      2

/**
 * @brief Auto-Increment for global control registers only. D3, D2, D1, D0 roll
 *        over to ‘0110’ after the last register (0111) is accessed.
 */
#define PCA9633_AI_GBL      3

/**
 * @brief Auto-Increment for individual and global control registers only. D3,
 *        D2, D1, D0 roll over to ‘0010’ after the last register (0111) is accessed.
 */
#define PCA9633_AI_IND_GBL  4



/**
 * @brief Control mode for blinking
 */
#define PCA9633_GROUP_CONTROL_MODE_BLINKING 0

/**
 * @brief Control mode for dimming
 */
#define PCA9633_GROUP_CONTROL_MODE_DIMMING  1



/* Frequency of 24 Hz is used */
/**
 * @brief Blinking period with a duration of 125 ms
 *
 * ((1 / 24 Hz) * 3 cycles)
 */
#define PCA9633_BLINKING_PERIOD_125_MS  3

/**
 * @brief Blinking period with a duration of 250 ms
 *
 * ((1 / 24 Hz) * 6 cycles)
 */
#define PCA9633_BLINKING_PERIOD_250_MS  6

/**
 * @brief Blinking period with a duration of 500 ms
 *
 * ((1 / 24 Hz) * 12 cycles)
 */
#define PCA9633_BLINKING_PERIOD_500_MS  12

/**
 * @brief Blinking period with a duration of 1 s
 *
 * ((1 / 24 Hz) * 24 cycles)
 */
#define PCA9633_BLINKING_PERIOD_1_S     24

/**
 * @brief Blinking period with a maximum duration of ~10.73 s
 */
#define PCA9633_BLINKING_PERIOD_MAX     255

/**
 * @brief Ration between on/ off in blinking mode is balanced.
 */
#define PCA9633_BLINKING_RATIO_BALANCED 0.5

/**
 * @brief   PCA9633 device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C device */
    uint16_t i2c_addr;          /**< I2C address of device */

    uint8_t reg_pwm_red;        /**< Register for red color */
    uint8_t reg_pwm_green;      /**< Register for green color */
    uint8_t reg_pwm_blue;       /**< Register for blue color */
    uint8_t reg_pwm_amber;      /**< Register for amber color */

    bool has_amber_channel;      /**< Whether PCA9633 has fourth channel */
} pca9633_params_t;

/**
 * @brief   PCA9633 PWM device data structure type
 */
typedef struct {
    pca9633_params_t params;     /**< Device initialization parameters */
    uint8_t stored_reg_ledout;   /**< Stored register content of LEDOUT */
} pca9633_t;

/**
 * @brief   PCA9633 driver error codes
 */
typedef enum {
    PCA9633_OK              = 0,    /**< Success */
    PCA9633_ERROR_I2C       = 1,    /**< I2C communication error */
} pca9685_error_t;

/**
 * @brief Initialization.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] params    Parameters for device initialization
 *
 * @return  PCA9633_OK on success
 * @return  -PCA9633_ERROR_I2C if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
int pca9633_init(pca9633_t *dev, pca9633_params_t *params);

/**
 * @brief Turn on all LEDs. Restores settings saved at pca9633_turn_off().
 *
 * WARNING: If you call pca9633_turn_off() twice, without calling
 *          pca9633_turn_on() in between, then the restored state will be
 *          PCA9633_LDR_STATE_OFF!
 *
 * @param[in] dev       Device descriptor of the PCA9633
 */
void pca9633_turn_on(pca9633_t* dev);

/**
 * @brief Turn off all LEDs. Saves current settings for pca9633_turn_on().
 *        For power saving, see pca9633_sleep().
 *
 * WARNING: If you call pca9633_turn_off() twice, without calling
 *          pca9633_turn_on() in between, then the restored state will be
 *          PCA9633_LDR_STATE_OFF!
 *
 * @param[in] dev       Device descriptor of the PCA9633
 */
void pca9633_turn_off(pca9633_t* dev);

/**
 * @brief Switch to normal mode.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 */
void pca9633_wakeup(pca9633_t* dev);

/**
 * @brief Switch to low power mode.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 */
void pca9633_sleep(pca9633_t* dev);

/**
 * @brief Set individual PWM signal for a given channel.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] reg_pwm   Register address for PWM channel
 * @param[in] pwm       PWM value
 */
void pca9633_set_pwm(pca9633_t* dev, uint8_t reg_pwm, uint8_t pwm);

/**
 * @brief Set global PWM signal.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] pwm       PWM value
 */
void pca9633_set_grp_pwm(pca9633_t* dev, uint8_t pwm);

/**
 * @brief Set up values for blinking mode. Blinking mode needs to be activated
 *        manually by calling
 *        pca9633_set_group_control_mode(GROUP_CONTROL_MODE_BLINKING).
 *
 * @param[in] dev           Device descriptor of the PCA9633
 * @param[in] blink_period  Period for one blink (turning off and on)
 * @param[in] on_off_ratio  Value between 0.0 and 1.0, where e.g. a value of
 *                          0.25 means 1/4 of the time the LEDs are on and
 *                          3/4 of the time the LEDs are off
 */
void pca9633_set_blinking(pca9633_t* dev, uint8_t blink_period, float on_off_ratio);

/**
 * @brief Set PWM values for RGB.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] r         Value for red color channel
 * @param[in] g         Value for green color channel
 * @param[in] b         Value for blue color channel
 */
void pca9633_set_rgb(pca9633_t* dev, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Set PWM values for RGBA.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] r         Value for red color channel
 * @param[in] g         Value for green color channel
 * @param[in] b         Value for blue color channel
 * @param[in] w         Value for amber color channel
 */
void pca9633_set_rgba(pca9633_t* dev, uint8_t r, uint8_t g, uint8_t b, uint8_t w);

/**
 * @brief Set the LED driver output state for a given channel.
 *        There are four states:
 *          - PCA9633_LDR_STATE_OFF
 *          - PCA9633_LDR_STATE_ON
 *          - PCA9633_LDR_STATE_IND
 *          - PCA9633_LDR_STATE_IND_GRP
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] state     One of the four possible states
 * @param[in] ldr_bit   Lower bit of LDR* (see BIT_LDR*)
 */
void pca9633_set_ldr_state(pca9633_t* dev, uint8_t state, uint8_t ldr_bit);

/**
 * @brief Set the LED driver output state for all channels.
 *        There are four states:
 *          - PCA9633_LDR_STATE_OFF
 *          - PCA9633_LDR_STATE_ON
 *          - PCA9633_LDR_STATE_IND
 *          - PCA9633_LDR_STATE_IND_GRP
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] state     One of the four possible states
 */
void pca9633_set_ldr_state_all(pca9633_t* dev, uint8_t state);

/**
 * @brief Set an option for auto increment.
 *        There are five options:
 *          - PCA9633_AI_DISABLED
 *          - PCA9633_AI_ALL
 *          - PCA9633_AI_IND
 *          - PCA9633_AI_GBL
 *          - PCA9633_AI_IND_GBL
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] option    One of the possible five options
 */
void pca9633_set_auto_increment(pca9633_t* dev, uint8_t option);

/**
 * @brief Set the group control mode.
 *        There are two modes:
 *          - PCA9633_GROUP_CONTROL_MODE_BLINKING
 *          - PCA9633_GROUP_CONTROL_MODE_DIMMING
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] mode      One of the two possible modes
 */
void pca9633_set_group_control_mode(pca9633_t* dev, uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif /* PCA9633_H */
/** @} */
