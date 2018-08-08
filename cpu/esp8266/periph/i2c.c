/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_esp8266
 * @ingroup drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation using ESP8266 SDK
 *
 * @note        This implementation only implements the 7-bit addressing mode.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

/*
   PLEASE NOTE:

   Some parts of the implementation bases on the bit-banging implementation as
   described in [wikipedia](https://en.wikipedia.org/wiki/I%C2%B2C) as well as
   its implementation in [esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos.git).
   These parts are under the copyright of their respective owners.
*/
#if 0
#define ENABLE_DEBUG (0)
#include "debug.h"

#include <stdbool.h>

#include "cpu.h"
#include "log.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

#include "common.h"
#include "sdk/ets.h"
#include "esp/gpio_regs.h"

#if defined(I2C_NUMOF) && I2C_NUMOF > 0

/* has to be declared as extern since it is not possible to include */
/* user_interface.h due to conflicts with gpio_init */
extern uint8_t system_get_cpu_freq(void);
extern bool system_update_cpu_freq(uint8_t freq);

/* max clock stretching counter */
#define I2C_CLOCK_STRETCH 200

typedef struct
{
    i2c_speed_t speed;

    bool started;

    gpio_t scl;
    gpio_t sda;

    uint32_t scl_bit;   /* gpio bit mask for faster access */
    uint32_t sda_bit;   /* gpio bit mask for faster access */

    uint32_t delay;

} _i2c_bus_t;

static _i2c_bus_t _i2c_bus[] =
{
  #if defined(I2C_SDA_0) && defined(I2C_SDA_0)
  {
    .sda = I2C_SDA_0,
    .scl = I2C_SCL_0
  },
  #endif
  #if defined(I2C_SDA_1) && defined(I2C_SDA_1)
  {
    .sda = I2C_SDA_1,
    .scl = I2C_SCL_1
  },
  #endif
  #if defined(I2C_SDA_2) && defined(I2C_SDA_2)
  {
    .sda = I2C_SDA_2,
    .scl = I2C_SCL_2
  },
  #endif
};

static const uint32_t _i2c_delays[][2] =
{
    /* values specify one half-period and are only valid for -O2 option */
    /* value = [period - 0.5us(160MHz) or 1.0us(80MHz)] * cycles per second / 2 */
    /* cycles per us = ca. 20 (80 MHz) / ca. 40 (160 MHz) */
    [I2C_SPEED_LOW]       = {1990, 990}, /*   10 kbps (period 100 us) */
    [I2C_SPEED_NORMAL]    = { 190,  90}, /*  100 kbps (period 10 us) */
    [I2C_SPEED_FAST]      = {  40,  17}, /*  400 kbps (period 2.5 us) */
    [I2C_SPEED_FAST_PLUS] = {  13,   0}, /*    1 Mbps (period 1 us) */
    [I2C_SPEED_HIGH]      = {   0,   0}  /*  3.4 Mbps (period 0.3 us) not working */
};

static mutex_t i2c_bus_lock[I2C_NUMOF] = { MUTEX_INIT };

/* forward declaration of internal functions */

static inline void _i2c_delay (_i2c_bus_t* bus);
static inline bool _i2c_read_scl (_i2c_bus_t* bus);
static inline bool _i2c_read_sda (_i2c_bus_t* bus);
static inline void _i2c_set_scl (_i2c_bus_t* bus);
static inline void _i2c_clear_scl (_i2c_bus_t* bus);
static inline void _i2c_set_sda (_i2c_bus_t* bus);
static inline void _i2c_clear_sda (_i2c_bus_t* bus);
static void _i2c_start_cond (_i2c_bus_t* bus);
static void _i2c_stop_cond (_i2c_bus_t* bus);
static void _i2c_write_bit (_i2c_bus_t* bus, bool bit);
static bool _i2c_read_bit (_i2c_bus_t* bus);
static bool _i2c_write_byte (_i2c_bus_t* bus, uint8_t byte);
static uint8_t _i2c_read_byte (_i2c_bus_t* bus, bool ack);

