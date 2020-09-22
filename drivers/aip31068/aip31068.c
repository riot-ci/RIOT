/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_aip31068
 * @brief       Device driver for AIP31068
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 * @{
 */

#include <stdio.h>

#include "xtimer.h"

#include "aip31068.h"
#include "aip31068_regs.h"
#include "aip31068_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Write a data byte to the device.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] value     Data byte to write
 *
 * @return  AIP31068_OK on success
 * @return  -AIP31068_ERROR_I2C if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static inline int _data(aip31068_t *dev, uint8_t value);

/**
 * @brief Write a command byte with it's arguments (modified bits) to the device.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] value     Command byte to write
 *
 * @return  AIP31068_OK on success
 * @return  -AIP31068_ERROR_I2C if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static inline int _command(aip31068_t *dev, uint8_t value);

/**
 * @brief Write a command or data byte to the device.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] data_byte Byte to write
 * @param[in] is_cmd    Whether byte should be interpreted as data or command
 *
 * @return  AIP31068_OK on success
 * @return  -AIP31068_ERROR_I2C if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static inline int _write(aip31068_t *dev, uint8_t data_byte, bool is_cmd);

/**
 * @brief Write data to the device.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 * @param[in] data  Data to write
 * @param[in] len   Length of the data
 *
 * @return  AIP31068_OK on success
 * @return  -AIP31068_ERROR_I2C if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static int _device_write(aip31068_t *dev, uint8_t *data, uint8_t len);

/**
 * @brief Initialize controller for progress bar feature.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 * @param[in] row   Row where the progress bar is displayed
 */
static void _init_progress_bar(aip31068_t* dev, uint8_t row);

/**
 * Custom character 1 x 8 bar for progress bar.
 */
static const uint8_t custom_char_progress_bar_1[8] =
        { 16, 16, 16, 16, 16, 16, 16, 16 };

/**
 * Custom character 2 x 8 bar for progress bar.
 */
static const uint8_t custom_char_progress_bar_2[8] =
        { 24, 24, 24, 24, 24, 24, 24, 24 };

/**
 * Custom character 3 x 8 bar for progress bar.
 */
static const uint8_t custom_char_progress_bar_3[8] =
        { 28, 28, 28, 28, 28, 28, 28, 28 };

/**
 * Custom character 4 x 8 bar for progress bar.
 */
static const uint8_t custom_char_progress_bar_4[8] =
        { 30, 30, 30, 30, 30, 30, 30, 30 };

/**
 * Custom character 5 x 8 bar for progress bar.
 */
static const uint8_t custom_char_progress_bar_5[8] =
        { 31, 31, 31, 31, 31, 31, 31, 31 };

int aip31068_init(aip31068_t *dev, const aip31068_params_t *params)
{
    assert(dev);
    assert(params);

    /* displays with more than 4 lines are not supported
     * (see aip31068_set_cursor_position) */
    assert(params->row_count <= 4);

    dev->params = *params;
    dev->_curr_display_control = 0;
    dev->_curr_entry_mode_set = 0;

    i2c_init(dev->params.i2c_dev);

    uint8_t _function_set = 0;

    /* configure bit mode */
    if (params->bit_mode == BITMODE_8_BIT) {
        _function_set |= (1 << BIT_FUNCTION_SET_BITMODE);
    }

    /* configure line count */
    if (params->row_count >= 2) {
        _function_set |= (1 << BIT_FUNCTION_SET_LINECOUNT);
    }

    /* configure character size */
    if (params->font_size == FONT_SIZE_5x10) {
        _function_set |= (1 << BIT_FUNCTION_SET_FONTSIZE);
    }

    /* begin of initialization sequence (page 20 in the datasheet) */
    xtimer_usleep(50 * US_PER_MS);

    int rc = 0;
    int c = 0;

    /* send function set command sequence */
    do {
        rc = _command(dev, CMD_FUNCTION_SET | _function_set);
        xtimer_usleep(5 * US_PER_MS);

        printf("A rc = %d\n", rc);
        c++;

        if (c > 10) {
            return AIP31068_ERROR_I2C;
        }
    } while (rc != 0); // TODO: get rid of the loop if possible

    /* second try */
    rc = _command(dev, CMD_FUNCTION_SET | _function_set);
    if (rc < 0) {
        return rc;
    }

    xtimer_usleep(500);

    /* third go */
    rc = _command(dev, CMD_FUNCTION_SET | _function_set);
    if (rc < 0) {
        return rc;
    }

    rc = aip31068_turn_off(dev);
    if (rc < 0) {
        return rc;
    }

    rc = aip31068_clear(dev);
    if (rc < 0) {
        return rc;
    }

    rc = aip31068_set_text_insertion_mode(dev, LEFT_TO_RIGHT);
    if (rc < 0) {
        return rc;
    }

    return AIP31068_OK;
}

int aip31068_turn_on(aip31068_t *dev)
{
    dev->_curr_display_control |= (1 << BIT_DISPLAY_CONTROL_DISPLAY);

    return _command(dev, CMD_DISPLAY_CONTROL | dev->_curr_display_control);
}

int aip31068_turn_off(aip31068_t *dev)
{
    dev->_curr_display_control &= ~(1 << BIT_DISPLAY_CONTROL_DISPLAY);

    return _command(dev, CMD_DISPLAY_CONTROL | dev->_curr_display_control);
}

int aip31068_clear(aip31068_t *dev)
{
    int rc = _command(dev, CMD_CLEAR_DISPLAY);

    xtimer_usleep(EXECUTION_TIME_MAX);

    return rc;
}

int aip31068_return_home(aip31068_t *dev)
{
    int rc = _command(dev, CMD_RETURN_HOME);

    xtimer_usleep(EXECUTION_TIME_MAX);

    return rc;
}

int aip31068_set_auto_scroll_enabled(aip31068_t *dev, bool enabled)
{
    if (enabled) {
        dev->_curr_entry_mode_set |= (1 << BIT_ENTRY_MODE_AUTOINCREMENT);
    }
    else {
        dev->_curr_entry_mode_set &= ~(1 << BIT_ENTRY_MODE_AUTOINCREMENT);
    }

    return _command(dev, CMD_ENTRY_MODE_SET | dev->_curr_entry_mode_set);
}

int aip31068_set_cursor_blinking_enabled(aip31068_t *dev, bool enabled)
{
    if (enabled) {
        dev->_curr_display_control |= (1 << BIT_DISPLAY_CONTROL_CURSOR_BLINKING);
    }
    else {
        dev->_curr_display_control &= ~(1 << BIT_DISPLAY_CONTROL_CURSOR_BLINKING);
    }

    return _command(dev, CMD_DISPLAY_CONTROL | dev->_curr_display_control);
}

int aip31068_set_cursor_visible(aip31068_t *dev, bool visible)
{
    if (visible) {
        dev->_curr_display_control |= (1 << BIT_DISPLAY_CONTROL_CURSOR);
    }
    else {
        dev->_curr_display_control &= ~(1 << BIT_DISPLAY_CONTROL_CURSOR);
    }

    return _command(dev, CMD_DISPLAY_CONTROL | dev->_curr_display_control);
}

int aip31068_set_cursor_position(aip31068_t *dev, uint8_t row, uint8_t col)
{
    uint8_t row_offsets[4];
    row_offsets[0] = 0x00;
    row_offsets[1] = 0x40;
    row_offsets[2] = 0x00 + dev->params.col_count;
    row_offsets[3] = 0x40 + dev->params.col_count;;

    if (row >= dev->params.row_count) {
        row = dev->params.row_count - 1;
    }

    return _command(dev, CMD_SET_DDRAM_ADDR | (col | row_offsets[row]));
}

int aip31068_set_text_insertion_mode(aip31068_t *dev,
                                     aip31068_text_insertion_mode_t mode)
{
    if (mode == RIGHT_TO_LEFT) {
        dev->_curr_entry_mode_set &= ~(1 << BIT_ENTRY_MODE_INCREMENT);
    }
    else {
        dev->_curr_entry_mode_set |= (1 << BIT_ENTRY_MODE_INCREMENT);
    }

    return _command(dev, CMD_ENTRY_MODE_SET | dev->_curr_entry_mode_set);
}

int aip31068_move_cursor_left(aip31068_t *dev)
{
    uint8_t cmd = CMD_CURSOR_DISPLAY_SHIFT;
    cmd &= ~(1 << BIT_CURSOR_DISPLAY_SHIFT_DIRECTION);

    return _command(dev, cmd);
}

int aip31068_move_cursor_right(aip31068_t *dev)
{
    uint8_t cmd = CMD_CURSOR_DISPLAY_SHIFT;
    cmd |= (1 << BIT_CURSOR_DISPLAY_SHIFT_DIRECTION);

    return _command(dev, cmd);
}

int aip31068_scroll_display_left(aip31068_t *dev)
{
    uint8_t cmd = CMD_CURSOR_DISPLAY_SHIFT;
    cmd |= (1 << BIT_CURSOR_DISPLAY_SHIFT_SELECTION);
    cmd &= ~(1 << BIT_CURSOR_DISPLAY_SHIFT_DIRECTION);

    return _command(dev, cmd);
}

int aip31068_scroll_display_right(aip31068_t *dev)
{
    uint8_t cmd = CMD_CURSOR_DISPLAY_SHIFT;
    cmd |= (1 << BIT_CURSOR_DISPLAY_SHIFT_SELECTION);
    cmd |= (1 << BIT_CURSOR_DISPLAY_SHIFT_DIRECTION);

    return _command(dev, cmd);
}

int aip31068_set_custom_symbol(aip31068_t *dev,
                               aip31068_custom_symbol_t custom_symbol,
                               const uint8_t charmap[])
{
    /* Bits 0-2 define the row address of a custom character in CGRAM.
     * Bits 3-5 define the base address of a custom character in CGRAM. */
    uint8_t location = custom_symbol << 3;
    int rc = _command(dev, CMD_SET_CGRAM_ADDR | location);

    if (rc < 0) {
        return rc;
    }

    /* How many rows are necessary for a complete character for given font? */
    int row_count = dev->params.font_size == FONT_SIZE_5x8 ? 8 : 10;

    for (int i = 0; i < row_count; i++) {
        rc = _data(dev, charmap[i]);

        if (rc < 0) {
            return rc;
        }
    }

    // todo: return to old cursor position
    return _command(dev, CMD_SET_DDRAM_ADDR);
}

int aip31068_print_custom_symbol(aip31068_t *dev,
                                 aip31068_custom_symbol_t custom_symbol)
{
    return _data(dev, (uint8_t) custom_symbol);
}

int aip31068_print(aip31068_t *dev, const char *data)
{
    while (*data != '\0') {
        int rc;
        if ((rc = _data(dev, *data)) != 0) {
            return rc;
        }
        data++;
    }

    return AIP31068_OK;
}

int aip31068_print_char(aip31068_t *dev, char c)
{
    return _data(dev, c);
}

void aip31068_set_progress_bar_enabled(aip31068_t *dev, bool enabled)
{
    dev->_progress_bar_enabled = enabled;

    if (enabled) {
        _init_progress_bar(dev, dev->params.row_count - 1);
    }
}

void aip31068_set_progress_bar_row(aip31068_t *dev, uint8_t row)
{
    dev->_progress_bar_row = row;
}

void aip31068_set_progress(aip31068_t *dev, uint8_t progress)
{
    if (!dev->_progress_bar_enabled) {
        return;
    }

    /* calculate the number of pixel-columns on a single line */
    int bar_count = dev->params.col_count * 5;

    if (progress > 100) {
        progress = 100;
    }

    /* How many bars to display for given progress? */
    int progress_bar_count = bar_count * progress / 100;

    /* number of completely filled sections / characters */
    int full_bar_count = progress_bar_count / 5;

    /* number of bars in the last section / character remaining */
    int remainder_bar_count = progress_bar_count % 5;

    aip31068_set_cursor_position(dev, dev->_progress_bar_row, 0);

    for (int i = 0; i < full_bar_count; i++) {
        aip31068_print_custom_symbol(dev, CUSTOM_SYMBOL_8);
    }

    uint8_t blank_count = dev->params.col_count - full_bar_count;

    switch (remainder_bar_count) {

        case 1:
            aip31068_print_custom_symbol(dev, CUSTOM_SYMBOL_4);
            blank_count--;
            break;

        case 2:
            aip31068_print_custom_symbol(dev, CUSTOM_SYMBOL_5);
            blank_count--;
            break;

        case 3:
            aip31068_print_custom_symbol(dev, CUSTOM_SYMBOL_6);
            blank_count--;
            break;

        case 4:
            aip31068_print_custom_symbol(dev, CUSTOM_SYMBOL_7);
            blank_count--;
            break;
    }

    /* clear the rest of the line, so it appears as empty part of the progressbar */
    for (int i = 0; i < blank_count; i++) {
        aip31068_print(dev, " ");
    }
}

static void _init_progress_bar(aip31068_t* dev, uint8_t row)
{
    dev->_progress_bar_row = row;

    /* if autoscroll was used, the progress bar would be displayed incorrectly */
    aip31068_set_auto_scroll_enabled(dev, false);

    /* undo any scrolling */
    aip31068_return_home(dev);

    /* progress bar should increase from left to right */
    aip31068_set_text_insertion_mode(dev, LEFT_TO_RIGHT);

    aip31068_set_custom_symbol(dev, CUSTOM_SYMBOL_4, custom_char_progress_bar_1);
    aip31068_set_custom_symbol(dev, CUSTOM_SYMBOL_5, custom_char_progress_bar_2);
    aip31068_set_custom_symbol(dev, CUSTOM_SYMBOL_6, custom_char_progress_bar_3);
    aip31068_set_custom_symbol(dev, CUSTOM_SYMBOL_7, custom_char_progress_bar_4);
    aip31068_set_custom_symbol(dev, CUSTOM_SYMBOL_8, custom_char_progress_bar_5);
}

static inline int _data(aip31068_t *dev, uint8_t value)
{
    int rc = _write(dev, value, false);

    xtimer_usleep(EXECUTION_TIME_DEFAULT);

    return rc;
}

static inline int _command(aip31068_t *dev, uint8_t value)
{
    int rc = _write(dev, value, true);

    xtimer_usleep(EXECUTION_TIME_DEFAULT);

    return rc;
}

static inline int _write(aip31068_t *dev, uint8_t data_byte, bool is_cmd)
{
    uint8_t control_byte = 0;
    if (!is_cmd) {
        control_byte |= (1 << BIT_CONTROL_BYTE_RS);
    }

    uint8_t data[] = { control_byte, data_byte };

    return _device_write(dev, data, sizeof(data));
}

static int _device_write(aip31068_t* dev, uint8_t *data, uint8_t len)
{
    i2c_t i2c_dev = dev->params.i2c_dev;

    if (i2c_acquire(i2c_dev) != 0) {
        return -AIP31068_ERROR_I2C;
    }

    int rc = i2c_write_bytes(i2c_dev, dev->params.i2c_addr, data, len, 0);

    i2c_release(i2c_dev);

    return rc;
}
