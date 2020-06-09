/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pca9633
 * @brief       Device driver for the PCA9633 I2C PWM controller
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 * @{
 */

#include <stdio.h>

#include "pca9633.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Write data to a register.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] reg       Register address to write to
 * @param[in] data      Data to write
 *
 * @return  0 on success
 * @return  -PCA9633_ERROR_I2C if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
int _write_reg(pca9633_t* dev, uint8_t reg, uint8_t data);

/**
 * @brief Read data from a register.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] reg       Register address to read from
 * @param[out] data     Byte read from given registerAddress
 *
 * @return  0 on success
 * @return  -PCA9633_ERROR_I2C if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
int _read_reg(pca9633_t* dev, uint8_t reg, uint8_t* data);

int pca9633_init(pca9633_t *dev, pca9633_params_t *params)
{
    assert(dev);
    assert(params);

    dev->params = *params;

    i2c_init(dev->params.i2c_dev);

    int rc = _write_reg(dev, REG_MODE1, 0x0);
    _write_reg(dev, REG_MODE2, 0x0);

    if (rc != PCA9633_OK) {
        return rc;
    }

    pca9633_set_ldr_state_all(dev, LDR_STATE_IND_GRP);
    pca9633_set_group_control_mode(dev, GROUP_CONTROL_MODE_DIMMING);
    pca9633_set_rgb(dev, 255, 255, 255);

    return PCA9633_OK;
}

void pca9633_wakeup(pca9633_t* dev)
{
    uint8_t prev_reg;
    _read_reg(dev, REG_MODE1, &prev_reg);
    uint8_t newReg = prev_reg & ~(1 << BIT_SLEEP);

    _write_reg(dev, REG_MODE1, newReg);
}

void pca9633_sleep(pca9633_t* dev)
{
    uint8_t prev_reg;
    _read_reg(dev, REG_MODE1, &prev_reg);
    uint8_t newReg = prev_reg | (1 << BIT_SLEEP);

    _write_reg(dev, REG_MODE1, newReg);
}

void pca9633_turn_on(pca9633_t* dev)
{
    _write_reg(dev, REG_LEDOUT, dev->stored_reg_ledout);
}

void pca9633_turn_off(pca9633_t* dev)
{
    _read_reg(dev, REG_LEDOUT, &dev->stored_reg_ledout);
    _write_reg(dev, REG_LEDOUT, LDR_STATE_OFF);
}

void pca9633_set_pwm(pca9633_t* dev, uint8_t reg_pwm, uint8_t pwm)
{
    _write_reg(dev, reg_pwm, pwm);
}

void pca9633_set_grp_pwm(pca9633_t* dev, uint8_t pwm)
{
    _write_reg(dev, REG_GRPPWM, pwm);
}

void pca9633_set_blinking(pca9633_t* dev, uint8_t blink_period,
        float on_off_ratio)
{
    int16_t ratio = on_off_ratio * 256;

    if (ratio < 0) {
        ratio = 0;
    }
    else if (ratio > 255) {
        ratio = 255;
    }

    _write_reg(dev, REG_GRPFREQ, blink_period);
    _write_reg(dev, REG_GRPPWM, (uint8_t) ratio);
}

void pca9633_set_rgb(pca9633_t* dev, uint8_t r, uint8_t g, uint8_t b)
{
    pca9633_set_pwm(dev, dev->params.reg_pwm_red, r);
    pca9633_set_pwm(dev, dev->params.reg_pwm_green, g);
    pca9633_set_pwm(dev, dev->params.reg_pwm_blue, b);
}

void pca9633_set_rgba(pca9633_t* dev, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    pca9633_set_rgb(dev, r, g, b);

    if (dev->params.has_amber_channel) {
        pca9633_set_pwm(dev, dev->params.reg_pwm_amber, a);
    }
}

void pca9633_set_ldr_state(pca9633_t* dev, uint8_t state, uint8_t ldr_bit)
{
    uint8_t prev_reg;
    _read_reg(dev, REG_LEDOUT, &prev_reg);
    uint8_t new_reg;

    // first clear both bits of ldr
    new_reg = prev_reg & ~(0b11 << ldr_bit);

    // second set new state to specified ldr
    new_reg |= (state << ldr_bit);

    _write_reg(dev, REG_LEDOUT, new_reg);
}

void pca9633_set_ldr_state_all(pca9633_t* dev, uint8_t state)
{
    uint8_t new_reg = ( state << BIT_LDR3
                       | state << BIT_LDR2
                       | state << BIT_LDR1
                       | state << BIT_LDR0 );

    _write_reg(dev, REG_LEDOUT, new_reg);
}

void pca9633_set_auto_increment(pca9633_t* dev, uint8_t option)
{
    bool enabled, bit0, bit1;

    switch (option) {

        case AI_ALL:
            enabled = true;
            bit0 = false;
            bit1 = false;
            break;

        case AI_IND:
            enabled = true;
            bit0 = false;
            bit1 = true;
            break;

        case AI_GBL:
            enabled = true;
            bit0 = true;
            bit1 = false;
            break;

        case AI_IND_GBL:
            enabled = true;
            bit0 = true;
            bit1 = true;
            break;

        case AI_DISABLED:
        default:
            enabled = false;
            bit0 = false;
            bit1 = false;
            break;
    }

    uint8_t new_reg = ( (enabled << BIT_AI2)
                       | (bit1 << BIT_AI1)
                       | (bit0 << BIT_AI0) );

    _write_reg(dev, REG_MODE1, new_reg);
}

void pca9633_set_group_control_mode(pca9633_t* dev, uint8_t mode)
{
    uint8_t prev_reg;
    _read_reg(dev, REG_MODE2, &prev_reg);

    switch (mode) {

        case GROUP_CONTROL_MODE_BLINKING:
            _write_reg(dev, REG_MODE2, prev_reg | (1 << BIT_DMBLNK));
            break;

        case GROUP_CONTROL_MODE_DIMMING:
        default:
            _write_reg(dev, REG_MODE2, prev_reg & ~(1 << BIT_DMBLNK));
            break;
    }
}

int _write_reg(pca9633_t* dev, uint8_t reg, uint8_t data)
{
    i2c_t i2c_dev = dev->params.i2c_dev;

    if (i2c_acquire(i2c_dev) != 0) {
        return -PCA9633_ERROR_I2C;
    }
    int rc = i2c_write_reg(i2c_dev, dev->params.i2c_addr, reg, data, 0);
    i2c_release(i2c_dev);

    return rc;
}

int _read_reg(pca9633_t* dev, uint8_t reg, uint8_t* data)
{
    i2c_t i2c_dev = dev->params.i2c_dev;

    if (i2c_acquire(i2c_dev) != 0) {
        return -PCA9633_ERROR_I2C;
    }
    int rc = i2c_read_reg(i2c_dev, dev->params.i2c_addr, reg, data, 0);
    i2c_release(i2c_dev);

    return rc;
}
