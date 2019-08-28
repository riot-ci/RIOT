#include "opt3001.h"
#include "opt3001_regs.h"
#include "periph/i2c.h"
#include "xtimer.h"
#include "log.h"
#include "byteorder.h"
#include "math.h"

#include "board.h"

#define ENABLE_DEBUG  (0)
#include "debug.h"

int opt3001_init(opt3001_t *dev, const opt3001_params_t *params)
{
    /* check parameters */
    assert(dev && params);

    uint16_t reg;

    dev->params = *params;

    /* test devide id */
    i2c_acquire(DEV_I2C);
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_DEVICE_ID, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_init: Error reading device ID!\n");
        return -OPT3001_ERROR_BUS;
    }

    reg = htons(reg);
    if (reg != OPT3001_DID_VALUE) {
        return -OPT3001_ERROR_DEV;
    }

    /* set range number and mode of conversion*/
    reg = OPT3001_REGS_CONFIG_RN(OPT3001_CONFIG_RN_FSR);
    reg |= OPT3001_REGS_CONFIG_MOC(OPT3001_CONFIG_M_SHUTDOWN);
    reg |= OPT3001_REGS_CONFIG_CT(OPT3001_CONVERSION_TIME);

    /* configure for latched window-style comparison operation */
    reg |= OPT3001_REGS_CONFIG_L;
    reg &= ~OPT3001_REGS_CONFIG_POL;
    reg &= ~OPT3001_REGS_CONFIG_ME;
    reg &= ~OPT3001_REGS_CONFIG_FC_MASK;

    reg = htons(reg);

    if (i2c_write_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_init: Error setting device configuration\n");
        return -OPT3001_ERROR_BUS;
    }
    i2c_release(DEV_I2C);

    return OPT3001_OK;
}

int opt3001_reset(const opt3001_t *dev)
{
    uint16_t reg = OPT3001_CONFIG_RESET;
    reg |= OPT3001_REGS_CONFIG_CT(OPT3001_CONVERSION_TIME);
    reg = htons(reg);

    /* Acquire exclusive access to the bus. */
    i2c_acquire(DEV_I2C);
    if (i2c_write_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        return -OPT3001_ERROR_BUS;
    }
    i2c_release(DEV_I2C);
    return OPT3001_OK;
}

int opt3001_set_active(const opt3001_t *dev)
{
    uint16_t reg;

    i2c_acquire(DEV_I2C);

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_init: Error reading BUS!\n");
        return -OPT3001_ERROR_BUS;
    }

    reg = htons(reg);
    reg |= OPT3001_REGS_CONFIG_MOC(OPT3001_CONFIG_M_CONTINUOUS);

    reg = htons(reg);
    if (i2c_write_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_init: Error setting device configuration\n");
        return -OPT3001_ERROR_BUS;
    }

    i2c_release(DEV_I2C);
    return OPT3001_OK;
}

int opt3001_read(const opt3001_t *dev, uint16_t *crf, uint16_t *rawl)
{
    uint16_t reg;

    i2c_acquire(DEV_I2C);

    /* wait for the conversion to finish */
    if (OPT3001_CONVERSION_TIME) {
        xtimer_usleep(OPT3001_CONVERSION_TIME_LONG);
    } else{
        xtimer_usleep(OPT3001_CONVERSION_TIME_SHORT);
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_init: Error reading BUS!\n");
        return -OPT3001_ERROR_BUS;
    }

    *crf = htons(reg) & OPT3001_REGS_CONFIG_CRF;

    if (!(*crf)) {
        i2c_release(DEV_I2C);
        LOG_DEBUG("opt3001_read: conversion in progress!\n");
        return -OPT3001_ERROR;
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_RESULT, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_init: Error reading BUS!\n");
        return -OPT3001_ERROR_BUS;
    }

    *rawl = htons(reg);

    i2c_release(DEV_I2C);
    return OPT3001_OK;
}

void opt3001_convert(int16_t rawl, float *convl)
{
  uint16_t mantissa;
  uint8_t exponent;

  exponent = OPT3001_REGS_REG_EXPONENT(rawl);
  mantissa = OPT3001_REGS_REG_MANTISSA(rawl);

  *convl = 0.01 * pow(2, exponent) * mantissa;
}
