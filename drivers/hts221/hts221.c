/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_hts221
 * @{
 *
 * @file
 * @brief       Driver for the ST HTS221 digital Humidity Sensor
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdbool.h>
#include <string.h>

#include "assert.h"
#include "hts221.h"
#include "periph/i2c.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define I2C_SPEED		I2C_SPEED_FAST
#define BUS				(dev->p.i2c)
#define ADDR			(dev->p.addr)

static int _set_power(const hts221_t *dev, const bool active);

int hts221_init(hts221_t *dev, const hts221_params_t *params)
{
	uint8_t reg;

	memcpy(&dev->p, params, sizeof(hts221_params_t));
	/* initialize the I2C bus */
	i2c_acquire(BUS);
	if (i2c_init_master(BUS, I2C_SPEED) < 0) {
		i2c_release(BUS);
		DEBUG("hts221_init: i2c_init_master failed!\n");
		return -HTS221_NOBUS;
	}

	/* try if we can interact with the device by reading its manufacturer ID */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_WHO_AM_I, &reg, 1) != 1) {
		i2c_release(BUS);
		DEBUG("hts221_init: i2c_read_regs HTS221_REGS_WHO_AM_I failed!\n");
		return -HTS221_NOBUS;
	}
	if (reg != HTS221_DEVICE_ID) {
		i2c_release(BUS);
		DEBUG("hts221_init: invalid HTS221_DEVICE_ID!\n");
		return -HTS221_NODEV;
	}
	i2c_release(BUS);
	/* reboot device before usage */
	if (hts221_reboot(dev) != HTS221_OK) {
		return -HTS221_NOBUS;
	}

	i2c_acquire(BUS);
	if (i2c_write_regs(BUS, ADDR, HTS221_REGS_AV_CONF, &dev->p.avgx, 1) != 1) {
		i2c_release(BUS);
		DEBUG("hts221_init: i2c_write_regs HTS221_REGS_AV_CONF failed!\n");
		return -HTS221_NOBUS;
	}
	reg = 0;
	if (i2c_write_regs(BUS, ADDR, HTS221_REGS_CTRL_REG1, &reg, 1) != 1) {
		i2c_release(BUS);
		DEBUG("hts221_init: i2c_write_reg HTS221_REGS_CTRL_REG1 failed!\n");
		return -HTS221_NOBUS;
	}
	i2c_release(BUS);

	DEBUG("hts221_init: DONE!\n");
	return HTS221_OK;
}

int hts221_one_shot(const hts221_t *dev)
{
	uint8_t reg = HTS221_REGS_CTRL_REG1_ODR_ONE_SHOT;
	/* first, disable any continuous measurement, enter one short mode */
	if (hts221_set_rate(dev, reg) != HTS221_OK) {
		return -HTS221_NOBUS;
	}

	i2c_acquire(BUS);
	/* second, read current settings */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_CTRL_REG2, &reg, 1) != 1) {
		i2c_release(BUS);
		DEBUG("hts221_init: i2c_read_regs HTS221_REGS_CTRL_REG1 failed!\n");
		return -HTS221_NOBUS;
	}
	/* third, enable one-shot */
	reg |= HTS221_REGS_CTRL_REG2_OS_EN;
	if (i2c_write_regs(BUS, ADDR, HTS221_REGS_CTRL_REG2, &reg, 1) != 1) {
		i2c_release(BUS);
		return -HTS221_NOBUS;
	}
	i2c_release(BUS);

	return HTS221_OK;
}

int hts221_set_rate(const hts221_t *dev, const uint8_t rate)
{
	uint8_t reg;

	i2c_acquire(BUS);
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_CTRL_REG1, &reg, 1) != 1) {
		i2c_release(BUS);
		DEBUG("hts221_set_rate: i2c_read_reg HTS221_REGS_CTRL_REG1 failed!\n");
		return -HTS221_NOBUS;
	}
	reg |= rate;
	DEBUG("hts221_set_rate: %u\n", reg);
	if (i2c_write_regs(BUS, ADDR, HTS221_REGS_CTRL_REG1, &reg, 1) != 1) {
		i2c_release(BUS);
		DEBUG("hts221_set_rate: i2c_write_reg HTS221_REGS_CTRL_REG1 failed!\n");
		return -HTS221_NOBUS;
	}
	i2c_release(BUS);

	return HTS221_OK;
}