/* implementation of i2c interface */

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    if (I2C_NUMOF != sizeof(_i2c_bus)/sizeof(_i2c_bus_t)) {
        LOG_INFO("I2C_NUMOF does not match number of the I2C_SDA_x/I2C_SCL_x definitions\n");
        LOG_INFO("Please check your configuration in file board.h\n");
        assert(I2C_NUMOF < sizeof(_i2c_bus)/sizeof(_i2c_bus_t));

        return -1;
    }

    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)

    if (speed == I2C_SPEED_HIGH) {
        LOG_INFO("%s I2C_SPEED_HIGH is not supported\n");
        return -2;
    }

    _i2c_bus[dev].speed   = speed;
    _i2c_bus[dev].delay   =_i2c_delays[speed][ets_get_cpu_frequency() == 80 ? 1 : 0];
    _i2c_bus[dev].scl_bit = BIT(_i2c_bus[dev].scl); /* store bit mask for faster access */
    _i2c_bus[dev].sda_bit = BIT(_i2c_bus[dev].sda); /* store bit mask for faster access */
    _i2c_bus[dev].started = false; /* for handling of repeated start condition */

    DEBUG ("%s scl=%d sda=%d speed=%d\n", __func__,
           _i2c_bus[dev].scl, _i2c_bus[dev].sda, _i2c_bus[dev].speed);

    /* configure SDA and SCL pin as GPIO in open-drain mode with enabled pull-ups */
    gpio_init (_i2c_bus[dev].scl, GPIO_OD_PU);
    gpio_init (_i2c_bus[dev].sda, GPIO_OD_PU);

    /* set SDA and SCL to be floating and pulled-up to high */
    _i2c_set_sda (&_i2c_bus[dev]);
    _i2c_set_scl (&_i2c_bus[dev]);

    return 0;
}

int i2c_acquire(i2c_t dev)
{
    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)

    mutex_lock(&i2c_bus_lock[dev]);
    return 0;
}

int i2c_release(i2c_t dev)
{
    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)

    mutex_unlock(&i2c_bus_lock[dev]);
    return 0;
}

int /* IRAM */ i2c_read_byte(i2c_t dev, uint8_t address, void *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

int /* IRAM */ i2c_read_bytes(i2c_t dev, uint8_t address, void *data, int length)
{
    DEBUG ("%s dev=%lu addr=%02x data=%p len=%d\n", __func__, dev, address, data, length);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -1);
    CHECK_PARAM_RET (length > 0, 0);
    CHECK_PARAM_RET (data != NULL, 0);

    _i2c_bus_t* bus = &_i2c_bus[dev];
    int i = 0;

    /* send START condition */
    _i2c_start_cond (bus);

    /* send address byte with read flag */
    if (_i2c_write_byte (bus, (address << 1) | I2C_FLAG_READ)) {
        /* receive bytes if send address was successful */
        for ( ; i < length; i++) {
            ((uint8_t*)data)[i] = _i2c_read_byte (bus, i < length-1);
        }
    }
    /* send STOP condition */
    _i2c_stop_cond (bus);

    return i;
}

int /* IRAM */ i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, void *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int /* IRAM */ i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, void *data, int length)
{
    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)
    CHECK_PARAM_RET (length > 0, 0)
    CHECK_PARAM_RET (data != NULL, 0);

    _i2c_bus_t* bus = &_i2c_bus[dev];

    /* send START condition */
    _i2c_start_cond (bus);

    /* send address with read flag and if successful */
    /* send register address */
    if (!_i2c_write_byte (bus, (address << 1) | I2C_FLAG_WRITE) ||
        !_i2c_write_byte (bus, reg)) {
        /* if not successful send STOP condition and return 0 */
        _i2c_stop_cond (bus);
        return 0;
    }

    /* receive data with repeated START condition if send address and register was successful */
    return i2c_read_bytes(dev, address, data, length);
}

int /* IRAM */ i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int /* IRAM */ i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
    DEBUG ("%s dev=%lu addr=%02x data=%p len=%d\n", __func__, dev, address, data, length);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -1);
    CHECK_PARAM_RET (length > 0, 0);
    CHECK_PARAM_RET (data != NULL, 0);

    _i2c_bus_t* bus = &_i2c_bus[dev];
    int i = 0;

    /* send START condition */
    _i2c_start_cond (bus);

    /* send address with read flag */
    if (!_i2c_write_byte (bus, (address << 1) | I2C_FLAG_WRITE)) {
        length = 0;
    }

    /* send bytes if send address was successful */
    for ( ; i < length; i++) {
        if (!_i2c_write_byte (bus, ((uint8_t*)data)[i])) {
            break;
        }
    }

    /* send STOP condition */
    _i2c_stop_cond (bus);

    return i;
}

int /* IRAM */ i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, uint8_t data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int /* IRAM */ i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, const void *data, int length)
{
    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)
    CHECK_PARAM_RET (length > 0, 0)
    CHECK_PARAM_RET (data != NULL, 0);

    _i2c_bus_t* bus = &_i2c_bus[dev];
    int i = 0;

    /* send START condition */
    _i2c_start_cond (bus);

    /* send address with read flag and if successful */
    /* send register address */
    if (!_i2c_write_byte (bus, (address << 1) | I2C_FLAG_WRITE) ||
        !_i2c_write_byte (bus, reg)) {
        length = 0;
    }

    /* send bytes if send address and register was successful */
    for ( ; i < length; i++) {
        if (!_i2c_write_byte (bus, ((uint8_t*)data)[i])) {
            break;
        }
    }

    /* send STOP condition */
    _i2c_stop_cond (bus);

    return i;
}

