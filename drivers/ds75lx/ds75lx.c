/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ds75lx
 * @{
 *
 * @file
 * @brief       Device driver implementation for the DS75LX temperature sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"

#include "ds75lx.h"
#include "ds75lx_internals.h"
#include "ds75lx_params.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define DEV_I2C      (dev->params.i2c)
#define DEV_ADDR     (dev->params.addr)

/* Mapping table between temperature resolution and max conversion time. */
static const uint8_t _max_conversion_times[4] = {
    [DS75LX_RESOLUTION_9]  = DS75LX_MAX_CONVERSION_25,
    [DS75LX_RESOLUTION_10] = DS75LX_MAX_CONVERSION_50,
    [DS75LX_RESOLUTION_11] = DS75LX_MAX_CONVERSION_100,
    [DS75LX_RESOLUTION_12] = DS75LX_MAX_CONVERSION_250,
};

static int _update_configuration_bits(const ds75lx_t *dev, uint8_t bit,
                                      uint8_t mask, bool set)
{
    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t config;
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, DS75LX_REG_CONFIGURATION, &config, 0) < 0) {
        DEBUG("[ds75lx] error reading configuration register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -DS75LX_ERR_I2C;
    }

    DEBUG("[ds75lx] initial configuration register value: 0x%02X\n", config);

    /* clear bits */
    config &= ~mask;

    /* set bits if required */
    if (set) {
        config |= bit;
    }

    DEBUG("[ds75lx] configuration register value: 0x%02X\n", config);

    if (i2c_write_reg(DEV_I2C, DEV_ADDR, DS75LX_REG_CONFIGURATION, config, 0) < 0) {
        DEBUG("[ds75lx] error writing configuration register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -DS75LX_ERR_I2C;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    return DS75LX_OK;
}

int ds75lx_init(ds75lx_t *dev, const ds75lx_params_t *params)
{
    dev->params = *params;

    /* Set resolution bits + force shutdown of sensor */
    return _update_configuration_bits(dev,
        (dev->params.resolution << DS75LX_CONF_R0_POS) | (1 << DS75LX_CONF_SD_POS),
        (DS75LX_CONF_R0_MASK | (1 << DS75LX_CONF_SD_POS)), true);
}

int ds75lx_read_temperature(const ds75lx_t *dev, int16_t *temperature)
{
    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t tmp[2];
    uint16_t temp;

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, DS75LX_REG_TEMPERATURE, tmp, 2, 0) < 0) {
        DEBUG("[ds75lx] error reading temperature register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -DS75LX_ERR_I2C;
    }

    temp = (tmp[0] << 8) | tmp[1];
    DEBUG("[ds75lx] temperature register content 0x%04X\n", temp);

    /* Manage sign bit */
    bool negative = false;
    if (temp & 0x8000) {
        negative = true;
        temp &= ~0x8000;
    }

    *temperature = (temp >> 5);
    if (negative) {
        *temperature = -*temperature;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    return DS75LX_OK;
}

int ds75lx_wakeup(const ds75lx_t *dev)
{
    /* disable shutdown bit in configuration register */
    int ret = _update_configuration_bits(dev, (1 << DS75LX_CONF_SD_POS),
                                         (1 << DS75LX_CONF_SD_POS), false);

    if (ret == DS75LX_OK) {
        /* Wait max conversion time (depends on resolution) */
        xtimer_usleep(_max_conversion_times[dev->params.resolution] * US_PER_MS);
    }

    return ret;
}

int ds75lx_shutdown(const ds75lx_t *dev)
{
    /* enable shutdown bit in configuration register */
    return _update_configuration_bits(dev, (1 << DS75LX_CONF_SD_POS),
                                      (1 << DS75LX_CONF_SD_POS), true);
}
