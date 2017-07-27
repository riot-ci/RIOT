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

#ifdef FEATURE_PERIPH_HWRNG

#include "periph/hwrng.h"
#include "vfs.h"

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

#else
typedef int dont_be_pedantic;
#endif
