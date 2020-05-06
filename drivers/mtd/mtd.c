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

int mtd_read_page(mtd_dev_t *mtd, void *dest, uint32_t page, uint16_t offset, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->read_page) {
        return mtd->driver->read_page(mtd, dest, page, offset, count);
    }
    else {
        return mtd_read(mtd, dest, page * mtd->page_size + offset, count);
    }

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

int mtd_write_page(mtd_dev_t *mtd, const void *src, uint32_t page, uint16_t offset, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->write_page) {
        return mtd->driver->write_page(mtd, src, page, offset, count);
    }
    else {
        return mtd_write(mtd, src, page * mtd->page_size + offset, count);
    }

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

int mtd_erase_page(mtd_dev_t *mtd, uint32_t page, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->erase_page) {
        return mtd->driver->erase_page(mtd, page, count);
    }
    else {
        return mtd_erase(mtd, page * mtd->page_size, count * mtd->page_size);
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
