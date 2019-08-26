/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at24cxxx
 * @{
 *
 * @file
 * @brief       Device driver implementation for the AT24C128 / AT24C256 EEPROM units.
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ENABLE_DEBUG  (0)
#include "debug.h"
#include "assert.h"
#include "xtimer.h"
#include "byteorder.h"

#include "at24cxxx_params.h"
#include "at24cxxx_defines.h"
#include "at24cxxx.h"

#ifndef min
#define min(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef AT24CXXX_SET_BUF_SIZE
#define AT24CXXX_SET_BUF_SIZE    (AT24CXXX_PAGE_SIZE) /**< Adjust to configure buffer size  */
#endif

static int32_t _read(const at24cxxx_t *dev, uint16_t pos, void *data,
                     size_t len)
{
    int check;
    uint8_t polls = AT24CXXX_MAX_POLLS;

    while (-ENXIO == (check = i2c_read_regs(dev->params.i2c,
                                            dev->params.dev_addr,
                                            pos, data, len,
                                            I2C_REG16))) {
        if (--polls == 0) {
            break;
        }
        xtimer_usleep(AT24CXXX_POLL_DELAY_US);
    }
    DEBUG("[AT24CXXX] _read::i2c_read_regs(): %d; polls: %d\n", check, polls);
    return check < 0 ? check : (int32_t)len;
}

static int32_t _write(const at24cxxx_t *dev, uint16_t pos, const void *data,
                      size_t len)
{
    int check = 0;
    size_t wr = 0;
    size_t chunk_len;

    while (!check && (chunk_len = len - wr)) {
        const uint8_t *chunk_data = ((const uint8_t *)data) + wr;
        chunk_len =
            min(chunk_len,
                AT24CXXX_PAGE_SIZE - (pos & (AT24CXXX_PAGE_SIZE - 1)));
        uint8_t polls = AT24CXXX_MAX_POLLS;
        while (-ENXIO == (check = i2c_write_regs(dev->params.i2c,
                                                 dev->params.dev_addr,
                                                 pos, chunk_data,
                                                 chunk_len,
                                                 I2C_REG16))) {
            if (--polls == 0) {
                break;
            }
            xtimer_usleep(AT24CXXX_POLL_DELAY_US);
        }
        DEBUG("[AT24CXXX] _write::i2c_write_regs(): %d; polls: %d\n", check,
              polls);
        if (!check) {
            wr += chunk_len;
            pos += chunk_len;
        }
    }
    return wr > 0 ? (int32_t)wr : check;
}

static int32_t _set(const at24cxxx_t *dev, uint16_t pos, uint8_t val,
                    size_t len)
{
    int check = 0;
    size_t wr = 0;
    size_t chunk_len;
    uint8_t set_buffer[AT24CXXX_SET_BUF_SIZE];

    memset(set_buffer, val, sizeof(set_buffer));

    while (!check && (chunk_len = len - wr)) {
        chunk_len = min(chunk_len, AT24CXXX_SET_BUF_SIZE);
        chunk_len =
            min(chunk_len,
                AT24CXXX_PAGE_SIZE - (pos & (AT24CXXX_PAGE_SIZE - 1)));
        uint8_t polls = AT24CXXX_MAX_POLLS;
        while (-ENXIO == (check = i2c_write_regs(dev->params.i2c,
                                                 dev->params.dev_addr,
                                                 pos, set_buffer,
                                                 chunk_len,
                                                 I2C_REG16))) {
            if (--polls == 0) {
                break;
            }
            xtimer_usleep(AT24CXXX_POLL_DELAY_US);
        }
        DEBUG("[AT24CXXX] _set::i2c_write_regs(): %d; polls: %d\n", check,
              polls);
        if (!check) {
            wr += chunk_len;
            pos += chunk_len;
        }
    }
    return wr > 0 ? (int32_t)wr : check;
}

const at24cxxx_t *at24cxxx_get_default_dev(void)
{
    return (const at24cxxx_t *)&at24cxxx_params[0];
}

int at24cxxx_init(at24cxxx_t *dev, at24cxxx_params_t *params)
{
    if (!dev || !params) {
        return -EINVAL;
    }

    dev->params = *params;
    if (dev->params.pin_wp != GPIO_UNDEF) {
        gpio_init(dev->params.pin_wp, GPIO_OUT);
        at24cxxx_disable_write_protect(dev);
    }
    return AT24CXXX_OK;
}

int32_t at24cxxx_read_byte(const at24cxxx_t *dev, uint16_t pos)
{
    if (pos >= dev->params.eeprom_size) {
        return -ERANGE;
    }

    uint8_t val = 0;
    if (i2c_acquire(dev->params.i2c)) {
        return -AT24CXXX_I2C_ERROR;
    }
    int32_t r = _read(dev, pos, &val, 1);
    i2c_release(dev->params.i2c);
    if (r < 0) {
        return r;
    }
    return (int32_t)val;
}

int32_t at24cxxx_read(const at24cxxx_t *dev, uint16_t pos, void *data,
                      size_t len)
{
    if (pos + len > dev->params.eeprom_size) {
        return -ERANGE;
    }

    int32_t r = 0;
    if (len) {
        if (i2c_acquire(dev->params.i2c)) {
            return -AT24CXXX_I2C_ERROR;
        }
        r = _read(dev, pos, data, len);
        i2c_release(dev->params.i2c);
    }
    return r;
}

int32_t at24cxxx_write_byte(const at24cxxx_t *dev, uint16_t pos,  uint8_t data)
{
    if (pos >= dev->params.eeprom_size) {
        return -ERANGE;
    }

    if (i2c_acquire(dev->params.i2c)) {
        return -AT24CXXX_I2C_ERROR;
    }
    int32_t w = _write(dev, pos, &data, 1);
    i2c_release(dev->params.i2c);
    if (w < 0) {
        return w;
    }
    return AT24CXXX_OK;
}

int32_t at24cxxx_write(const at24cxxx_t *dev, uint16_t pos, const void *data,
                       size_t len)
{
    if (pos + len > dev->params.eeprom_size) {
        return -ERANGE;
    }

    int32_t w = 0;
    if (len) {
        if (i2c_acquire(dev->params.i2c)) {
            return -AT24CXXX_I2C_ERROR;
        }
        w = _write(dev, pos, data, len);
        i2c_release(dev->params.i2c);
    }
    return w;
}

int32_t at24cxxx_set(const at24cxxx_t *dev, uint16_t pos, uint8_t val,
                     size_t len)
{
    if (pos + len > dev->params.eeprom_size) {
        return -ERANGE;
    }

    int32_t w = 0;
    if (len) {
        if (i2c_acquire(dev->params.i2c)) {
            return -AT24CXXX_I2C_ERROR;
        }
        w = _set(dev, pos, val, len);
        i2c_release(dev->params.i2c);
    }
    return w;
}

int32_t at24cxxx_clear(const at24cxxx_t *dev, uint16_t pos, size_t len)
{
    return at24cxxx_set(dev, pos, AT24CXXX_CLEAR_BYTE, len);
}

int32_t at24cxxx_erase(const at24cxxx_t *dev)
{
    return at24cxxx_clear(dev, 0, dev->params.eeprom_size);
}

int at24cxxx_enable_write_protect(const at24cxxx_t *dev)
{
    if (dev->params.pin_wp == GPIO_UNDEF) {
        return -ENOTSUP;
    }
    gpio_set(dev->params.pin_wp);
    return AT24CXXX_OK;
}

int at24cxxx_disable_write_protect(const at24cxxx_t *dev)
{
    if (dev->params.pin_wp == GPIO_UNDEF) {
        return -ENOTSUP;
    }
    gpio_clear(dev->params.pin_wp);
    return AT24CXXX_OK;
}
