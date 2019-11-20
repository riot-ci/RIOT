/*
 * Copyright (C) 2019 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at24mac
 *
 * @{
 * @file
 * @brief       Driver for AT24MAC unique ID chip.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>

#include "at24mac.h"
#include "at24mac_params.h"

#define CMD_READ_EUI48      (0x9A)
#define CMD_READ_EUI64      (0x98)
#define CMD_READ_ID128      (0x80)

static int _read_reg(unsigned idx, uint8_t reg, void *dst, size_t size)
{
    if (idx >= ARRAY_SIZE(at24mac_params)) {
        return -ERANGE;
    }

    int res = 0;
    const at24mac_params_t *params = &at24mac_params[idx];

    res = i2c_acquire(params->i2c_dev);
    if (res) {
        return res;
    }

    res = i2c_read_regs(params->i2c_dev, params->i2c_addr,
                        reg, dst, size, 0);

    i2c_release(params->i2c_dev);

    return res;
}

#ifdef MODULE_AT24MAC4XX
int at24mac_get_eui48(unsigned idx, eui48_t *dst)
{
    return _read_reg(idx, CMD_READ_EUI48, dst, sizeof(*dst));
}
#endif

#ifdef MODULE_AT24MAC6XX
int at24mac_get_eui64(unsigned idx, eui64_t *dst)
{
    return _read_reg(idx, CMD_READ_EUI64, dst, sizeof(*dst));
}
#endif

int at24mac_get_id128(unsigned idx, void *dst)
{
    return _read_reg(idx, CMD_READ_ID128, dst, AT24MAC_ID_LEN);
}
