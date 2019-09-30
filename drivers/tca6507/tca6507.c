/*
 * Copyright (C) 2019 Noel Le Moult <noel.lemoult@dfxlab.fr>
 * Copyright (C) 2019 William MARTIN <william.martin@power-lan.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tca6507
 * @brief       Driver for the TCA6507 I2C Led controller
 * @author      Noel Le Moult <noel.lemoult@dfxlab.fr>
 * @author      William MARTIN <william.martin@power-lan.com>
 * @file
 */

#include <string.h>
#include "xtimer.h"

#include "tca6507_internals.h"
#include "tca6507.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define TCA6507_I2C     (dev->params.i2c_dev)
#define TCA6507_ADDR    (dev->params.address)

int tca6507_init(tca6507_t *dev, const tca6507_params_t *params)
{
    /* initialize the device descriptor */
    dev->params = *params;

#ifdef TCA6507_FORCE_PROBE_ON_INIT
    i2c_acquire(TCA6507_I2C);

    /* Dummy read to check the I2C bus */
    uint8_t regs[1];
    if (i2c_read_regs(TCA6507_I2C, TCA6507_ADDR, TCA6507_SELECT0, &regs,
                      sizeof(regs), 0) != 0) {
        DEBUG("[ERROR] Cannot access TCA6507 device.\n");
        return TCA6507_ERR_I2C;
    }

    i2c_release(TCA6507_I2C);
#endif

    DEBUG("[DEBUG] Device initialized with success.\n");
    return TCA6507_OK;
}

int tca6507_set_led(const tca6507_t *dev, uint8_t led, uint8_t bank)
{
    int ret = TCA6507_ERR_I2C;
    uint8_t regs[3];

    /* setup the i2c bus */
    i2c_acquire(TCA6507_I2C);

    if (i2c_read_regs(TCA6507_I2C, TCA6507_ADDR,
                      TCA6507_SELECT0 | TCA6507_COMMAND_AUTOINC, &regs,
                      sizeof(regs), 0) != 0) {
        DEBUG("[ERROR] Cannot read TCA6507 from I2C.\n");
        goto finish;
    }

    if (bank == 0) {
        regs[0] = regs[0] & ~(1UL << led);
        regs[1] = regs[1] | 1UL << led;
        regs[2] = regs[2] & ~(1UL << led);
    }
    else {
        regs[0] = regs[0] | 1UL << led;
        regs[1] = regs[1] | 1UL << led;
        regs[2] = regs[2] & ~(1UL << led);
    }

    if (i2c_write_regs(TCA6507_I2C, TCA6507_ADDR,
                       TCA6507_SELECT0 | TCA6507_COMMAND_AUTOINC, &regs,
                       sizeof(regs), 0) != 0) {
        DEBUG("[ERROR] Cannot write command 'TCA6507_COMMAND_AUTOINC' to I2C.\n");
        goto finish;
    }
    ret = TCA6507_OK;

finish:
    i2c_release(TCA6507_I2C);
    return ret;
}

int tca6507_clear_led(const tca6507_t *dev, uint8_t led)
{
    int ret = TCA6507_ERR_I2C;
    uint8_t regs[3];

    /* setup the i2c bus */
    i2c_acquire(TCA6507_I2C);

    if (i2c_read_regs(TCA6507_I2C, TCA6507_ADDR,
                      TCA6507_SELECT0 | TCA6507_COMMAND_AUTOINC, &regs,
                      sizeof(regs), 0) != 0) {
        DEBUG("[ERROR] Cannot read TCA6507 from I2C.\n");
        goto finish;
    }

    regs[0] = regs[0] & ~(1UL << led);
    regs[1] = regs[1] & ~(1UL << led);
    regs[2] = regs[2] & ~(1UL << led);

    if (i2c_write_regs(TCA6507_I2C, TCA6507_ADDR,
                       TCA6507_SELECT0 | TCA6507_COMMAND_AUTOINC, &regs,
                       sizeof(regs), 0) != 0) {
        DEBUG("[ERROR] Cannot write command 'TCA6507_COMMAND_AUTOINC' to I2C.\n");
        goto finish;
    }
    ret = TCA6507_OK;

finish:
    i2c_release(TCA6507_I2C);
    return ret;
}