void i2c_poweron(i2c_t dev)
{
    /* since I2C is realized in software there is no device to power on */
    /* just return */
}

void i2c_poweroff(i2c_t dev)
{
    /* since I2C is realized in software there is no device to power off */
    /* just return */
}

/* --- internal functions --- */

static inline void _i2c_delay (_i2c_bus_t* bus)
{
    /* produces a delay */
    /* ca. 20 cycles = 1 us (80 MHz) or ca. 40 cycles = 1 us (160 MHz) */

    uint32_t cycles = bus->delay;
    if (cycles) {
        __asm__ volatile ("1:     _addi.n  %0, %0, -1 \n"
                          "       bnez     %0, 1b     \n" : "=r" (cycles) : "0" (cycles));
    }
}

/*
 * Please note: SDA and SDL pins are used in GPIO_OD_PU mode
 *              (open-drain with pull-ups).
 *
 * Setting a pin which is in open-drain mode leaves the pin floating and
 * the signal is pulled up to high. The signal can then be actively driven
 * to low by a slave. A read operation returns the current signal at the pin.
 *
 * Clearing a pin which is in open-drain mode actively drives the signal to
 * low.
 */

static inline bool _i2c_read_scl(_i2c_bus_t* bus)
{
    /* read SCL status (pin is in open-drain mode and set) */
    return GPIO.IN & bus->scl_bit;
}

static inline bool _i2c_read_sda(_i2c_bus_t* bus)
{
    /* read SDA status (pin is in open-drain mode and set) */
    return GPIO.IN & bus->sda_bit;
}

static inline void _i2c_set_scl(_i2c_bus_t* bus)
{
    /* set SCL signal high (pin is in open-drain mode and pulled-up) */
    GPIO.OUT_SET = bus->scl_bit;
}

static inline void _i2c_clear_scl(_i2c_bus_t* bus)
{
    /* set SCL signal low (actively driven to low) */
    GPIO.OUT_CLEAR = bus->scl_bit;
}

static inline void _i2c_set_sda(_i2c_bus_t* bus)
{
    /* set SDA signal high (pin is in open-drain mode and pulled-up) */
    GPIO.OUT_SET = bus->sda_bit;
}

static inline void _i2c_clear_sda(_i2c_bus_t* bus)
{
    /* set SDA signal low (actively driven to low) */
    GPIO.OUT_CLEAR = bus->sda_bit;
}

static /* IRAM */ void _i2c_start_cond(_i2c_bus_t* bus)
{
    /*
     * send start condition
     * on entry: SDA and SCL are set to be floating and pulled-up to high
     * on exit : SDA and SCL are actively driven to low
     */

    if (bus->started) {
        /* prepare the repeated start condition */

        /* SDA = passive HIGH (floating and pulled-up) */
        _i2c_set_sda (bus);

        /* t_VD;DAT not neccessary */
        /* _i2c_delay (bus); */

        /* SCL = passive HIGH (floating and pulled-up) */
        _i2c_set_scl (bus);

        /* clock stretching wait as long as clock is driven to low by the slave */
        uint32_t stretch = I2C_CLOCK_STRETCH;
        while (!_i2c_read_scl (bus) && stretch--) {}

        /* wait t_SU;STA - set-up time for a repeated START condition */
        /* min. in us: 4.7 (SM), 0.6 (FM), 0.26 (FPM), 0.16 (HSM); no max. */
        _i2c_delay (bus);
    }

    /* if SDA is low, arbitration is lost and someone else is driving the bus */
    if (_i2c_read_sda (bus) == 0) {
        DEBUG("%s arbitration lost dev\n", __func__);
    }

    /* begin the START condition: SDA = active LOW */
    _i2c_clear_sda (bus);

    /* wait t_HD;STA - hold time (repeated) START condition, */
    /* max none */
    /* min 4.0 us (SM), 0.6 us (FM), 0.26 us (FPM), 0.16 us (HSM) */
    _i2c_delay (bus);

    /* complete the START condition: SCL = active LOW */
    _i2c_clear_scl (bus);

    /* needed for repeated start condition */
    bus->started = true;
}

