/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mtd_flashpage
 * @brief       Driver for internal flash devices implementing flashpage interface
 *
 * @{
 *
 * @file
 * @brief       Implementation for the flashpage memory driver
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include <string.h>
#include <errno.h>

#include "mtd_flashpage.h"
#include "periph/flashpage.h"

static int _init(mtd_dev_t *dev)
{
    (void)dev;
    assert(dev->pages_per_sector * dev->page_size == FLASHPAGE_SIZE);
    return 0;
}

static int _read(mtd_dev_t *dev, void *buf, uint32_t addr, uint32_t size)
{
    (void)dev;
    memcpy(buf, (void *)addr, size);

    return size;
}

static int _write(mtd_dev_t *dev, const void *buf, uint32_t addr, uint32_t size)
{
    if (addr % FLASHPAGE_RAW_ALIGNMENT) {
        return -EINVAL;
    }
    if ((uintptr_t)buf % FLASHPAGE_RAW_ALIGNMENT) {
        return -EINVAL;
    }
    if (size % FLASHPAGE_RAW_BLOCKSIZE) {
        return -EOVERFLOW;
    }
    if (addr + size > dev->pages_per_sector * dev->page_size * dev->sector_count) {
        return -EOVERFLOW;
    }
    flashpage_write_raw((void *)addr, buf, size);

    return size;
}

int _erase(mtd_dev_t *dev, uint32_t addr, uint32_t size)
{
    size_t sector_size = dev->page_size * dev->pages_per_sector;

    if (size % sector_size) {
        return -EOVERFLOW;
    }
    if (addr + size > sector_size * dev->sector_count) {
        return - EOVERFLOW;
    }
    if (addr % sector_size) {
        return - EOVERFLOW;
    }
    for (size_t i = 0; i < size; i += sector_size) {
        flashpage_write(flashpage_page((void *)addr), NULL);
    }

    return 0;
}


const mtd_desc_t mtd_flashpage_driver = {
    .init = _init,
    .read = _read,
    .write = _write,
    .erase = _erase,
};
