/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mtd
 * @{
 * @brief       Low level Memory Technology Device interface
 *
 * Generic memory technology device interface
 *
 * @file
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#include <errno.h>

#include "mtd.h"

int mtd_init(mtd_dev_t *mtd)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->init) {
        return mtd->driver->init(mtd);
    }
    else {
        return -ENOTSUP;
    }
}

int mtd_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->read) {
        return mtd->driver->read(mtd, dest, addr, count);
    }
    else {
        return -ENOTSUP;
    }
}

int mtd_read_page(mtd_dev_t *mtd, void *dest, uint32_t page, uint32_t offset,
                  uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->read_page == NULL) {
        return mtd_read(mtd, dest, mtd->page_size * page + offset, count);
    }

    char *_dst = dest;

    /* page size is always a power of two */
    const uint32_t page_shift = 32 - __builtin_clz(mtd->page_size) - 1;
    const uint32_t page_mask = mtd->page_size - 1;

    page  += offset >> page_shift;
    offset = offset & page_mask;

    while (count) {
        int read = mtd->driver->read_page(mtd, _dst, page, offset, count);

        if (read < 0) {
            return read;
        }

        count -= read;

        if (count == 0) {
            break;
        }

        _dst   += read;
        page   += (offset + read) >> page_shift;
        offset  = (offset + read) & page_mask;
    }

    return 0;
}

int mtd_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->write) {
        return mtd->driver->write(mtd, src, addr, count);
    }
    else {
        return -ENOTSUP;
    }
}

int mtd_write_page(mtd_dev_t *mtd, const void *src, uint32_t page, uint32_t offset,
                   uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->write_page == NULL) {
        return mtd_write(mtd, src, page * mtd->page_size + offset, count);
    }

    const char *_src = src;

    /* page size is always a power of two */
    const uint32_t page_shift = 32 - __builtin_clz(mtd->page_size) - 1;
    const uint32_t page_mask = mtd->page_size - 1;

    page  += offset >> page_shift;
    offset = offset & page_mask;

    while (count) {
        int written = mtd->driver->write_page(mtd, _src, page, offset, count);

        if (written < 0) {
            return written;
        }

        count -= written;

        if (count == 0) {
            break;
        }

        _src   += written;
        page   += (offset + written) >> page_shift;
        offset  = (offset + written) & page_mask;
    }

    return 0;
}

int mtd_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->erase) {
        return mtd->driver->erase(mtd, addr, count);
    }
    else {
        return -ENOTSUP;
    }
}

int mtd_erase_sector(mtd_dev_t *mtd, uint32_t sector, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->erase_sector) {
        return mtd->driver->erase_sector(mtd, sector, count);
    }
    else {
        uint32_t sector_size = mtd->pages_per_sector * mtd->page_size;
        return mtd_erase(mtd, sector * sector_size, count * sector_size);
    }
}

int mtd_power(mtd_dev_t *mtd, enum mtd_power_state power)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->power) {
        return mtd->driver->power(mtd, power);
    }
    else {
        return -ENOTSUP;
    }
}

/** @} */
