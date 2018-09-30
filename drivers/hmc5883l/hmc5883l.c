/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hmc5883l
 * @brief       Device driver for the Honeywell HMC5883L 3-axis digital compass
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <string.h>
#include <stdlib.h>

#include "hmc5883l_regs.h"
#include "hmc5883l.h"

#include "log.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    if (!(cond)) { \
        DEBUG("[hmc5883l] %s: %s\n", \
              __func__, "parameter condition (" # cond ") not fulfilled"); \
        assert(cond); \
    }

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[hmc5883l] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.dev, d->params.addr, ## __VA_ARGS__);

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond);
#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[hmc5883l] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.dev, d->params.addr, ## __VA_ARGS__);

#define EXEC_RET(f, r) \
    if ((r = f) != HMC5883L_OK) { \
        DEBUG("[hmc5883l] %s: error code %d\n", __func__, res); \
        return res; \
    }

#define EXEC_RET_CODE(f, r, c) \
    if ((r = f) != HMC5883L_OK) { \
        DEBUG("[hmc5883l] %s: error code %d\n", __func__, res); \
        return c; \
    }

/** Forward declaration of functions for internal use */

static int _is_available(const hmc5883l_t *dev);

static uint8_t _get_reg_bit(uint8_t byte, uint8_t mask);
static void _set_reg_bit(uint8_t *byte, uint8_t mask, uint8_t bit);
static int _reg_read(const hmc5883l_t *dev, uint8_t reg, uint8_t *data, uint16_t len);
static int _reg_write(const hmc5883l_t *dev, uint8_t reg, uint8_t *data, uint16_t len);

int hmc5883l_init(hmc5883l_t *dev, const hmc5883l_params_t *params)
{
    int res = HMC5883L_OK;

    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);
    DEBUG_DEV("params=%p", dev, params);

    /* init sensor data structure */
    dev->params = *params;

    /* check availability of the sensor */
    EXEC_RET(_is_available(dev), res)

    uint8_t cfg_a = 0;
    uint8_t cfg_b = 0;
    uint8_t mode = 0;

    /* set configuration register A and B */
    _set_reg_bit(&cfg_a, HMC5883L_REG_CFG_A_MA, params->meas_avg);
    _set_reg_bit(&cfg_a, HMC5883L_REG_CFG_A_MS, params->meas_mode);
    _set_reg_bit(&cfg_a, HMC5883L_REG_CFG_A_DO, params->dor);
    _set_reg_bit(&cfg_b, HMC5883L_REG_CFG_B_GN, params->gain);

    EXEC_RET(_reg_write(dev, HMC5883L_REG_CFG_A, &cfg_a, 1), res);
    EXEC_RET(_reg_write(dev, HMC5883L_REG_CFG_B, &cfg_b, 1), res);

    /* set operation mode */
    _set_reg_bit(&mode, HMC5883L_REG_MODE_HS, 0);
    _set_reg_bit(&mode, HMC5883L_REG_MODE_MD, params->op_mode);

    EXEC_RET(_reg_write(dev, HMC5883L_REG_MODE, &mode, 1), res);

    /* wait 6 ms accoring to data sheet */
    xtimer_usleep(6 * US_PER_MS);

    return res;
}

int hmc5883l_data_ready(const hmc5883l_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    int res = HMC5883L_OK;

    uint8_t reg;

    EXEC_RET(_reg_read(dev, HMC5883L_REG_STATUS, &reg, 1), res);
    return _get_reg_bit(reg, HMC5883L_REG_STATUS_RDY) ? HMC5883L_OK
                                                      : HMC5883L_ERROR_NO_DATA;
}

/*
 * scale factors for conversion of raw sensor data to degree for possible
 * sensitivities according to mechanical characteristics in datasheet
 */
static const uint32_t HMC5883L_RES[] = {
     730,      /* uG/LSb for HMC5883L_GAIN_1370 with range +-0.88 Gs */
     920,      /* uG/LSb for HMC5883L_GAIN_1090 with range  +-1.3 Gs */
    1220,      /* uG/LSb for HMC5883L_GAIN_820  with range  +-1.9 Gs */
    1520,      /* uG/LSb for HMC5883L_GAIN_660  with range  +-2.5 Gs */
    2270,      /* uG/LSb for HMC5883L_GAIN_440  with range  +-4.0 Gs */
    2560,      /* uG/LSb for HMC5883L_GAIN_390  with range  +-4.7 Gs */
    3030,      /* uG/LSb for HMC5883L_GAIN_330  with range  +-5.6 Gs */
    4350,      /* uG/LSb for HMC5883L_GAIN_230  with range  +-8.1 Gs */
};

int hmc5883l_read(const hmc5883l_t *dev, hmc5883l_data_t *data)

{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("data=%p", dev, data);

    int res = HMC5883L_OK;

    hmc5883l_raw_data_t raw;

    EXEC_RET(hmc5883l_read_raw (dev, &raw), res);

    data->x = ((uint32_t)raw.x * HMC5883L_RES[dev->params.gain]);
    data->y = ((uint32_t)raw.y * HMC5883L_RES[dev->params.gain]);
    data->z = ((uint32_t)raw.z * HMC5883L_RES[dev->params.gain]);

    return res;
}

