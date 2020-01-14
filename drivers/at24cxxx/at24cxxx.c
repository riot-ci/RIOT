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

#include "at24cxxx_defines.h"
#include "at24cxxx.h"

#ifndef MIN
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

/**
 * @brief Calculate x mod y, if y is a power of 2
 */
#define MOD_POW2(x, y) ((x) & ((y) - 1))

/**
 * @brief I2C bus number shortcut
 */
#define I2C_BUS                     (dev->params.i2c)
/**
 * @brief Pin wp shortcut
 */
#define PIN_WP                      (dev->params.pin_wp)
/**
 * @brief EEPROM size shortcut
 */
#define EEPROM_SIZE                 (dev->params.eeprom_size)
/**
 * @brief I2C device address shortcut
 */
#define I2C_ADDR                    (dev->params.dev_addr)
/**
 * @brief Page size shortcut
 */
#define PAGE_SIZE                   (dev->params.page_size)
/**
 * @brief Word address length shortcut
 */
#define WORD_ADDR_LEN               (dev->params.word_addr_len)
/**
 * @brief Max polls shortcut
 */
#define MAX_POLLS                   (dev->params.max_polls)

#ifndef AT24CXXX_SET_BUF_SIZE
/**
 * @brief  Adjust to configure buffer size
 */
#define AT24CXXX_SET_BUF_SIZE       (PAGE_SIZE)
#endif

static
int _read(const at24cxxx_t *dev, uint32_t pos, void *data, size_t len)
{
    int check;
    uint8_t polls = MAX_POLLS;
    uint8_t dev_addr;
    uint8_t flags = 0;

    if (WORD_ADDR_LEN > 11) {
        /* 2 bytes word address length */
        /* append page address bits to device address (if any) */
        dev_addr  = (I2C_ADDR | ((pos & 0xFF0000) >> 16));
        pos &= 0xFFFF;
        flags = I2C_REG16;
    }
    else {
        /* append page address bits to device address (if any) */
        dev_addr = (I2C_ADDR | ((pos & 0xFF00) >> 8));
        pos &= 0xFF;
    }

    while (-ENXIO == (check = i2c_read_regs(I2C_BUS, dev_addr,
                                            pos, data, len,
                                            flags))) {
        if (--polls == 0) {
            break;
        }
        xtimer_usleep(AT24CXXX_POLL_DELAY_US);
    }
    DEBUG("[at24cxxx] i2c_read_regs(): %d; polls: %d\n", check, polls);
    return check;
}

static
int _write(const at24cxxx_t *dev, uint32_t pos, const void *data, size_t len)
{
    int check = 0;
    const uint8_t *chunk_data = ((const uint8_t *)data);

    while (len) {
        size_t chunk_len = MIN(len, PAGE_SIZE - MOD_POW2(pos, PAGE_SIZE));
        uint8_t polls = MAX_POLLS;
        uint8_t dev_addr;
        uint16_t _pos;
        uint8_t flags = 0;
        if (WORD_ADDR_LEN > 11) {
            /* 2 bytes word address length */
            /* append page address bits to device address (if any) */
            dev_addr  = (I2C_ADDR | ((pos & 0xFF0000) >> 16));
            _pos = (pos & 0xFFFF);
            flags = I2C_REG16;
        }
        else {
            /* append page address bits to device address (if any) */
            dev_addr = (I2C_ADDR | ((pos & 0xFF00) >> 8));
            _pos = pos & 0xFF;
        }
        while (-ENXIO == (check = i2c_write_regs(I2C_BUS, dev_addr,
                                                 _pos, chunk_data,
                                                 chunk_len,
                                                 flags))) {
            if (--polls == 0) {
                break;
            }
            xtimer_usleep(AT24CXXX_POLL_DELAY_US);
        }
        DEBUG("[at24cxxx] i2c_write_regs(): %d; polls: %d\n", check, polls);
        if (!check) {
            len -= chunk_len;
            pos += chunk_len;
            chunk_data += chunk_len;
        }
        else {
            break;
        }
    }
    return check;
}