int tca6507_clear_all(const tca6507_t *dev)
{
    int ret = TCA6507_ERR_I2C;
    uint8_t regs[3];

    /* setup the i2c bus */
    i2c_acquire(TCA6507_I2C);

    regs[0] = 0;
    regs[1] = 0;
    regs[2] = 0;

    if (i2c_write_regs(TCA6507_I2C, TCA6507_ADDR,
                       TCA6507_SELECT0 | TCA6507_COMMAND_AUTOINC, &regs,
                       sizeof(regs), 0) != 0) {
        DEBUG("[ERROR] Cannot write command 'TCA6507_COMMAND_AUTOINC' to I2C.\n");
        goto finish;
    }
    ret = TCA6507_OK;

finish:
    i2c_release(TCA6507_I2C);
    return ret;
}


int tca6507_toggle_led(const tca6507_t *dev, uint8_t led, uint8_t bank)
{
    int ret = TCA6507_ERR_I2C;
    uint8_t regs[3];

    /* setup the i2c bus */
    i2c_acquire(TCA6507_I2C);

    if (i2c_read_regs(TCA6507_I2C, TCA6507_ADDR,
                      TCA6507_SELECT0 | TCA6507_COMMAND_AUTOINC, &regs,
                      sizeof(regs), 0) != 0) {
        DEBUG("[ERROR] Cannot read TCA6507 from I2C.\n");
        goto finish;
    }

    /* only work if led was is state 000 or 010/110 before. */
    if (bank == 0) {
        regs[0] = regs[0] ^ 0UL << led;
        regs[1] = regs[1] ^ 1UL << led;
        regs[2] = regs[2] ^ 0UL << led;
    }
    else {
        regs[0] = regs[0] ^ 1UL << led;
        regs[1] = regs[1] ^ 1UL << led;
        regs[2] = regs[2] ^ 0UL << led;
    }

    if (i2c_write_regs(TCA6507_I2C, TCA6507_ADDR,
                       TCA6507_SELECT0 | TCA6507_COMMAND_AUTOINC, &regs,
                       sizeof(regs), 0) != 0) {
        DEBUG("[ERROR] Cannot write command 'TCA6507_COMMAND_AUTOINC' to I2C.\n");
        goto finish;
    }
    ret = TCA6507_OK;

finish:
    i2c_release(TCA6507_I2C);
    return ret;
}

int tca6507_blink_led(const tca6507_t *dev, uint8_t led, uint8_t bank)
{
    int ret = TCA6507_ERR_I2C;
    uint8_t regs[3];
    uint8_t mask = 1UL << led;

    /* setup the i2c bus */
    i2c_acquire(TCA6507_I2C);

    if (i2c_read_regs(TCA6507_I2C, TCA6507_ADDR,
                      TCA6507_SELECT0 | TCA6507_COMMAND_AUTOINC, &regs,
                      sizeof(regs), 0) != 0) {
        DEBUG("[ERROR] Cannot read TCA6507 from I2C.\n");
        goto finish;
    }

    if (bank == 0) {
        regs[0] = regs[0] & ~mask;
        regs[1] = regs[1] | mask;
        regs[2] = regs[2] | mask;
    }
    else {
        regs[0] = regs[0] | mask;
        regs[1] = regs[1] | mask;
        regs[2] = regs[2] | mask;
    }

    if (i2c_write_regs(TCA6507_I2C, TCA6507_ADDR,
                       TCA6507_SELECT0 | TCA6507_COMMAND_AUTOINC, &regs,
                       sizeof(regs), 0) != 0) {
        DEBUG("[ERROR] Cannot write command 'TCA6507_MAX_INTENSITY' to I2C.\n");
        goto finish;
    }
    ret = TCA6507_OK;

finish:
    i2c_release(TCA6507_I2C);
    return ret;
}

