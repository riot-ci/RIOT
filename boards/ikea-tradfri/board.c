/*
 * Copyright (C) 2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_ikea-tradfri
 * @{
 *
 * @file
 * @brief       Board specific implementations IKEA TRÅDFRI modules
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

#include "mtd_spi_nor.h"

static mtd_spi_nor_t board_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 16,
        .sector_count = 64,
    },
    .opcode = &mtd_spi_nor_opcode_default,
    .spi = TRADFRI_SPI_FLASH_DEV,
    .cs = TRADFRI_SPI_FLASH_CS,
    .addr_width = 3,
    .mode = SPI_MODE_0,
    .clk = SPI_CLK_5MHZ,
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&board_nor_dev;

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

#ifndef RIOTBOOT
    /* initialize the LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
#endif
}
