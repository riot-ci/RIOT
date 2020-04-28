/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_at25xxx_mtd_wrapper
 * @{
 *
 * @file
 * @brief       MTD wrapper for spi eeprom (like AT25xxx, M95xxx, 25AAxxx, 25LCxxx,
 *              CAT25xxx & BR25Sxxx)
 *
 * @author      Johannes Koster <johannes.koster@ml-pa.com>
 *
 * @}
 */
#define ENABLE_DEBUG (0)
#include "debug.h"
#include "mtd.h"
#include "at25xxx/mtd_wrapper.h"
#include "at25xxx.h"
#include "at25xxx_params.h"

#include <inttypes.h>
#include <errno.h>

static int mtd_at25xxx_init(mtd_dev_t *mtd);
static int mtd_at25xxx_read(mtd_dev_t *mtd, void *dest, uint32_t addr,
                           uint32_t size);
static int mtd_at25xxx_write(mtd_dev_t *mtd, const void *src, uint32_t addr,
                            uint32_t size);
static int mtd_at25xxx_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t size);
static int mtd_at25xxx_power(mtd_dev_t *mtd, enum mtd_power_state power);

static int mtd_at25xxx_init(mtd_dev_t *dev)
{
    DEBUG("[at25xxx/mtd_wrapper] initializing\n");
    mtd_at25xxx_t *mtd_at25xxx = (mtd_at25xxx_t*)dev;
    if (at25xxx_init(mtd_at25xxx->at25xxx_eeprom, mtd_at25xxx->params) == 0)
    {
        dev->pages_per_sector = 1;
        dev->page_size        = mtd_at25xxx->params->page_size;
        return 0;
    }
    return -EIO;
}

static int mtd_at25xxx_read(mtd_dev_t *dev, void *buff, uint32_t addr,
                           uint32_t size)
{
    DEBUG("[at25xxx/mtd_wrapper] read: addr:%" PRIu32 " size:%" PRIu32 "\n", addr, size);
    mtd_at25xxx_t *mtd_at25xxx_ = (mtd_at25xxx_t*)dev;
    return at25xxx_read(mtd_at25xxx_->at25xxx_eeprom, addr, buff, size);
}

static int mtd_at25xxx_write(mtd_dev_t *dev, const void *buff, uint32_t addr,
                            uint32_t size)
{
    DEBUG("[at25xxx/mtd_wrapper] write: addr:%" PRIu32 " size:%" PRIu32 "\n", addr, size);
    mtd_at25xxx_t *mtd_at25xxx_ = (mtd_at25xxx_t*)dev;
    return at25xxx_write(mtd_at25xxx_->at25xxx_eeprom, addr,
                                    buff, size);
}

static int mtd_at25xxx_erase(mtd_dev_t *dev,
                            uint32_t addr,
                            uint32_t size)
{
    DEBUG("[at25xxx/mtd_wrapper] mtd_at25xxx_erase: addr:%" PRIu32 " size:%" PRIu32 "\n", addr, size);
    mtd_at25xxx_t *mtd_at25xxx_ = (mtd_at25xxx_t*)dev;
    return at25xxx_clear(mtd_at25xxx_->at25xxx_eeprom, addr, size);
}

static int mtd_at25xxx_power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void)dev;
    (void)power;

    /* TODO: implement power down/up of EEPROM (at25xxx driver?)
    */
    return -ENOTSUP; /* currently not supported */
}

const mtd_desc_t mtd_at25xxx_driver = {
    .init = mtd_at25xxx_init,
    .read = mtd_at25xxx_read,
    .write = mtd_at25xxx_write,
    .erase = mtd_at25xxx_erase,
    .power = mtd_at25xxx_power,
};