/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pcd8544
 * @{
 * @file
 * @brief       Implementation of the SPI driver for the PDC8544 graphics display
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "xtimer.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "pcd8544.h"
#include "pcd8544_internal.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define ASCII_MIN           0x20    /**< start of ASCII table */
#define ASCII_MAX           0x7e    /**< end of ASCII table */
#define CHAR_WIDTH          (6U)    /**< pixel width of a single character */

#define SPI_CLK             (SPI_CLK_1MHZ)
#define SPI_MODE            (SPI_MODE_0)

static const uint8_t _ascii[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00},/* 20 SPACE*/
    {0x00, 0x00, 0x5f, 0x00, 0x00},/* 21 ! */
    {0x00, 0x07, 0x00, 0x07, 0x00},/* 22 " */
    {0x14, 0x7f, 0x14, 0x7f, 0x14},/* 23 # */
    {0x24, 0x2a, 0x7f, 0x2a, 0x12},/* 24 $ */
    {0x23, 0x13, 0x08, 0x64, 0x62},/* 25 % */
    {0x36, 0x49, 0x55, 0x22, 0x50},/* 26 & */
    {0x00, 0x05, 0x03, 0x00, 0x00},/* 27 ' */
    {0x00, 0x1c, 0x22, 0x41, 0x00},/* 28 ( */
    {0x00, 0x41, 0x22, 0x1c, 0x00},/* 29 ) */
    {0x14, 0x08, 0x3e, 0x08, 0x14},/* 2a * */
    {0x08, 0x08, 0x3e, 0x08, 0x08},/* 2b + */
    {0x00, 0x50, 0x30, 0x00, 0x00},/* 2c , */
    {0x08, 0x08, 0x08, 0x08, 0x08},/* 2d - */
    {0x00, 0x60, 0x60, 0x00, 0x00},/* 2e . */
    {0x20, 0x10, 0x08, 0x04, 0x02},/* 2f / */
    {0x3e, 0x51, 0x49, 0x45, 0x3e},/* 30 0 */
    {0x00, 0x42, 0x7f, 0x40, 0x00},/* 31 1 */
    {0x42, 0x61, 0x51, 0x49, 0x46},/* 32 2 */
    {0x21, 0x41, 0x45, 0x4b, 0x31},/* 33 3 */
    {0x18, 0x14, 0x12, 0x7f, 0x10},/* 34 4 */
    {0x27, 0x45, 0x45, 0x45, 0x39},/* 35 5 */
    {0x3c, 0x4a, 0x49, 0x49, 0x30},/* 36 6 */
    {0x01, 0x71, 0x09, 0x05, 0x03},/* 37 7 */
    {0x36, 0x49, 0x49, 0x49, 0x36},/* 38 8 */
    {0x06, 0x49, 0x49, 0x29, 0x1e},/* 39 9 */
    {0x00, 0x36, 0x36, 0x00, 0x00},/* 3a : */
    {0x00, 0x56, 0x36, 0x00, 0x00},/* 3b ; */
    {0x08, 0x14, 0x22, 0x41, 0x00},/* 3c < */
    {0x14, 0x14, 0x14, 0x14, 0x14},/* 3d = */
    {0x00, 0x41, 0x22, 0x14, 0x08},/* 3e > */
    {0x02, 0x01, 0x51, 0x09, 0x06},/* 3f ? */
    {0x32, 0x49, 0x79, 0x41, 0x3e},/* 40 @ */
    {0x7e, 0x11, 0x11, 0x11, 0x7e},/* 41 A */
    {0x7f, 0x49, 0x49, 0x49, 0x36},/* 42 B */
    {0x3e, 0x41, 0x41, 0x41, 0x22},/* 43 C */
    {0x7f, 0x41, 0x41, 0x22, 0x1c},/* 44 D */
    {0x7f, 0x49, 0x49, 0x49, 0x41},/* 45 E */
    {0x7f, 0x09, 0x09, 0x09, 0x01},/* 46 F */
    {0x3e, 0x41, 0x49, 0x49, 0x7a},/* 47 G */
    {0x7f, 0x08, 0x08, 0x08, 0x7f},/* 48 H */
    {0x00, 0x41, 0x7f, 0x41, 0x00},/* 49 I */
    {0x20, 0x40, 0x41, 0x3f, 0x01},/* 4a J */
    {0x7f, 0x08, 0x14, 0x22, 0x41},/* 4b K */
    {0x7f, 0x40, 0x40, 0x40, 0x40},/* 4c L */
    {0x7f, 0x02, 0x0c, 0x02, 0x7f},/* 4d M */
    {0x7f, 0x04, 0x08, 0x10, 0x7f},/* 4e N */
    {0x3e, 0x41, 0x41, 0x41, 0x3e},/* 4f O */
    {0x7f, 0x09, 0x09, 0x09, 0x06},/* 50 P */
    {0x3e, 0x41, 0x51, 0x21, 0x5e},/* 51 Q */
    {0x7f, 0x09, 0x19, 0x29, 0x46},/* 52 R */
    {0x46, 0x49, 0x49, 0x49, 0x31},/* 53 S */
    {0x01, 0x01, 0x7f, 0x01, 0x01},/* 54 T */
    {0x3f, 0x40, 0x40, 0x40, 0x3f},/* 55 U */
    {0x1f, 0x20, 0x40, 0x20, 0x1f},/* 56 V */
    {0x3f, 0x40, 0x38, 0x40, 0x3f},/* 57 W */
    {0x63, 0x14, 0x08, 0x14, 0x63},/* 58 X */
    {0x07, 0x08, 0x70, 0x08, 0x07},/* 59 Y */
    {0x61, 0x51, 0x49, 0x45, 0x43},/* 5a Z */
    {0x00, 0x7f, 0x41, 0x41, 0x00},/* 5b [ */
    {0x02, 0x04, 0x08, 0x10, 0x20},/* 5c \ */
    {0x00, 0x41, 0x41, 0x7f, 0x00},/* 5d ] */
    {0x04, 0x02, 0x01, 0x02, 0x04},/* 5e ^ */
    {0x40, 0x40, 0x40, 0x40, 0x40},/* 5f _ */
    {0x00, 0x01, 0x02, 0x04, 0x00},/* 60 ` */
    {0x20, 0x54, 0x54, 0x54, 0x78},/* 61 a */
    {0x7f, 0x48, 0x44, 0x44, 0x38},/* 62 b */
    {0x38, 0x44, 0x44, 0x44, 0x20},/* 63 c */
    {0x38, 0x44, 0x44, 0x48, 0x7f},/* 64 d */
    {0x38, 0x54, 0x54, 0x54, 0x18},/* 65 e */
    {0x08, 0x7e, 0x09, 0x01, 0x02},/* 66 f */
    {0x0c, 0x52, 0x52, 0x52, 0x3e},/* 67 g */
    {0x7f, 0x08, 0x04, 0x04, 0x78},/* 68 h */
    {0x00, 0x44, 0x7d, 0x40, 0x00},/* 69 i */
    {0x20, 0x40, 0x44, 0x3d, 0x00},/* 6a j */
    {0x7f, 0x10, 0x28, 0x44, 0x00},/* 6b k */
    {0x00, 0x41, 0x7f, 0x40, 0x00},/* 6c l */
    {0x7c, 0x04, 0x18, 0x04, 0x78},/* 6d m */
    {0x7c, 0x08, 0x04, 0x04, 0x78},/* 6e n */
    {0x38, 0x44, 0x44, 0x44, 0x38},/* 6f o */
    {0x7c, 0x14, 0x14, 0x14, 0x08},/* 70 p */
    {0x08, 0x14, 0x14, 0x18, 0x7c},/* 71 q */
    {0x7c, 0x08, 0x04, 0x04, 0x08},/* 72 r */
    {0x48, 0x54, 0x54, 0x54, 0x20},/* 73 s */
    {0x04, 0x3f, 0x44, 0x40, 0x20},/* 74 t */
    {0x3c, 0x40, 0x40, 0x20, 0x7c},/* 75 u */
    {0x1c, 0x20, 0x40, 0x20, 0x1c},/* 76 v */
    {0x3c, 0x40, 0x30, 0x40, 0x3c},/* 77 w */
    {0x44, 0x28, 0x10, 0x28, 0x44},/* 78 x */
    {0x0c, 0x50, 0x50, 0x50, 0x3c},/* 79 y */
    {0x44, 0x64, 0x54, 0x4c, 0x44},/* 7a z */
    {0x00, 0x08, 0x36, 0x41, 0x00},/* 7b { */
    {0x00, 0x00, 0x7f, 0x00, 0x00},/* 7c | */
    {0x00, 0x41, 0x36, 0x08, 0x00},/* 7d } */
    {0x10, 0x08, 0x08, 0x10, 0x08},/* 7e ~ */
};