int hmc5883l_read_raw(const hmc5883l_t *dev, hmc5883l_raw_data_t *raw)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(raw != NULL);
    DEBUG_DEV("raw=%p", dev, raw);

    int res = HMC5883L_OK;

    uint8_t data[6];

    /* read raw data sample */
    EXEC_RET_CODE(_reg_read(dev, HMC5883L_REG_OUT_X_MSB, data, 6),
                  res, -HMC5883L_ERROR_RAW_DATA);

    /* data MSB @ lower address */
    raw->x = (data[0] << 8) | data[1];
    raw->y = (data[2] << 8) | data[3];
    raw->z = (data[4] << 8) | data[5];

    return res;
}

int hmc5883l_power_down (hmc5883l_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t mode = 0;

    /* set operation mode to Idle mode with only 5 uA current */
    _set_reg_bit(&mode, HMC5883L_REG_MODE_HS, 0);
    _set_reg_bit(&mode, HMC5883L_REG_MODE_MD, HMC5883L_OP_MODE_IDLE);

    return _reg_write(dev, HMC5883L_REG_MODE, &mode, 1);
}

int hmc5883l_power_up (hmc5883l_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t mode = 0;

    /* set operation mode to last operation mode */
    _set_reg_bit(&mode, HMC5883L_REG_MODE_HS, 0);
    _set_reg_bit(&mode, HMC5883L_REG_MODE_MD, dev->params.op_mode);

    return _reg_write(dev, HMC5883L_REG_MODE, &mode, 1);
}

/** Functions for internal use only */

/**
 * @brief   Check the chip ID to test whether sensor is available
 */
static int _is_available(const hmc5883l_t *dev)
{
    DEBUG_DEV("", dev);

    int res = HMC5883L_OK;

    uint8_t id_c[] = HMC5883L_ID;
    uint8_t id_r[HMC5883L_ID_LEN];

    /* read the chip id from HMC5883L_REG_ID_X */
    EXEC_RET(_reg_read(dev, HMC5883L_REG_ID_A, id_r, HMC5883L_ID_LEN), res);

    if (memcmp(id_r, id_c, HMC5883L_ID_LEN)) {
        DEBUG_DEV("sensor is not available, wrong id %02x%02x%02x, "
                  "should be %02x%02x%02x",
                  dev, id_r[0], id_r[1], id_r[2], id_c[0], id_c[1], id_c[2]);
        return -HMC5883L_ERROR_WRONG_ID;
    }

    return res;
}

static void _set_reg_bit(uint8_t *byte, uint8_t mask, uint8_t bit)
{
    ASSERT_PARAM(byte != NULL);

    uint8_t shift = 0;
    while (!((mask >> shift) & 0x01)) {
        shift++;
    }
    *byte = ((*byte & ~mask) | ((bit << shift) & mask));
}

static uint8_t _get_reg_bit(uint8_t byte, uint8_t mask)
{
    uint8_t shift = 0;

    while (!((mask >> shift) & 0x01)) {
        shift++;
    }
    return (byte & mask) >> shift;
}

static int _reg_read(const hmc5883l_t *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    ASSERT_PARAM(len != 0);

    DEBUG_DEV("read %d byte from sensor registers starting at addr 0x%02x",
              dev, len, reg);

    if (i2c_acquire(dev->params.dev)) {
        DEBUG_DEV("could not aquire I2C bus", dev);
        return -HMC5883L_ERROR_I2C;
    }

    int res = i2c_read_regs(dev->params.dev, dev->params.addr, reg, data, len, 0);
    i2c_release(dev->params.dev);

    if (res == HMC5883L_OK) {
        if (ENABLE_DEBUG) {
            printf("[hmc5883l] %s i2c dev=%d addr=%02x: read following bytes: ",
                   __func__, dev->params.dev, dev->params.addr);
            for (int i = 0; i < len; i++) {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
    }
    else {
        DEBUG_DEV("could not read %d bytes from sensor registers "
                  "starting at addr %02x, reason %d (%s)",
                  dev, len, reg, res, strerror(res * -1));
        return -HMC5883L_ERROR_I2C;
    }

    return res;
}

static int _reg_write(const hmc5883l_t *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    ASSERT_PARAM(len != 0);

    DEBUG_DEV("write %d bytes to sensor registers starting at addr 0x%02x",
              dev, len, reg);

    if (ENABLE_DEBUG) {
        printf("[hmc5883l] %s i2c dev=%d addr=%02x: write following bytes: ",
               __func__, dev->params.dev, dev->params.addr);
        for (int i = 0; i < len; i++) {
            printf("%02x ", data[i]);
        }
        printf("\n");
    }

    if (i2c_acquire(dev->params.dev)) {
        DEBUG_DEV("could not aquire I2C bus", dev);
        return -HMC5883L_ERROR_I2C;
    }

    int res;

    if (!data || !len) {
        res = i2c_write_byte(dev->params.dev, dev->params.addr, reg, 0);
    }
    else {
        res = i2c_write_regs(dev->params.dev, dev->params.addr, reg, data, len, 0);
    }
    i2c_release(dev->params.dev);

    if (res != HMC5883L_OK) {
        DEBUG_DEV("could not write %d bytes to sensor registers "
                  "starting at addr 0x%02x, reason %d (%s)",
                  dev, len, reg, res, strerror(res * -1));
        return -HMC5883L_ERROR_I2C;
    }

    return res;
}