int hts221_reboot(const hts221_t *dev)
{
	uint8_t reg;

	i2c_acquire(BUS);
	reg = HTS221_REGS_CTRL_REG2_BOOT;
	if (i2c_write_regs(BUS, ADDR, HTS221_REGS_CTRL_REG2, &reg, 1) != 1) {
		i2c_release(BUS);
		DEBUG("hts221_reboot: i2c_write_reg HTS221_REGS_CTRL_REG2 failed!\n");
		return -HTS221_NOBUS;
	}
	/* loop until HTS221_REGS_CTRL_REG2_BOOT == 0 */
	do {
		i2c_read_regs(BUS, ADDR, HTS221_REGS_CTRL_REG2, &reg, 1);
	} while (reg & HTS221_REGS_CTRL_REG2_BOOT);
	i2c_release(BUS);

	return HTS221_OK;
}

static int _set_power(const hts221_t *dev, const bool active)
{
	uint8_t reg;

	i2c_acquire(BUS);
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_CTRL_REG1, &reg, 1) != 1) {
		i2c_release(BUS);
		DEBUG("_set_power: i2c_read_reg HTS221_REGS_CTRL_REG1 failed!\n");
		return -HTS221_NOBUS;
	}
	if (active) {
		reg |= HTS221_REGS_CTRL_REG1_PD_ACTIVE;
	}
	else {
		reg &= ~HTS221_REGS_CTRL_REG1_PD_ACTIVE;
	}
	if (i2c_write_regs(BUS, ADDR, HTS221_REGS_CTRL_REG1, &reg, 1) != 1) {
		i2c_release(BUS);
		DEBUG("_set_power: i2c_write_reg HTS221_REGS_CTRL_REG1 failed!\n");
		return -HTS221_NOBUS;
	}
	i2c_release(BUS);

	return HTS221_OK;
}

int hts221_power_on(const hts221_t *dev)
{
	return _set_power(dev, true);
}

int hts221_power_off(const hts221_t *dev)
{
	return _set_power(dev, false);
}

int hts221_get_state(const hts221_t *dev)
{
	uint8_t reg;

	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_STATUS_REG, &reg, 1) != 1) {
		i2c_release(BUS);
		DEBUG("hts221_get_state: i2c_read_regs failed!\n");
		return -HTS221_NOBUS;
	}
	return (int)reg;
}

int hts221_read_humidity(const hts221_t *dev, uint16_t *val)
{
	uint8_t reg[2];
	int16_t h0_t0_out, h1_t0_out, h_t_out;
	int16_t h0_rh, h1_rh;
	uint32_t tmp;

	/* 1. read h0_rh and h1_rh coefficients */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_H0_RH_X2, &reg[0], 2) != 2) {
		i2c_release(BUS);
		DEBUG("hts221_read_humidity: i2c_read_regs failed!\n");
		return -HTS221_NOBUS;
	}
	h0_rh = reg[0] >> 1;
	h1_rh = reg[1] >> 1;
	DEBUG("hts221_read_humidity: h0_rh %"PRIi16", h1_rh %"PRIi16"\n", h0_rh, h1_rh);
	/* 2. read h0_t0_out */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_H0_T0_OUT_L, &reg[0], 2) != 2) {
		i2c_release(BUS);
		DEBUG("hts221_read_humidity: i2c_read_regs failed!\n");
		return -HTS221_NOBUS;
	}
	h0_t0_out = (((uint16_t)reg[1]) << 8) | (uint16_t)reg[0];
	/* 3. read h1_t0_out */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_H1_T0_OUT_L, &reg[0], 2) != 2) {
		i2c_release(BUS);
		DEBUG("hts221_read_humidity: i2c_read_regs failed!\n");
		return -HTS221_NOBUS;
	}
	h1_t0_out = (((uint16_t)reg[1]) << 8) | (uint16_t)reg[0];
	DEBUG("hts221_read_humidity: h0_t0_out %"PRIi16", h1_t0_out %"PRIi16"\n", h0_t0_out, h1_t0_out);
	/* 4. read humidity */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_HUMIDITY_OUT_L, &reg[0], 2) != 2) {
		i2c_release(BUS);
		DEBUG("hts221_read_humidity: i2c_read_regs failed!\n");
		return -HTS221_NOBUS;
	}
	h_t_out = (((uint16_t)reg[1]) << 8) | (uint16_t)reg[0];
	DEBUG("hts221_read_humidity, raw: %i\n", h_t_out);
	/* 5. compute RH [%] value by linear interpolation */
	tmp = ((uint32_t)(h_t_out - h0_t0_out)) * ((uint32_t)(h1_rh - h0_rh)*10);
	*val = (tmp / (h1_t0_out - h0_t0_out)) + (h0_rh * 10);
	if (*val > 1000) {
		*val = 1000;
	}
	DEBUG("hts221_read_humidity, val: %"PRIu16"\n", *val);
	return HTS221_OK;
}