static const uint8_t _riot[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfc, 0x7e,
    0x3e, 0x3e, 0x1f, 0x1f, 0x1f, 0x1f, 0x3f, 0x3e,
    0x7e, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff,
    0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
    0xc0, 0xc0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xfc, 0xfc,
    0x7e, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xc0, 0xf0, 0xf8, 0xfc, 0xfe, 0x3f,
    0x1f, 0x0f, 0x0f, 0x07, 0x07, 0x07, 0x03, 0x03,
    0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff,
    0xfe, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x83,
    0x81, 0x81, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
    0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x03, 0x0f, 0x3f, 0xff, 0xff,
    0xfc, 0xf0, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x0f, 0x1f, 0x3f, 0x3f, 0x7e,
    0x7c, 0x78, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0x78,
    0x7c, 0x7e, 0x3f, 0x3f, 0x1f, 0x0f, 0x07, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x03, 0x0f, 0x1f, 0x3f, 0x3f,
    0x1f, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static inline void lock(const pcd8544_t *dev)
{
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
}

static inline void done(const pcd8544_t *dev)
{
    spi_release(dev->spi);
}

static void _write(const pcd8544_t *dev, uint8_t is_data, uint8_t data)
{
    /* set command or data mode */
    gpio_write(dev->mode, is_data);
    /* write byte to LCD */
    spi_transfer_bytes(dev->spi, dev->cs, false, &data, NULL, 1);
}

static inline void _set_x(const pcd8544_t *dev, uint8_t x)
{
    _write(dev, MODE_CMD, CMD_SET_X | x);
}

static inline void _set_y(const pcd8544_t *dev, uint8_t y)
{
    _write(dev, MODE_CMD, CMD_SET_Y | y);
}

int pcd8544_init(pcd8544_t *dev, spi_t spi, gpio_t cs, gpio_t reset, gpio_t mode)
{
    /* save pin mapping */
    dev->spi = spi;
    dev->cs = cs;
    dev->reset = reset;
    dev->mode = mode;
    dev->inverted = 0;

    DEBUG("done setting dev members\n");

    /* initialize pins */
    gpio_init(reset, GPIO_OUT);
    gpio_init(mode, GPIO_OUT);
    DEBUG("done with gpios\n");
    /* clear CS line */
    DEBUG("done clearing CS line\n");
    /* initialize SPI */
    spi_init_cs(spi, (spi_cs_t)cs);
    DEBUG("done initializing SPI master\n");
    /* reset display */
    gpio_clear(reset);
    xtimer_usleep(RESET_DELAY);
    gpio_set(reset);

    /* clear display memory */
    pcd8544_clear(dev);
    /* write initialization sequence to display */
    pcd8544_set_contrast(dev, PCD8544_DEFAULT_CONTRAST);
    pcd8544_set_bias(dev, PCD8544_DEFAULT_BIAS);
    pcd8544_set_tempcoef(dev, PCD8544_DEFAULT_TEMPCOEF);
    /* enable display */
    lock(dev);
    _write(dev, MODE_CMD, CMD_ENABLE_H);
    _write(dev, MODE_CMD, CMD_MODE_NORMAL);
    done(dev);
    return 0;
}

void pcd8544_set_contrast(const pcd8544_t *dev, uint8_t contrast)
{
    if (contrast > CONTRAST_MAX) {
        contrast = CONTRAST_MAX;
    }
    lock(dev);
    _write(dev, MODE_CMD, CMD_EXTENDED);
    _write(dev, MODE_CMD, (CMD_EXT_CONTRAST | contrast));
    _write(dev, MODE_CMD, CMD_ENABLE_H);
    done(dev);
}

void pcd8544_set_tempcoef(const pcd8544_t *dev, uint8_t coef)
{
    if (coef > TEMP_MAX) {
        coef = TEMP_MAX;
    }
    lock(dev);
    _write(dev, MODE_CMD, CMD_EXTENDED);
    _write(dev, MODE_CMD, (CMD_EXT_TEMP | coef));
    _write(dev, MODE_CMD, CMD_ENABLE_H);
    done(dev);
}

void pcd8544_set_bias(const pcd8544_t *dev, uint8_t bias)
{
    if (bias > BIAS_MAX) {
        bias = BIAS_MAX;
    }
    lock(dev);
    _write(dev, MODE_CMD, CMD_EXTENDED);
    _write(dev, MODE_CMD, (CMD_EXT_BIAS | bias));
    _write(dev, MODE_CMD, CMD_ENABLE_H);
    done(dev);
}

void pcd8544_riot(const pcd8544_t *dev)
{
    pcd8544_write_img(dev, _riot);
}

void pcd8544_write_img(const pcd8544_t *dev, const uint8_t img[])
{
    /* set initial position */
    lock(dev);
    _set_x(dev, 0);
    _set_y(dev, 0);
    /* write image data to display */
    for (unsigned i = 0; i < (PCD8544_RES_X * PCD8544_RES_Y / 8); i++) {
        _write(dev, MODE_DTA, img[i]);
    }
    done(dev);
}

void pcd8544_write_c(const pcd8544_t *dev, uint8_t x, uint8_t y, char c)
{
    /* check position */
    if (x >= PCD8544_COLS || y >= PCD8544_ROWS) {
        return ;
    }
    /* set position */
    lock(dev);
    _set_x(dev, x * CHAR_WIDTH);
    _set_y(dev, y);
    /* write char */
    for (unsigned i = 0; i < CHAR_WIDTH - 1; i++) {
        _write(dev, MODE_DTA, _ascii[c - ASCII_MIN][i]);
    }
    _write(dev, MODE_DTA, 0x00);
    done(dev);
}

void pcd8544_write_s(const pcd8544_t *dev, uint8_t x, uint8_t y, const char *s)
{
    for (; (*s != '\0') && x < PCD8544_COLS; x++, s++) {
        pcd8544_write_c(dev, x, y, *s);
    }
}

void pcd8544_clear(const pcd8544_t *dev)
{
    lock(dev);
    _set_x(dev, 0);
    _set_y(dev, 0);
    for (unsigned i = 0; i < PCD8544_RES_X * PCD8544_ROWS; i++) {
        _write(dev, MODE_DTA, 0x00);
    }
    done(dev);
}

void pcd8544_invert(pcd8544_t *dev)
{
    lock(dev);
    if (dev->inverted) {
        _write(dev, MODE_CMD, CMD_MODE_NORMAL);
    }
    else {
        _write(dev, MODE_CMD, CMD_MODE_INVERSE);
    }
    dev->inverted ^= 0x01;
    done(dev);
}

int pcd8544_is_inverted(const pcd8544_t *dev)
{
    return dev->inverted;
}

void pcd8544_poweron(const pcd8544_t *dev)
{
    lock(dev);
    _write(dev, MODE_CMD, CMD_ENABLE_H);
    done(dev);
}

void pcd8544_poweroff(const pcd8544_t *dev)
{
    lock(dev);
    _write(dev, MODE_CMD, CMD_DISABLE);
    done(dev);
}
