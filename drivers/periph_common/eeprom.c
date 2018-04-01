/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers
 * @{
 *
 * @file
 * @brief       Common eeprom functions implmentation
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>
#include "cpu.h"
#include "assert.h"

/* guard this file, must be done before including periph/eeprom.h */
#if defined(EEPROM_SIZE) && defined(EEPROM_START_ADDR)

#include "periph/eeprom.h"

void eeprom_read(uint32_t pos, void *data, uint8_t len)
{
    assert(pos + len < EEPROM_SIZE);

    while(len--) {
        *(uint8_t *)data++ = eeprom_read_byte(pos++);
    }
}

void eeprom_write(uint32_t pos, const void *data, uint8_t len)
{
    assert(pos + len < EEPROM_SIZE);

    while(len--) {
        eeprom_write_byte(pos++, *(uint8_t *)data++);
    }
}

#endif
