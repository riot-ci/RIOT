/*
 * Copyright (C) 2017 HAW-Habmurg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     auto_init_mtd_sdcard
 * @{
 *
 * @file
 * @brief       Auto initialization for mtd devices using sdcard_spi
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifdef MODULE_MTD_SDCARD

#include "log.h"
#include "sdcard_spi.h"
#include "sdcard_spi_params.h"
#include "mtd_sdcard.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Memory for sdcard_spi devs is allocated by auto_init_sdcard_spi.c
 * @{
 */
extern sdcard_spi_t sdcard_spi_devs[SDCARD_SPI_NUM];
/** @} */

/**
 * @brief   Allocate memory for the mtd descriptors
 * @{
 */
mtd_sdcard_t mtd_sdcard_devs[SDCARD_SPI_NUM];
/** @} */

void auto_init_mtd_sdcard(void)
{
    for (unsigned i = 0; i < SDCARD_SPI_NUM; i++) {

        LOG_DEBUG("[auto_init_storage] initializing mtd_sdcard #%u\n", i);

        mtd_sdcard_devs[i].base.driver = &mtd_sdcard_driver;
        mtd_sdcard_devs[i].sd_card = &sdcard_spi_devs[i];
        mtd_sdcard_devs[i].params = &sdcard_spi_params[i];

        if (mtd_init((mtd_dev_t*)&mtd_sdcard_devs[i]) != 0) {
            LOG_ERROR("[auto_init_storage] error initializing mtd_sdcard #%u\n", i);
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SDCARD_SPI */
/** @} */
