/*
 * Copyright (C) 2019 University of Applied Sciences Emden / Leer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ph_oem
 * @{
 *
 * @file
 * @brief       pH OEM device driver
 *
 * @author      Igor Knippenberg <igor.knippenberg@gmail.com>
 * @}
 */

#include "xtimer.h"
#include "assert.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#include "ph_oem.h"
#include "ph_oem_params.h"
#include "ph_oem_regs.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define I2C (dev->params.i2c)
#define ADDR (dev->params.addr)
#define EN_PIN (dev->params.enable_pin)

static int ph_oem_init_test(const ph_oem_t *dev);

int ph_oem_init(ph_oem_t *dev, const ph_oem_params_t *params)
{
    assert(dev && params);

    dev->params = *params;

    return ph_oem_init_test(dev);
}

static int ph_oem_init_test(const ph_oem_t *dev)
{
    uint8_t reg_data;

    i2c_acquire(I2C);

    /* Register read test */
    if (i2c_read_regs(I2C, ADDR, PH_OEM_REG_DEVICE_TYPE,
                      &reg_data, 1, 0x0) < 0) {
        DEBUG("\n[ph_oem debug] init - error: unable to read reg %x\n",
              PH_OEM_REG_DEVICE_TYPE);

        i2c_release(I2C);
        return PH_OEM_NODEV;
    }

    /* Test if the device ID of the attached pH OEM sensor equals the
     * value of the PH_OEM_REG_DEVICE_TYPE register
     * */
    if (reg_data != PH_OEM_DEVICE_TYPE_ID) {
        DEBUG("\n[ph_oem debug] init - error: the attached device is not a pH OEM "
              "Sensor. Read Device Type ID is: %i\n", reg_data);
        i2c_release(I2C);
        return PH_OEM_NOT_PH;
    }
    i2c_release(I2C);

    return PH_OEM_OK;
}

static int ph_oem_unlock_address_reg(ph_oem_t *dev)
{
    uint8_t reg_value = 1;

    assert(dev);
    i2c_acquire(I2C);

    i2c_write_reg(I2C, ADDR, PH_OEM_REG_UNLOCK, 0x55, 0x0);
    i2c_write_reg(I2C, ADDR, PH_OEM_REG_UNLOCK, 0xAA, 0x0);
    /* if successfully unlocked the register will equal 0x00 */
    i2c_read_reg(I2C, ADDR, PH_OEM_REG_UNLOCK, &reg_value, 0x0);

    if (reg_value != 0x00) {
        DEBUG("\n[ph_oem debug] Failed at unlocking I2C address register. \n");
        i2c_release(I2C);
        return PH_OEM_WRITE_ERR;
    }
    i2c_release(I2C);

    return PH_OEM_OK;
}

int ph_oem_set_i2c_address(ph_oem_t *dev, uint8_t addr)
{
    if (ph_oem_unlock_address_reg(dev) != PH_OEM_OK) {
        return PH_OEM_WRITE_ERR;
    }

    assert(dev);
    i2c_acquire(I2C);

    if (i2c_write_reg(I2C, ADDR, PH_OEM_REG_ADDRESS, addr, 0x0) < 0) {
        DEBUG("\n[ph_oem debug] Setting I2C address to %x failed\n", addr);
        i2c_release(I2C);
        return PH_OEM_WRITE_ERR;
    }

    dev->params.addr = addr;
    i2c_release(I2C);

    return PH_OEM_OK;
}

int ph_oem_enable_interrupt(ph_oem_t *dev, ph_oem_interrupt_pin_cb_t cb,
                            void *arg, ph_oem_irq_option_t option, gpio_mode_t gpio_mode)
{
    if (dev->params.interrupt_pin == GPIO_UNDEF) {
        return PH_OEM_INTERRUPT_GPIO_UNDEF;
    }

    if (ph_oem_set_interrupt_pin(dev, option) < 0) {
        return PH_OEM_WRITE_ERR;
    }

    int gpio_flank = 0;

    dev->arg = arg;
    dev->cb = cb;

    switch (option) {
        case PH_OEM_IRQ_DISABLED:
            break;
        case PH_OEM_IRQ_FALLING:
            gpio_flank = GPIO_FALLING;
            //gpio_mode = GPIO_IN_PU;
            break;
        case PH_OEM_IRQ_RISING:
            gpio_flank = GPIO_RISING;
            //gpio_mode = GPIO_IN_PD;
            break;
        case PH_OEM_IRQ_BOTH:
            gpio_flank = GPIO_BOTH;
            //gpio_mode = GPIO_IN_PD;
            break;
    }

    if (option != PH_OEM_IRQ_DISABLED) {
        if (gpio_init_int(dev->params.interrupt_pin,
                          gpio_mode, gpio_flank, cb, arg) < 0) {

            DEBUG("\n[ph_oem debug] Initilizing enable gpio pin failed.\n");
            return PH_OEM_GPIO_INIT_ERR;
        }
    }
    return PH_OEM_OK;
}