static
int _set(const at24cxxx_t *dev, uint32_t pos, uint8_t val, size_t len)
{
    int check;
    uint8_t set_buffer[AT24CXXX_SET_BUF_SIZE];

    memset(set_buffer, val, sizeof(set_buffer));
    while (len) {
        size_t chunk_len = MIN(sizeof(set_buffer), len);
        check = _write(dev, pos, set_buffer, chunk_len);
        if (!check) {
            len -= chunk_len;
            pos += chunk_len;
        }
        else {
            break;
        }
    }
    return check;
}

int at24cxxx_init(at24cxxx_t *dev, const at24cxxx_params_t *params)
{
    if (!dev || !params) {
        return -EINVAL;
    }
    dev->params = *params;
    if (PIN_WP != GPIO_UNDEF) {
        gpio_init(PIN_WP, GPIO_OUT);
        at24cxxx_disable_write_protect(dev);
    }
    /* Check I2C bus once */
    if (i2c_acquire(I2C_BUS)) {
        return -AT24CXXX_I2C_ERROR;
    }
    i2c_release(I2C_BUS);
    return AT24CXXX_OK;
}

int at24cxxx_read_byte(const at24cxxx_t *dev, uint32_t pos, void *dest)
{
    if (pos >= EEPROM_SIZE) {
        return -ERANGE;
    }

    i2c_acquire(I2C_BUS);
    int check = _read(dev, pos, dest, 1);
    i2c_release(I2C_BUS);
    return check;
}

int at24cxxx_read(const at24cxxx_t *dev, uint32_t pos, void *data,
                      size_t len)
{
    if (pos + len > EEPROM_SIZE) {
        return -ERANGE;
    }

    int check = AT24CXXX_OK;
    if (len) {
        i2c_acquire(I2C_BUS);
        check = _read(dev, pos, data, len);
        i2c_release(I2C_BUS);
    }
    return check;
}

int at24cxxx_write_byte(const at24cxxx_t *dev, uint32_t pos, uint8_t data)
{
    if (pos >= EEPROM_SIZE) {
        return -ERANGE;
    }

    i2c_acquire(I2C_BUS);
    int  check = _write(dev, pos, &data, 1);
    i2c_release(I2C_BUS);
    return check;
}

int at24cxxx_write(const at24cxxx_t *dev, uint32_t pos, const void *data,
                       size_t len)
{
    if (pos + len > EEPROM_SIZE) {
        return -ERANGE;
    }

    int check = AT24CXXX_OK;
    if (len) {
        i2c_acquire(I2C_BUS);
        check = _write(dev, pos, data, len);
        i2c_release(I2C_BUS);
    }
    return check;
}

int at24cxxx_set(const at24cxxx_t *dev, uint32_t pos, uint8_t val,
                     size_t len)
{
    if (pos + len > EEPROM_SIZE) {
        return -ERANGE;
    }

    int check = AT24CXXX_OK;
    if (len) {
        i2c_acquire(I2C_BUS);
        check = _set(dev, pos, val, len);
        i2c_release(I2C_BUS);
    }
    return check;
}

int at24cxxx_clear(const at24cxxx_t *dev, uint32_t pos, size_t len)
{
    return at24cxxx_set(dev, pos, AT24CXXX_CLEAR_BYTE, len);
}

int at24cxxx_erase(const at24cxxx_t *dev)
{
    return at24cxxx_clear(dev, 0, EEPROM_SIZE);
}

int at24cxxx_enable_write_protect(const at24cxxx_t *dev)
{
    if (PIN_WP == GPIO_UNDEF) {
        return -ENOTSUP;
    }
    gpio_set(PIN_WP);
    return AT24CXXX_OK;
}

int at24cxxx_disable_write_protect(const at24cxxx_t *dev)
{
    if (PIN_WP == GPIO_UNDEF) {
        return -ENOTSUP;
    }
    gpio_clear(PIN_WP);
    return AT24CXXX_OK;
}
