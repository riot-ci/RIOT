/*
 * Copyright (C) 2020 Continental
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_lsm6dsl
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LSM6DSL 3D accelerometer/gyroscope.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Sylvain Fabre <sylvain.fabre-ext@continental-its.com>
 *
 * @}
 */

#include "xtimer.h"

#include "lsm6dsl.h"
#include "lsm6dsl_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BUS         (dev->params.i2c)
#define ADDR        (dev->params.addr)

int lsm6dsl_init(lsm6dsl_t *dev, const lsm6dsl_params_t *params)
{
    uint8_t tmp;
    int res;

    assert(dev && params);

    dev->params = *params;

    i2c_acquire(BUS);

    /* Reboot */
    i2c_write_reg(BUS, ADDR, LSM6DSL_REG_CTRL3_C, LSM6DSL_CTRL3_C_BOOT, 0);

    xtimer_usleep(LSM6DSL_BOOT_WAIT);

    if (i2c_read_reg(BUS, ADDR, LSM6DSL_REG_WHO_AM_I, &tmp, 0) < 0) {
        i2c_release(BUS);
        DEBUG("[ERROR] lsm6dsl_init: i2c_read_reg LSM6DSL_REG_WHO_AM_I!\n");
        return -LSM6DSL_ERROR_BUS;
    }

    if (tmp != LSM6DSL_WHO_AM_I) {
        DEBUG("[ERROR] lsm6dsl_init: WHO_AM_I\n");
        return -LSM6DSL_ERROR_DEV;
    }

    /* Set acc odr / full scale */
    tmp = (dev->params.acc_odr << LSM6DSL_CTRL_ODR_SHIFT) |
          (dev->params.acc_fs << LSM6DSL_CTRL_FS_SHIFT);
    res = i2c_write_reg(BUS, ADDR, LSM6DSL_REG_CTRL1_XL, tmp, 0);
    /* Set gyro odr / full scale */
    tmp = (dev->params.gyro_odr << LSM6DSL_CTRL_ODR_SHIFT) |
          (dev->params.gyro_fs << LSM6DSL_CTRL_FS_SHIFT);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_CTRL2_G, tmp, 0);
    /* Set continuous mode */
    uint8_t fifo_odr = MAX(dev->params.acc_odr, dev->params.gyro_odr);
    tmp = (fifo_odr << LSM6DSL_FIFO_CTRL5_FIFO_ODR_SHIFT) |
          LSM6DSL_FIFO_CTRL5_CONTINUOUS_MODE;
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_FIFO_CTRL5, tmp, 0);
    tmp = (dev->params.gyro_decimation << LSM6DSL_FIFO_CTRL3_GYRO_DEC_SHIFT) |
          dev->params.acc_decimation;
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_FIFO_CTRL3, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_init: config\n");
        return -LSM6DSL_ERROR_CNF;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_enable_latched_interrupt(const lsm6dsl_t *dev)
{
    uint8_t tmp;
    int res = 0;

    i2c_acquire(BUS);
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, &tmp, 0);
    tmp |= LSM6DSL_TAP_CFG_LIR_MASK;
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, tmp, 0);
    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_enable_latched_interrupt\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_disable_latched_interrupt(const lsm6dsl_t *dev)
{
    uint8_t tmp;
    int res = 0;
    i2c_acquire(BUS);
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, &tmp, 0);
    tmp &= (~LSM6DSL_TAP_CFG_LIR_MASK);
    res += i2c_write_reg(BUS, ADDR, LSM6DSL_REG_TAP_CFG, tmp, 0);
    i2c_release(BUS);
    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_disable_latched_interrupt\n");
        return -LSM6DSL_ERROR_BUS;
    }
    return LSM6DSL_OK;
}

int lsm6dsl_read_temp(const lsm6dsl_t *dev, int16_t *data)
{
    uint8_t tmp;
    uint16_t traw;
    /* read raw temperature */
    i2c_acquire(BUS);
    if (i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUT_TEMP_L, &tmp, 0) < 0) {
        i2c_release(BUS);
        return -LSM6DSL_ERROR_BUS;
    }
    traw = tmp;
    if (i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUT_TEMP_H, &tmp, 0) < 0) {
        i2c_release(BUS);
        return -LSM6DSL_ERROR_BUS;
    }
    traw |= (uint16_t)tmp << 8;
    i2c_release(BUS);
    /* convert temperature to degC x 100 */
    traw += LSM6DSL_TEMP_OFFSET;
    *data = (int16_t)(((int32_t)traw * 100) / 256);

    return LSM6DSL_OK;
}