int ph_oem_set_interrupt_pin(const ph_oem_t *dev, ph_oem_irq_option_t option)
{
    assert(dev);
    i2c_acquire(I2C);

    if (i2c_write_reg(I2C, ADDR, PH_OEM_REG_INTERRUPT, option, 0x0) < 0) {
        DEBUG("\n[ph_oem debug] Setting interrupt pin to option %d failed.\n", option);
        i2c_release(I2C);
        return PH_OEM_WRITE_ERR;
    }

    i2c_release(I2C);

    return PH_OEM_OK;
}

int ph_oem_set_led_state(const ph_oem_t *dev, ph_oem_led_state_t state)
{
    assert(dev);
    i2c_acquire(I2C);

    if (i2c_write_reg(I2C, ADDR, PH_OEM_REG_LED, state, 0x0) < 0) {
        DEBUG("\n[ph_oem debug] Setting LED state to %d failed.\n", state);
        i2c_release(I2C);
        return PH_OEM_WRITE_ERR;
    }
    i2c_release(I2C);

    return PH_OEM_OK;
}

int ph_oem_set_device_state(const ph_oem_t *dev, ph_oem_device_state_t state)
{
    assert(dev);
    i2c_acquire(I2C);

    if (i2c_write_reg(I2C, ADDR, PH_OEM_REG_HIBERNATE, state, 0x0) < 0) {
        DEBUG("\n[ph_oem debug] Setting device state to %d failed\n", state);
        i2c_release(I2C);
        return PH_OEM_WRITE_ERR;
    }
    i2c_release(I2C);

    return PH_OEM_OK;
}

/* polling PH_OEM_REG_NEW_READING register as long as it does not equal 0x01 */
static int ph_oem_new_reading_available(const ph_oem_t *dev)
{
    int8_t new_reading_available;

    assert(dev);
    i2c_acquire(I2C);
    do {
        if (i2c_read_reg(I2C, ADDR, PH_OEM_REG_NEW_READING,
                         &new_reading_available, 0x0) < 0) {
            DEBUG("\n[ph_oem debug] Failed at reading PH_OEM_REG_NEW_READING\n");
            i2c_release(I2C);
            return PH_OEM_READ_ERR;
        }
        xtimer_usleep(20 * US_PER_MS);
    } while (new_reading_available == 0);

    /* need to manually reset register back to 0x00 */
    if (i2c_write_reg(I2C, ADDR, PH_OEM_REG_NEW_READING, 0x00, 0x0) < 0) {
        DEBUG("\n[ph_oem debug] Resetting PH_OEM_REG_NEW_READING failed\n");
        i2c_release(I2C);
        return PH_OEM_WRITE_ERR;
    }
    i2c_release(I2C);

    return PH_OEM_OK;
}

int ph_oem_start_new_reading(const ph_oem_t *dev)
{
    if (ph_oem_set_device_state(dev, PH_OEM_TAKE_READINGS) < 0) {
        return PH_OEM_WRITE_ERR;
    }

    /* if interrupt pin is undefined, poll till new reading was taken and stop
     * device form taking further readings */
    if (dev->params.interrupt_pin == GPIO_UNDEF) {
        int result = ph_oem_new_reading_available(dev);
        if (result < 0) {
            return result;
        }

        if (ph_oem_set_device_state(dev, PH_OEM_STOP_READINGS) < 0) {
            return PH_OEM_WRITE_ERR;
        }
    }
    return PH_OEM_OK;
}

int ph_oem_clear_calibration(const ph_oem_t *dev)
{
    uint8_t reg_value;

    assert(dev);
    i2c_acquire(I2C);
    if (i2c_write_reg(I2C, ADDR, PH_OEM_REG_CALIBRATION_REQUEST, 0x01, 0) < 0) {
        DEBUG("\n[ph_oem debug] Clearing calibration failed \n");
        i2c_release(I2C);
        return PH_OEM_WRITE_ERR;
    }

    do {
        if (i2c_read_reg(I2C, ADDR, PH_OEM_REG_CALIBRATION_REQUEST, &reg_value,
                         0) < 0) {
            i2c_release(I2C);
            return PH_OEM_READ_ERR;
        }
    } while (reg_value != 0x00);

    i2c_release(I2C);

    return PH_OEM_OK;
}