int hts221_read_temperature(const hts221_t *dev, int16_t *val)
{
	uint8_t reg[2];
	int16_t t_out, t0_out, t1_out, t0_degc_x8_u16, t1_degc_x8_u16;
	int16_t t0_degc, t1_degc;
	uint8_t tmp;
	uint32_t tmp32;
	/* 1. read t0_degc and t1_degc coefficients */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_T0_DEGC_X8, &reg[0], 2) != 2) {
		i2c_release(BUS);
		DEBUG("hts221_read_temperature: i2c_read_regs failed!\n");
		return -HTS221_NOBUS;
	}
	/* 2. read t1_t0_msb */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_T1_T0_MSB, &tmp, 1) != 1) {
		i2c_release(BUS);
		DEBUG("hts221_read_temperature: i2c_read_regs failed!\n");
		return -HTS221_NOBUS;
	}
	/* 3. calc values */
	t0_degc_x8_u16 = (((uint16_t)(tmp & 0x03)) << 8) | ((uint16_t)reg[0]);
	t1_degc_x8_u16 = (((uint16_t)(tmp & 0x0C)) << 8) | ((uint16_t)reg[1]);
	t0_degc = t0_degc_x8_u16 >> 3;
	t1_degc = t1_degc_x8_u16 >> 3;
	DEBUG("hts221_read_temperature: t0_degc %"PRIi16", t1_degc %"PRIi16"\n", t0_degc, t1_degc);
	/* 4. read t0_out */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_T0_OUT_L, &reg[0], 2) != 2) {
		i2c_release(BUS);
		DEBUG("hts221_read_temperature: i2c_read_regs failed!\n");
		return -HTS221_NOBUS;
	}
	t0_out = (((uint16_t)reg[1]) << 8) | (uint16_t)reg[0];
	/* 5. read t1_out */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_T1_OUT_L, &reg[0], 2) != 2) {
		i2c_release(BUS);
		DEBUG("hts221_read_temperature: i2c_read_regs failed!\n");
		return -HTS221_NOBUS;
	}
	t1_out = (((uint16_t)reg[1]) << 8) | (uint16_t)reg[0];
	DEBUG("hts221_read_temperature: t0_out %"PRIi16", t1_out %"PRIi16"\n", t0_out, t1_out);
	/* 6. read t_out */
	if (i2c_read_regs(BUS, ADDR, HTS221_REGS_TEMP_OUT_L, &reg[0], 2) != 2) {
		i2c_release(BUS);
		DEBUG("hts221_read_temperature: i2c_read_regs failed!\n");
		return -HTS221_NOBUS;
	}
	t_out = (((uint16_t)reg[1]) << 8) | (uint16_t)reg[0];
	DEBUG("hts221_read_temperature, raw: %i\n", t_out);
	/* 7. calculate temperatue */
	tmp32 = ((uint32_t)(t_out - t0_out)) * ((uint32_t)(t1_degc - t0_degc) * 10);
	*val = tmp32 / (t1_out - t0_out) + (t0_degc * 10);
	DEBUG("hts221_read_temperature, val: %"PRIi16"\n", *val);
	return HTS221_OK;
}