static int tca6507_update_4bits(const tca6507_t *dev, uint8_t function,
                                uint8_t payload, uint8_t bank)
{
    int ret = TCA6507_ERR_I2C;
    uint8_t value;

    /* setup the i2c bus */
    i2c_acquire(TCA6507_I2C);

    if (i2c_write_regs(TCA6507_I2C, TCA6507_ADDR, function, &value, 1,
                       0) != 0) {
        DEBUG("[ERROR] Cannot read TCA6507 from I2C.\n");
        goto finish;
    }

    if (bank == 0) {
        value = (value & 0xF0) | (payload & 0x0F);
    }
    else {
        value = (value & 0x0F) | ((payload & 0x0F) << 4);
    }

    if (i2c_write_regs(TCA6507_I2C, TCA6507_ADDR, function, &value, 1,
                       0) != 0) {
        DEBUG("[ERROR] Cannot write command 'TCA6507_MAX_INTENSITY' to I2C.\n");
        goto finish;
    }

    ret = TCA6507_OK;

finish:
    i2c_release(TCA6507_I2C);
    return ret;
}

int tca6507_intensity(const tca6507_t *dev, uint8_t brightness, uint8_t bank)
{
    if (brightness > TCA6507_BRIGHTNESS_COUNT) {
        DEBUG(
            "[WARN] Brightness used a unsupported value, falling back to 100%%\n");
        brightness = TCA6507_BRIGHTNESS_100_PCENT;
    }

    return tca6507_update_4bits(dev, TCA6507_MAX_INTENSITY, brightness, bank);
}


int tca6507_fade_on_time(const tca6507_t *dev, uint8_t time, uint8_t bank)
{
    if (time > TCA6507_TIME_COUNT) {
        DEBUG("[WARN] Time used a unsupported value, falling back to 256ms\n");
        time = TCA6507_TIME_256_MS;
    }

    return tca6507_update_4bits(dev, TCA6507_FADEON_TIME, time, bank);
}

int tca6507_fade_off_time(const tca6507_t *dev, uint8_t time, uint8_t bank)
{
    if (time > TCA6507_TIME_COUNT) {
        DEBUG("[WARN] Time used a unsupported value, falling back to 256ms\n");
        time = TCA6507_TIME_256_MS;
    }

    return tca6507_update_4bits(dev, TCA6507_FADEOFF_TIME, time, bank);
}

int tca6507_fully_on_time(const tca6507_t *dev, uint8_t time, uint8_t bank)
{
    if (time > TCA6507_TIME_COUNT) {
        DEBUG("[WARN] Time used a unsupported value, falling back to 256ms\n");
        time = TCA6507_TIME_256_MS;
    }

    return tca6507_update_4bits(dev, TCA6507_FULLYON_TIME, time, bank);
}

int tca6507_first_fully_off_time(const tca6507_t *dev, uint8_t time,
                                 uint8_t bank)
{
    if (time > TCA6507_TIME_COUNT) {
        DEBUG("[WARN] Time used a unsupported value, falling back to 256ms\n");
        time = TCA6507_TIME_256_MS;
    }

    return tca6507_update_4bits(dev, TCA6507_FIRST_FULLYOFF_TIME, time, bank);
}

int tca6507_second_fully_off_time(const tca6507_t *dev, uint8_t time,
                                  uint8_t bank)
{
    if (time > TCA6507_TIME_COUNT) {
        DEBUG("[WARN] Time used a unsupported value, falling back to 256ms\n");
        time = TCA6507_TIME_256_MS;
    }

    return tca6507_update_4bits(dev, TCA6507_SEC_FULLYOFF_TIME, time, bank);
}
