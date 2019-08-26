/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at24cxxx
 * @{
 *
 * @file
 * @brief       Implementation of expected EEPROM interface, expected by EEPROM registry
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include "at24cxxx.h"
#include "log.h"

/**
 * @brief Wrapper around @see at24cxxx_read_byte that uses
 * the first parameters in the at24cxxx_params array
 *
 * This allows the use of EEPROM registry.
 *
 * @param[in] pos      Position in EEPROM memory
 *
 * @return    Read byte
 */
uint8_t eeprom_read_byte(uint32_t pos)
{
    int32_t r =  at24cxxx_read_byte(
        at24cxxx_get_default_dev(), pos);

    if (r < 0) {
        LOG_ERROR("[AT24CXXX] at24cxxx_read_byte(): %" PRId32 "\n", r);
        r = 0;
    }
    return (uint8_t)r;
}

/**
 * @brief Wrapper around @see at24cxxx_read that uses
 * the first parameters in the at24cxxx_params array
 *
 * This allows the use of EEPROM registry.
 *
 * @param[in] pos       Position in EEPROM memory
 * @param[out] data     Read buffer
 * @param[in] len       Requested length to be read
 *
 * @return    @p len on success
 * @return    0 on failure
 */
size_t eeprom_read(uint32_t pos, void *data, size_t len)
{
    int32_t r = at24cxxx_read(
        at24cxxx_get_default_dev(), pos, data, len);

    if (r < 0) {
        LOG_ERROR("[AT24CXXX] at24cxxx_read(): %" PRId32 "\n", r);
        r = 0;
    }
    return (size_t)r;
}

/**
 * @brief Wrapper around @see at24cxxx_write_byte that uses
 * the first parameters in the at24cxxx_params array
 *
 * This allows the use of EEPROM registry.
 *
 * @param[in] pos      Position in EEPROM memory
 * @param[in] data     Value to be written
 */
void eeprom_write_byte(uint32_t pos,  uint8_t data)
{
    int32_t w = at24cxxx_write_byte(
        at24cxxx_get_default_dev(), pos, data);

    if (w < 0) {
        LOG_ERROR("[AT24CXXX] at24cxxx_write_byte(): %" PRId32 "\n", w);
    }
}

/**
 * @brief Wrapper around @see at24cxxx_write that uses
 * the first parameters in the at24cxxx_params array
 *
 * This allows the use of EEPROM registry.
 *
 * @param[in] pos       Position in EEPROM memory
 * @param[in] data      Write buffer
 * @param[in] len       Requested length to be written
 *
 * @return    Number of bytes that have been written
 * @return    0 on failure
 */
size_t eeprom_write(uint32_t pos, const void *data, size_t len)
{
    int32_t w = at24cxxx_write(
        at24cxxx_get_default_dev(), pos, data, len);

    if (w < 0) {
        LOG_ERROR("[AT24CXXX] at24cxxx_write(): %" PRId32 "\n", w);
        w = 0;
    }
    return (size_t)w;
}

/**
 * @brief Wrapper around @see at24cxxx_set that uses
 * the first parameters in the at24cxxx_params array
 *
 * This allows the use of EEPROM registry.
 *
 * @param[in] pos       Position in EEPROM memory
 * @param[in] val       Value to be set
 * @param[in] len       Requested length to be written
 *
 * @return    Number of bytes that have been set
 * @return    0 on failure
 */
size_t eeprom_set(uint32_t pos, uint8_t val, size_t len)
{
    int32_t w = at24cxxx_set(
        at24cxxx_get_default_dev(), pos, val, len);

    if (w < 0) {
        LOG_ERROR("[AT24CXXX] at24cxxx_set(): %" PRId32 "\n", w);
        w = 0;
    }
    return (size_t)w;
}

/**
 * @brief Wrapper around @see at24cxxx_clear that uses
 * the first parameters in the at24cxxx_params array
 *
 * This allows the use of EEPROM registry.
 *
 * @param[in] pos       Position in EEPROM memory
 * @param[in] len       Requested length to be written
 *
 * @return    Number of bytes that have been cleared
 */
size_t eeprom_clear(uint32_t pos, size_t len)
{
    int32_t w =  at24cxxx_clear(
        at24cxxx_get_default_dev(), pos, len);

    if (w < 0) {
        LOG_ERROR("[AT24CXXX] at24cxxx_clear(): %" PRId32 "\n", w);
        w = 0;
    }
    return (size_t)w;
}

/**
 * @brief Wrapper around @see at24cxxx_erase that uses
 * the first parameters in the at24cxxx_params array
 *
 * This allows the use of EEPROM registry.
 *
 * @return    Number of bytes that have been erased
 */
size_t eeprom_erase(void)
{
    int32_t w = at24cxxx_erase(
        at24cxxx_get_default_dev());

    if (w < 0) {
        LOG_ERROR("[AT24CXXX] at24cxxx_erase(): %" PRId32 "\n", w);
        w = 0;
    }
    return (size_t)w;
}
