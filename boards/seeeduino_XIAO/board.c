/*
 * Copyright (C)  2021 Franz Freitag, Justus Krebs, Nick Weiler 
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_seeedurino_XIAO
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Seeeduino XIAO board
 *
 * @author      Franz Freitag <franz.freitag@st.ovgu.de>, Justus Krebs <justus.krebs@st.ovgu.de>, Nick Weiler <nick.weiler@st.ovgu.de>, Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "mtd.h"
#include "mtd_spi_nor.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "timex.h"

#ifdef MODULE_MTD
/* GD25Q32C */
static const mtd_spi_nor_params_t _seeduino_XIAO_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 15LU * US_PER_SEC,
    .wait_32k_erase = 250LU * US_PER_MS,
    .wait_sector_erase = 50LU * US_PER_MS,
    .wait_chip_wake_up = 1LU * US_PER_MS,
    .clk = SEEEDUINO_XIAO_NOR_SPI_CLK,
    .flag = SEEEDUINO_XIAO_NOR_FLAGS,
    .spi = SEEEDUINO_XIAO_NOR_SPI_DEV,
    .mode = SEEEDUINO_XIAO_NOR_SPI_MODE,
    .cs = SEEEDUINI_XIAO_NOR_SPI_CS,
    .wp = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
    .addr_width = 3,
};

static mtd_spi_nor_t seeeduino_XIAO_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = SEEEDUINO_XIAO_NOR_PAGE_SIZE,
        .pages_per_sector = SEEEDUINO_XIAO_NOR_PAGES_PER_SECTOR,
        .sector_count = SEEEDUINO_XIAO_NOR_SECTOR_COUNT,
    },
    .params = &_seeeduino_XIAO_nor_params,
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&seeeduino_XIAO_nor_dev;
#endif /* MODULE_MTD */

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the on-board red LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    LED0_OFF;
    LED1_OFF;
    LED2_OFF;
}
