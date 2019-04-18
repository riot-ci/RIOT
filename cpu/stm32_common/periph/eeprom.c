/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_eeprom
 * @{
 *
 * @file
 * @brief       Low-level eeprom driver implementation
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Oleg Artamonov <oleg@unwds.com>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "periph/eeprom.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

extern void _lock(void);
extern void _unlock(void);
extern void _wait_for_pending_operations(void);

#ifndef EEPROM_START_ADDR
#error "periph/eeprom: EEPROM_START_ADDR is not defined"
#endif

#define ALIGNMENT_MASK      (0x00000003)

static void _erase_word(uint32_t addr)
{
    assert(addr <= EEPROM_SIZE + EEPROM_START_ADDR);

    /* Wait for last operation to be completed */
    _wait_for_pending_operations();

    /* Write "00000000h" to valid address in the data memory" */
    *(__IO uint32_t *)addr = 0x00000000;
}

static void _write_word(uint32_t addr, uint32_t data)
{
    assert(addr <= EEPROM_SIZE + EEPROM_START_ADDR);

    /* Wait for last operation to be completed */
    _wait_for_pending_operations();

    *(__IO uint32_t *)addr = data;
}

static void _write_byte(uint32_t addr, uint8_t data)
{
    uint32_t tmp = 0;
    uint32_t data_mask = 0;

    /* Check the parameters */
    assert(addr <= EEPROM_SIZE + EEPROM_START_ADDR);

    /* Wait for last operation to be completed */
    _wait_for_pending_operations();

    /* stm32l1xxx cat 1, 2 &3 can't write NULL bytes RefManual p78*/
    if (data != (uint8_t)0x00) {
        *(__IO uint8_t *)addr = data;
    }
    else {
        tmp = *(__IO uint32_t *)(addr & (~ALIGNMENT_MASK));
        data_mask = 0xFF << ((uint32_t)(0x8 * (addr & ALIGNMENT_MASK)));
        tmp &= ~data_mask;
        _erase_word(addr & (~ALIGNMENT_MASK));
        _write_word((addr & (~ALIGNMENT_MASK)), tmp);
    }
}

size_t eeprom_read(uint32_t pos, uint8_t *data, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    uint8_t *p = data;

    _unlock();

    DEBUG("Reading data from EEPROM at pos %" PRIu32 ": ", pos);
    for (size_t i = 0; i < len; i++) {
        _wait_for_pending_operations();
        *p++ = *(__IO uint8_t *)(EEPROM_START_ADDR + pos++);
        DEBUG("0x%02X ", *p);
    }
    DEBUG("\n");

    _lock();

    return len;
}

size_t eeprom_write(uint32_t pos, const uint8_t *data, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    uint8_t *p = (uint8_t *)data;

    _unlock();

    for (size_t i = 0; i < len; i++) {
        _wait_for_pending_operations();
        _write_byte((EEPROM_START_ADDR + pos++), *p++);
    }

    _lock();

    return len;
}
