/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vcnl40x0
 * @{
 *
 * @file
 * @brief       Device driver implementation for VCNL40X0 Proximity and Ambient Light devices.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "vcnl40x0.h"
#include "vcnl40x0_internals.h"
#include "vcnl40x0_params.h"
#include "periph/i2c.h"
#include "xtimer.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/* Internal functions */
static void _set_command(const vcnl40x0_t *dev, uint8_t command);
static void _get_command(const vcnl40x0_t *dev, uint8_t *command);

/*---------------------------------------------------------------------------*
 *                          VCNL40X0 Core API                                *
 *---------------------------------------------------------------------------*/

int vcnl40x0_init(vcnl40x0_t *dev, const vcnl40x0_params_t *params)
{
    dev->params = *params;

    /* Initialize I2C interface */
    if (i2c_init_master(dev->params.i2c_dev, I2C_SPEED_NORMAL)) {
        DEBUG("[Error] I2C device not enabled\n");
        return -VCNL40X0_ERR_I2C;
    }

    /* Acquire exclusive access */
    i2c_acquire(dev->params.i2c_dev);

    /* Check sensor ID */
    uint8_t checkid;
    i2c_read_reg(dev->params.i2c_dev, dev->params.i2c_addr,
                 VCNL40X0_REG_PRODUCT_ID, &checkid);
    if ((checkid & VCNL40X0_PRODUCT_MASK_PRODUCT_ID) != VCNL40X0_PRODUCT_ID) {
        DEBUG("[Error] Wrong device ID: %d\n", checkid);
        i2c_release(dev->params.i2c_dev);
        return -VCNL40X0_ERR_NODEV;
    }

    if (dev->params.led_current > 20) {
        dev->params.led_current = 20;
    }

    i2c_write_reg(dev->params.i2c_dev, dev->params.i2c_addr,
                  VCNL40X0_REG_PROXIMITY_CURRENT, dev->params.led_current);

    _set_command(dev, VCNL40X0_COMMAND_ALL_DISABLE);

    i2c_write_reg(dev->params.i2c_dev, dev->params.i2c_addr,
                  VCNL40X0_REG_PROXIMITY_RATE, dev->params.proximity_rate);

    _set_command(dev,
                 VCNL40X0_COMMAND_PROX_ENABLE | VCNL40X0_COMMAND_AMBI_ENABLE);

    i2c_write_reg(dev->params.i2c_dev, dev->params.i2c_addr,
                  VCNL40X0_REG_PROXIMITY_RATE,
                  dev->params.ambient_avg |
                  VCNL40X0_AMBIENT_PARA_AUTO_OFFSET_ENABLE |
                  dev->params.ambient_rate);

    /* Release I2C device */
    i2c_release(dev->params.i2c_dev);

    return VCNL40X0_OK;
}

uint16_t vcnl40x0_read_proximity(const vcnl40x0_t *dev)
{
    i2c_acquire(dev->params.i2c_dev);

    _set_command(dev,
                 VCNL40X0_COMMAND_PROX_ENABLE | VCNL40X0_COMMAND_PROX_ON_DEMAND);

    while (1) {
        uint8_t result;
        _get_command(dev, &result);

        if (result & VCNL40X0_COMMAND_MASK_PROX_DATA_READY) {
            uint8_t prox_buf[2];
            i2c_read_regs(dev->params.i2c_dev, dev->params.i2c_addr,
                          VCNL40X0_REG_PROXIMITY_VALUE, &prox_buf, 2);
            uint16_t res = (uint16_t)((prox_buf[0] << 8) | prox_buf[1]);
            DEBUG("Proximity measured: %i\n", res);
            i2c_release(dev->params.i2c_dev);
            return res;
        }
        xtimer_usleep(1);
    }

    i2c_release(dev->params.i2c_dev);
}

uint16_t vcnl40x0_read_ambient_light(const vcnl40x0_t *dev)
{
    i2c_acquire(dev->params.i2c_dev);

    _set_command(dev,
                 VCNL40X0_COMMAND_AMBI_ENABLE | VCNL40X0_COMMAND_AMBI_ON_DEMAND);

    while (1) {
         uint8_t result;
        _get_command(dev, &result);
        if (result & VCNL40X0_COMMAND_MASK_AMBI_DATA_READY) {
            uint8_t ambient_buf[2];
            i2c_read_regs(dev->params.i2c_dev, dev->params.i2c_addr,
                          VCNL40X0_REG_AMBIENT_VALUE, &ambient_buf, 2);
            uint16_t res = (uint16_t)((ambient_buf[0] << 8) | ambient_buf[1]);
            DEBUG("Proximity measured: %i\n", res);
            i2c_release(dev->params.i2c_dev);
            return res;
        }
        xtimer_usleep(1);
    }

    i2c_release(dev->params.i2c_dev);
}

uint16_t vcnl40x0_read_illuminance(const vcnl40x0_t *dev)
{
    return vcnl40x0_read_ambient_light(dev) >> 2;
}

void _set_command(const vcnl40x0_t *dev, uint8_t command)
{
    i2c_write_reg(dev->params.i2c_dev, dev->params.i2c_addr,
                  VCNL40X0_REG_COMMAND, command);
}

void _get_command(const vcnl40x0_t *dev, uint8_t *command)
{
    i2c_read_reg(dev->params.i2c_dev, dev->params.i2c_addr,
                 VCNL40X0_REG_COMMAND, &command);
}