static int ph_oem_set_calibration_value(const ph_oem_t *dev,
                                        uint16_t calibration_value)
{
    uint8_t reg_value[4];

    reg_value[0] = 0x00;
    reg_value[1] = 0x00;
    reg_value[2] = (uint8_t)(calibration_value >> 8);
    reg_value[3] = (uint8_t)(calibration_value & 0x00FF);

    i2c_acquire(I2C);

    if (i2c_write_regs(I2C, ADDR, PH_OEM_REG_CALIBRATION_BASE, &reg_value, 4, 0) < 0) {
        DEBUG("\n[ph_oem debug] Calibrating device failed \n");
        i2c_release(I2C);
        return PH_OEM_WRITE_ERR;
    }

    /* Calibration is critical, so check if written value is in fact correct */
    if (i2c_read_regs(I2C, ADDR, PH_OEM_REG_CALIBRATION_BASE, &reg_value, 4, 0) < 0) {
        DEBUG("\n[ph_oem debug] Calibrating device failed \n");
        i2c_release(I2C);
        return PH_OEM_READ_ERR;
    }

    uint16_t confirm_value = (int16_t)(reg_value[2] << 8)
                             | (int16_t)(reg_value[3]);

    if (confirm_value != calibration_value) {
        DEBUG("\n[ph_oem debug] Calibrating device to pH raw %d failed \n",
              calibration_value);
        i2c_release(I2C);
        return PH_OEM_WRITE_ERR;
    }

    i2c_release(I2C);

    return PH_OEM_OK;
}

int ph_oem_set_calibration(const ph_oem_t *dev, uint16_t calibration_value,
                           ph_oem_calibration_option_t option)
{
    assert(dev);

    if (ph_oem_set_calibration_value(dev, calibration_value) != PH_OEM_OK) {
        return PH_OEM_WRITE_ERR;
    }

    uint8_t reg_value;

    i2c_acquire(I2C);

    if (i2c_write_reg(I2C, ADDR, PH_OEM_REG_CALIBRATION_REQUEST,
                      option, 0) < 0) {
        DEBUG("\n[ph_oem debug] Sending calibration request failed\n");
        return PH_OEM_WRITE_ERR;
    }

    do {
        if (i2c_read_reg(I2C, ADDR, PH_OEM_REG_CALIBRATION_REQUEST, &reg_value,
                         0) < 0) {
            DEBUG("\n[ph_oem debug] Reading calibration request status failed\n");
            i2c_release(I2C);
            return PH_OEM_READ_ERR;
        }
    } while (reg_value != 0x00);

    i2c_release(I2C);

    return PH_OEM_OK;
}

int ph_oem_read_calibration_state(const ph_oem_t *dev, uint16_t *calibration_state)
{
    assert(dev);
    i2c_acquire(I2C);

    if (i2c_read_reg(I2C, ADDR, PH_OEM_REG_CALIBRATION_CONFIRM,
                     calibration_state, 0) < 0) {
        DEBUG("\n[ph_oem debug] Failed at reading calibration confirm register\n");
        i2c_release(I2C);
        return PH_OEM_READ_ERR;
    }
    i2c_release(I2C);
    return PH_OEM_OK;
}

int ph_oem_set_compensation(const ph_oem_t *dev,
                            uint16_t temperature_compensation)
{
    if (!(temperature_compensation >= 1 && temperature_compensation <= 20000)) {
        return PH_OEM_TEMP_OUT_OF_RANGE;
    }

    assert(dev);
    uint8_t reg_value[4];

    reg_value[0] = 0x00;
    reg_value[1] = 0x00;
    reg_value[2] = (uint8_t)(temperature_compensation >> 8);
    reg_value[3] = (uint8_t)(temperature_compensation & 0x00FF);

    i2c_acquire(I2C);

    if (i2c_write_regs(I2C, ADDR, PH_OEM_REG_TEMP_COMPENSATION_BASE,
                       &reg_value, 4, 0) < 0) {
        DEBUG("\n[ph_oem debug] Setting temperature compensation of device to "
              "%d failed\n", temperature_compensation);
        i2c_release(I2C);
        return PH_OEM_WRITE_ERR;
    }
    i2c_release(I2C);

    return PH_OEM_OK;
}

int ph_oem_read_compensation(const ph_oem_t *dev,
                             uint16_t *temperature_compensation)
{
    uint8_t reg_value[4];

    assert(dev);
    i2c_acquire(I2C);

    if (i2c_read_regs(I2C, ADDR, PH_OEM_REG_TEMP_CONFIRMATION_BASE,
                      &reg_value, 4, 0) < 0) {
        DEBUG("[ph_oem debug] Getting temperature compensation value failed\n");
        i2c_release(I2C);
        return PH_OEM_READ_ERR;
    }
    *temperature_compensation = (int16_t)(reg_value[2] << 8) | (int16_t)(reg_value[3]);

    i2c_release(I2C);

    return PH_OEM_OK;
}

int ph_oem_read_ph(const ph_oem_t *dev, uint16_t *ph_value)
{
    uint8_t reg_value[4];

    assert(dev);
    i2c_acquire(I2C);

    if (i2c_read_regs(I2C, ADDR, PH_OEM_REG_PH_READING_BASE,
                      &reg_value, 4, 0) < 0) {
        DEBUG("[ph_oem debug] Getting pH value failed\n");
        i2c_release(I2C);
        return PH_OEM_READ_ERR;
    }
    *ph_value = (int16_t)(reg_value[2] << 8) | (int16_t)(reg_value[3]);

    i2c_release(I2C);

    return PH_OEM_OK;
}
