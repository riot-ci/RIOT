/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file
 * @brief       HW RNG backend for devfs
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include "vfs.h"

#ifdef FEATURE_PERIPH_HWRNG

#include "periph/hwrng.h"

static ssize_t hwrng_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes);

const vfs_file_ops_t hwrng_vfs_ops = {
    .read  = hwrng_vfs_read,
};

static ssize_t hwrng_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    (void)filp;

    hwrng_read(dest, nbytes);

    return nbytes;
}
#endif

#ifdef MODULE_RANDOM

#include "random.h"

static ssize_t random_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes);

const vfs_file_ops_t random_vfs_ops = {
    .read = random_vfs_read,
};

static ssize_t random_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    (void)filp;
    uint32_t random;
    uint8_t *random_pos = (uint8_t*)&random;
    uint8_t *target = dest;
    size_t n = nbytes;
    size_t _n = 0;

    while (n--) {
        if (! (_n++ & 0x3)) {
            random = random_uint32();
            random_pos = (uint8_t *) &random;
        }
        *target++ = *random_pos++;
    }

    return nbytes;
}
#endif