static /* IRAM */ void _i2c_stop_cond(_i2c_bus_t* bus)
{
    /*
     * send stop condition
     * on entry: SCL is active low and SDA can be changed
     * on exit : SCL and SDA are set to be floating and pulled-up to high
     */

    /* begin the STOP condition: SDA = active LOW */
    _i2c_clear_sda (bus);

    /* wait t_LOW - LOW period of SCL clock */
    /* min. in us: 4.7 (SM), 1.3 (FM), 0.5 (FPM), 0.16 (HSM); no max. */
    _i2c_delay (bus);

    /* SCL = passive HIGH (floating and pulled up) while SDA = active LOW */
    _i2c_set_scl (bus);

    /* clock stretching wait as long as clock is driven to low by the slave */
    uint32_t stretch = I2C_CLOCK_STRETCH;
    while (!_i2c_read_scl (bus) && stretch--) {}

    /* wait t_SU;STO - hold time (repeated) START condition, */
    /* min. in us: 4.0 (SM), 0.6 (FM), 0.26 (FPM), 0.16 (HSM); no max. */
    _i2c_delay (bus);

    /* complete the STOP condition: SDA = passive HIGH (floating and pulled up) */
    _i2c_set_sda (bus);

    /* wait t_BUF - bus free time between a STOP and a START condition */
    /* min. in us: 4.7 (SM), 1.3 (FM), 0.5 (FPM), 0.16 (HSM); no max. */
    _i2c_delay (bus);

    /* if SDA is low, arbitration is lost and someone else is driving the bus */
    if (_i2c_read_sda (bus) == 0) {
        DEBUG("%s arbitration lost dev\n", __func__);
    }

    bus->started = false;
}

static /* IRAM */ void _i2c_write_bit (_i2c_bus_t* bus, bool bit)
{
    /*
     * send one bit
     * on entry: SCL is active low, SDA can be changed
     * on exit : SCL is active low, SDA can be changed
     */

    /* SDA = bit */
    if (bit) {
        _i2c_set_sda (bus);
    }
    else {
        _i2c_clear_sda (bus);
    }

    /* wait t_VD;DAT - data valid time (time until data are valid) */
    /* max. in us: 3.45 (SM), 0.9 (FM), 0.45 (FPM); no min */
    _i2c_delay (bus);

    /* SCL = passive HIGH (floating and pulled-up), SDA value is available */
    _i2c_set_scl (bus);

    /* wait t_HIGH - time for the slave to read SDA */
    /* min. in us: 4 (SM), 0.6 (FM), 0.26 (FPM), 0.09 (HSM); no max. */
    _i2c_delay (bus);

    /* clock stretching wait as long as clock is driven low by the slave */
    uint32_t stretch = I2C_CLOCK_STRETCH;
    while (!_i2c_read_scl (bus) && stretch--) {}

    /* if SCL is high, now data is valid */
    /* if SDA is high, check that nobody else is driving SDA low */
    if (bit && !_i2c_read_sda(bus)) {
        DEBUG("%s arbitration lost dev\n", __func__);
    }

    /* SCL = active LOW to allow next SDA change */
    _i2c_clear_scl(bus);
}

static /* IRAM */ bool _i2c_read_bit (_i2c_bus_t* bus)
{
    /* read one bit
     * on entry: SCL is active low, SDA can be changed
     * on exit : SCL is active low, SDA can be changed
     */

    bool bit;

    /* SDA = passive HIGH (floating and pulled-up) to let the slave drive data */
    _i2c_set_sda (bus);

    /* wait t_VD;DAT - data valid time (time until data are valid) */
    /* max. in us: 3.45 (SM), 0.9 (FM), 0.45 (FPM); no min */
    _i2c_delay (bus);

    /* SCL = passive HIGH (floating and pulled-up), SDA value is available */
    _i2c_set_scl (bus);

    /* clock stretching wait as long as clock is driven to low by the slave */
    uint32_t stretch = I2C_CLOCK_STRETCH;
    while (!_i2c_read_scl (bus) && stretch--) {}

    /* wait t_HIGH - time for the slave to read SDA */
    /* min. in us: 4 (SM), 0.6 (FM), 0.26 (FPM), 0.09 (HSM); no max. */
    _i2c_delay (bus);

    /* SCL is high, read out bit */
    bit = _i2c_read_sda (bus);

    /* SCL = active LOW to allow next SDA change */
    _i2c_clear_scl(bus);

    return bit;
}

static /* IRAM */ bool _i2c_write_byte (_i2c_bus_t* bus, uint8_t byte)
{
    /* send one byte and returns true in case of ACK from slave */

    uint8_t bit;

    /* send the byte */
    for (bit = 0; bit < 8; ++bit) {
        _i2c_write_bit(bus, (byte & 0x80) != 0);
        byte <<= 1;
    }

    /* read acknowledge bit from slave */
    return !_i2c_read_bit (bus);
}


static /* IRAM */ uint8_t _i2c_read_byte(_i2c_bus_t* bus, bool ack)
{
    uint8_t byte = 0;
    uint8_t bit;

    /* read the byte */
    for (bit = 0; bit < 8; ++bit) {
        byte = (byte << 1) | _i2c_read_bit (bus);
    }

    /* write acknowledgement flag */
    _i2c_write_bit(bus, !ack);

    return byte;
}

#endif
#endif
