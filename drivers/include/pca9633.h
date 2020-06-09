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

// Register definitions (page 11, table 7)

/**
 * @brief Mode register 1
 */
#define PCA9633_REG_MODE1       0x00

/**
 * @brief Mode register 2
 */
#define PCA9633_REG_MODE2       0x01

/**
 * @brief Brightness control LED0
 */
#define PCA9633_REG_PWM0        0x02

/**
 * @brief Brightness control LED1
 */
#define PCA9633_REG_PWM1        0x03

/**
 * @brief Brightness control LED2
 */
#define PCA9633_REG_PWM2        0x04

/**
 * @brief Brightness control LED3
 */
#define PCA9633_REG_PWM3        0x05

/**
 * @brief Group duty cycle control
 */
#define PCA9633_REG_GRPPWM      0x06

/**
 * @brief Group frequency
 */
#define PCA9633_REG_GRPFREQ     0x07

/**
 * @brief LED output state
 */
#define PCA9633_REG_LEDOUT      0x08

/**
 * @brief I2C-bus subaddress 1
 */
#define PCA9633_REG_SUBADR1     0x09

/**
 * @brief I2C-bus subaddress 2
 */
#define PCA9633_REG_SUBADR2     0x0A

/**
 * @brief I2C-bus subaddress 3
 */
#define PCA9633_REG_SUBADR3     0x0B

/**
 * @brief LED All Call I2C-bus address
 */
#define PCA9633_REG_ALLCALLADR  0x0C



// Bits in REG_MODE1 (page 12, table 8)

/**
 * @brief Bit for register Auto-Increment
 *        0 = disabled
 *        1 = enabled
 */
#define PCA9633_BIT_AI2     7

/**
 * @brief Bit for Auto-Increment bit1
 */
#define PCA9633_BIT_AI1     6

/**
 * @brief Bit for Auto-Increment bit0
 */
#define PCA9633_BIT_AI0     5

/**
 * @brief 0 = Normal mode
 *        1 = Low power mode. Oscillator off
 */
#define PCA9633_BIT_SLEEP   4

/**
 * @brief 0 = PCA9633 does not respond to I2C-bus subaddress 1
 *        1 = PCA9633 responds to I2C-bus subaddress 1
 */
#define PCA9633_BIT_SUB1    3

/**
 * @brief 0 = PCA9633 does not respond to I2C-bus subaddress 2
 *        1 = PCA9633 responds to I2C-bus subaddress 2
 */
#define PCA9633_BIT_SUB2    2

/**
 * @brief 0 = PCA9633 does not respond to I2C-bus subaddress 3
 *        1 = PCA9633 responds to I2C-bus subaddress 3
 */
#define PCA9633_BIT_SUB3    1

/**
 * @brief 0 = PCA9633 does not respond to LED All Call I2C-bus address
 *        1 = PCA9633 responds to LED All Call I2C-bus address
 */
#define PCA9633_BIT_ALLCALL 0



// Bits in REG_MODE2 (page 12-13, table 9)

/**
 * @brief Bit for group control; 0=dimming, 1=blinking
 */
#define PCA9633_BIT_DMBLNK  5

/**
 * @brief 0 = Output logic state not inverted. Value to use when no external driver used
 *        1 = Output logic state inverted. Value to use when external driver used
 */
#define PCA9633_BIT_INVRT   4

/**
 * @brief 0 = Outputs change on STOP command
 *        1 = Outputs change on ACK
 */
#define PCA9633_BIT_OCH     3

/**
 * @brief 0 = The 4 LED outputs are configured with an open-drain structure
 *        1 = The 4 LED outputs are configured with a totem pole structure
 */
#define PCA9633_BIT_OUTDRV  2

/**
 * @brief See PCA9633_BIT_OUTNE0
 */
#define PCA9633_BIT_OUTNE1  1

/**
 * @brief 00 = When OE = 1 (output drivers not enabled), LEDn = 0.
 *        01* = When OE = 1 (output drivers not enabled):
 *           LEDn = 1 when OUTDRV = 1
 *           LEDn = high-impedance when OUTDRV = 0 (same as OUTNE[1:0] = 10)
 *        10 When OE = 1 (output drivers not enabled), LEDn = high-impedance.
 *        11 reserved
 */
#define PCA9633_BIT_OUTNE0  0



// Bits in REG_LEDOUT (page 14, table 13)

/**
 * @brief Lower of two bits for LDR3
 */
#define PCA9633_BIT_LDR3    6

/**
 * @brief Lower of two bits for LDR2
 */
#define PCA9633_BIT_LDR2    4

/**
 * @brief Lower of two bits for LDR1
 */
#define PCA9633_BIT_LDR1    2

/**
 * @brief Lower of two bits for LDR0
 */
#define PCA9633_BIT_LDR0    0



// LED driver output state, LEDOUT (page 14, below table 13)

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



// Auto-Increment options (page 10, table 6)

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



// Frequency of 24 Hz is used
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
