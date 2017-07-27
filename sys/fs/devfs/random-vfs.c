/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_fs_devfs
 * @{
 *
 * @file
 * @brief       Random backends for devfs implementation
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include "vfs.h"

#ifdef MODULE_PERIPH_HWRNG

#include "periph/hwrng.h"

static ssize_t hwrng_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes);
static int hwrng_vfs_open(vfs_file_t *filp, const char *name, int flags, mode_t mode, const char *abs_path);

const vfs_file_ops_t hwrng_vfs_ops = {
    .open = hwrng_vfs_open,
    .read  = hwrng_vfs_read,
};

static int hwrng_vfs_open(vfs_file_t *filp, const char *name, int flags, mode_t mode, const char *abs_path)
{
    (void)filp;
    (void)name;
    (void)flags;
    (void)mode;
    (void)abs_path;

    hwrng_init();

    return 0;
}

static ssize_t hwrng_vfs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    (void)filp;

    hwrng_read(dest, nbytes);

    return nbytes;
}
#endif /* MODULE_PERIPH_HWRNG */

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
#endif /* MODULE_RANDOM */
