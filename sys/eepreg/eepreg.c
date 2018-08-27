/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_eepreg
 * @{
 *
 * @file
 * @brief   eepreg implementation
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "eepreg.h"
#include "irq.h"
#include "periph/eeprom.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* EEPREG magic number */
static const char eepreg_magic[] = "RIOTREG";

/* constant lengths */
#define MAGIC_SIZE      (sizeof(eepreg_magic))

/* constant locations */
#define REG_START        (EEPROM_RESERV_CPU_LOW + EEPROM_RESERV_BOARD_LOW)
#define REG_MAGIC_LOC    (REG_START)
#define REG_END_LOC      (REG_MAGIC_LOC + MAGIC_SIZE)
#define REG_ENT1_LOC     (REG_END_LOC + EEPREG_LOC_LEN)
#define DAT_START        (EEPROM_SIZE - EEPROM_RESERV_CPU_HI \
                          - EEPROM_RESERV_BOARD_HI - 1)

static inline uint32_t _read_meta_uint(uint32_t loc)
{
    uint8_t data[4];
    uint32_t ret;

    eeprom_read(loc, data, EEPREG_LOC_LEN);

    /* unused array members will be discarded */
    ret = ((uint32_t)data[0] << 24)
          | ((uint32_t)data[1] << 16)
          | ((uint32_t)data[2] << 8)
          | ((uint32_t)data[3]);

    /* bit shift to discard unused array members */
    ret >>= 8 * (4 - EEPREG_LOC_LEN);

    return ret;
}

static inline void _write_meta_uint(uint32_t loc, uint32_t val)
{
    uint8_t data[4];

    val <<= 8 * (4 - EEPREG_LOC_LEN);

    data[0] = (uint8_t)(val >> 24);
    data[1] = (uint8_t)(val >> 16);
    data[2] = (uint8_t)(val >> 8);
    data[3] = (uint8_t)val;

    eeprom_write(loc, data, EEPREG_LOC_LEN);
}

static inline uint32_t _get_reg_end(void)
{
    return _read_meta_uint(REG_END_LOC);
}

static inline void _set_reg_end(uint32_t loc)
{
    _write_meta_uint(REG_END_LOC, loc);
}

static inline uint32_t _get_free_loc(void)
{
    /* free location is stored at the end of the registry */
    return _read_meta_uint(_get_reg_end() - EEPREG_LOC_LEN);
}

static inline void _set_free_loc(uint32_t loc)
{
    /* free location is stored at the end of the registry */
    _write_meta_uint(_get_reg_end() - EEPREG_LOC_LEN, loc);
}

static inline uint32_t _get_free_space(void)
{
    return _get_free_loc() - _get_reg_end() + 1;
}

static inline uint32_t _get_data_loc(uint32_t meta_loc)
{
    /* data location is at the start of meta-data */
    return _read_meta_uint(meta_loc);
}

static inline void _set_data_loc(uint32_t meta_loc, uint32_t loc)
{
    /* data location is at the start of meta-data */
    _write_meta_uint(meta_loc, loc);
}

static inline uint8_t _get_name_len(uint32_t meta_loc)
{
    meta_loc += EEPREG_LOC_LEN;

    for (uint8_t offset = 0; offset < (uint8_t)UINT_MAX; offset++) {
        uint8_t byte = eeprom_read_byte(meta_loc + offset);

        if (byte == '\0') {
            return offset + 1;
        }
    }

    return (uint8_t)UINT_MAX;
}

static inline void _get_name(uint32_t meta_loc, char *name, uint8_t len)
{
    meta_loc += EEPREG_LOC_LEN;

    for (uint8_t offset = 0; offset < len; offset++) {
        name[offset] = eeprom_read_byte(meta_loc + offset);

        if (name[offset] == '\0') {
            return;
        }
    }
}

static inline uint32_t _get_prev_meta_loc(uint32_t meta_loc)
{
    /* minimum entry size */
    meta_loc -= EEPREG_LOC_LEN + 2;

    for (uint32_t offset = 0; meta_loc - offset > REG_ENT1_LOC; offset++) {
        uint8_t byte = eeprom_read_byte(meta_loc - offset);

        if (byte == '\0') {
            return meta_loc - offset + 1;
        }
    }

    return REG_ENT1_LOC;
}

static inline uint32_t _get_next_meta_loc(uint32_t meta_loc)
{
    /* Note: returns where next entry would be, even if non-existent */
    uint8_t name_len = _get_name_len(meta_loc);

    if (name_len == (uint8_t)UINT_MAX) {
        return _get_reg_end() - EEPREG_LOC_LEN;
    }

    return meta_loc + EEPREG_LOC_LEN + name_len;
}

static inline uint32_t _get_meta_loc(const char *name)
{
    uint32_t meta_loc = REG_ENT1_LOC;
    uint32_t reg_end = _get_reg_end();
    uint8_t len = (uint8_t)strlen(name) + 1;
    char data[len + 1];

    /* make sure string is always terminated */
    data[len] = '\0';

    while (meta_loc < reg_end - EEPREG_LOC_LEN) {
        _get_name(meta_loc, data, len);

        /* check for match */
        if (strcmp(data, name) == 0) {
            return meta_loc;
        }

        meta_loc = _get_next_meta_loc(meta_loc);
    }

    /* no meta-data found */
    return (uint32_t)UINT_MAX;
}

static inline uint32_t _get_data_len(uint32_t meta_loc)
{
    uint32_t loc = _get_data_loc(meta_loc);

    uint32_t prev_loc;
    if (meta_loc == REG_ENT1_LOC) {
        prev_loc = DAT_START;
    }
    else {
        /* previous entry data location is end of name's data */
        meta_loc = _get_prev_meta_loc(meta_loc);
        prev_loc = _get_data_loc(meta_loc);
    }

    return prev_loc - loc;
}

static inline int _new_entry(const char *name, uint32_t len)
{
    uint8_t name_len = (uint8_t)strlen(name) + 1;

    /* check to see if there is enough room */
    if (_get_free_space() < EEPREG_LOC_LEN + name_len + len) {
        return -ENOSPC;
    }

    /* don't allow interrupts when editing registry */
    unsigned int irq_state = irq_disable();

    uint32_t meta_loc = _get_reg_end() - EEPREG_LOC_LEN;
    uint32_t free_loc = _get_free_loc();

    /* set the location of the data */
    _set_data_loc(meta_loc, free_loc - len);

    /* write name of entry */
    eeprom_write(meta_loc + EEPREG_LOC_LEN, (uint8_t *)name, name_len);

    /* update end of the registry */
    _set_reg_end(meta_loc + EEPREG_LOC_LEN + name_len + EEPREG_LOC_LEN);

    /* update beginning of free space location */
    _set_free_loc(free_loc - len);

    irq_restore(irq_state);

    return 0;
}

static inline void _move_data(uint32_t oldpos, uint32_t newpos, uint32_t len)
{
    for (uint32_t count = 0; count < len; count++) {
        uint32_t offset;

        if (newpos < oldpos) {
            /* move from beginning of data */
            offset = count;
        }
        else {
            /* move from end of data */
            offset = len - count;
        }

        uint8_t byte = eeprom_read_byte(oldpos + offset);

        eeprom_write_byte(newpos + offset, byte);
    }
}

int eepreg_add(uint32_t *pos, const char *name, uint32_t len)
{
    uint32_t meta_loc;

    int ret = eepreg_check();
    if (ret == -ENOENT) {
        /* reg does not exist, so make a new one */
        eepreg_reset();
    }
    else if (ret < 0) {
        DEBUG("[eepreg_add] eepreg_check failed\n");
        return ret;
    }

    meta_loc = _get_meta_loc(name);

    if (meta_loc == (uint32_t)UINT_MAX) {
        /* entry does not exist, so make a new one */

        /* location of the new data */
        *pos = _get_free_loc() - len;

        if (_new_entry(name, len) < 0) {
            DEBUG("[eepreg_add] not enough space for %s\n", name);
            return -ENOSPC;
        }

        return 0;
    }

    *pos = _get_data_loc(meta_loc);

    if (len != _get_data_len(meta_loc)) {
        DEBUG("[eepreg_add] %s already exists with different length\n", name);
        return -EADDRINUSE;
    }

    return 0;
}

int eepreg_read(uint32_t *pos, const char *name)
{
    uint32_t meta_loc;

    int ret = eepreg_check();
    if (ret < 0) {
        DEBUG("[eepreg_read] eepreg_check failed\n");
        return ret;
    }

    meta_loc = _get_meta_loc(name);

    if (meta_loc == (uint32_t)UINT_MAX) {
        DEBUG("[eepreg_read] no entry for %s\n", name);
        return -ENOENT;
    }

    *pos = _get_data_loc(meta_loc);

    return 0;
}

int eepreg_write(uint32_t *pos, const char *name, uint32_t len)
{
    int ret = eepreg_check();
    if (ret == -ENOENT) {
        /* reg does not exist, so make a new one */
        eepreg_reset();
    }
    else if (ret < 0) {
        DEBUG("[eepreg_write] eepreg_check failed\n");
        return ret;
    }

    /* location of the new data */
    *pos = _get_free_loc() - len;

    if (_new_entry(name, len) < 0) {
        DEBUG("[eepreg_write] not enough space for %s\n", name);
        return -ENOSPC;
    }

    return 0;
}

int eepreg_rm(const char *name)
{
    uint32_t meta_loc, next_meta, meta_len,
             len, loc, new_loc, tot_len,
             new_reg_end, new_free_loc;

    int ret = eepreg_check();
    if (ret < 0) {
        DEBUG("[eepreg_rm] eepreg_check failed\n");
        return ret;
    }

    meta_loc = _get_meta_loc(name);

    if (meta_loc == (uint32_t)UINT_MAX) {
        DEBUG("[eepreg_rm] no entry for %s\n", name);
        return -ENOENT;
    }

    next_meta = _get_next_meta_loc(meta_loc);
    meta_len = _get_reg_end() - next_meta;

    len = _get_data_len(meta_loc);
    loc = _get_data_loc(meta_loc);
    new_loc = loc + len;
    tot_len = loc - _get_free_loc();

    new_reg_end = meta_loc + meta_len;
    new_free_loc = new_loc - tot_len;

    /* don't allow interrupts when editing registry */
    unsigned int irq_state = irq_disable();

    _move_data(next_meta, meta_loc, meta_len);
    _move_data(loc - tot_len, new_free_loc, tot_len);

    _set_reg_end(new_reg_end);
    _set_free_loc(new_free_loc);

    while (meta_loc < new_reg_end - EEPREG_LOC_LEN) {
        _set_data_loc(meta_loc, _get_data_loc(meta_loc) + len);
        meta_loc = _get_next_meta_loc(meta_loc);
    }

    irq_restore(irq_state);

    return 0;
}

int eepreg_iter(eepreg_iter_cb_t cb, void *arg)
{
    uint32_t meta_loc = REG_ENT1_LOC;
    uint32_t reg_end = _get_reg_end();

    int ret = eepreg_check();
    if (ret < 0) {
        DEBUG("[eepreg_len] eepreg_check failed\n");
        return ret;
    }

    while (meta_loc < reg_end - EEPREG_LOC_LEN) {
        /* size of memory allocation */
        uint8_t name_len = _get_name_len(meta_loc);

        char name[name_len];

        _get_name(meta_loc, name, name_len);

        /* execute callback */
        ret = cb(name, arg);

        if (ret < 0) {
            DEBUG("[eepreg_iter] callback reports failure\n");
            return ret;
        }

        meta_loc = _get_next_meta_loc(meta_loc);
    }

    return 0;
}

int eepreg_check(void)
{
    char magic[MAGIC_SIZE + 1];

    /* make sure string is always terminated */
    magic[MAGIC_SIZE] = '\0';

    /* get magic number from EEPROM */
    if (eeprom_read(REG_MAGIC_LOC, (uint8_t *)magic, MAGIC_SIZE)
        != MAGIC_SIZE) {

        DEBUG("[eepreg_check] EEPROM read error\n");
        return -EIO;
    }

    /* check to see if magic number is the same */
    if (strcmp(magic, eepreg_magic) != 0) {
        DEBUG("[eepreg_check] No registry detected\n");
        return -ENOENT;
    }

    return 0;
}

int eepreg_reset(void)
{
    /* don't allow interrupts when editing registry */
    unsigned int irq_state = irq_disable();

    /* write new registry magic number */
    if (eeprom_write(REG_MAGIC_LOC, (uint8_t *)eepreg_magic, MAGIC_SIZE)
        != MAGIC_SIZE) {

        DEBUG("[eepreg_reset] EEPROM write error\n");
        irq_restore(irq_state);
        return -EIO;
    }

    /* new registry has no entries */
    _set_reg_end(REG_ENT1_LOC + EEPREG_LOC_LEN);

    /* new registry has no corresponding data */
    _set_free_loc(DAT_START);

    irq_restore(irq_state);

    return 0;
}

int eepreg_len(uint32_t *len, const char *name)
{
    uint32_t meta_loc;

    int ret = eepreg_check();
    if (ret < 0) {
        DEBUG("[eepreg_len] eepreg_check failed\n");
        return ret;
    }

    meta_loc = _get_meta_loc(name);

    if (meta_loc == (uint32_t)UINT_MAX) {
        DEBUG("[eepreg_len] no entry for %s\n", name);
        return -ENOENT;
    }

    *len = _get_data_len(meta_loc);

    return 0;
}

int eepreg_free(uint32_t *len)
{
    int ret = eepreg_check();
    if (ret < 0) {
        DEBUG("[eepreg_free] eepreg_check failed\n");
        return ret;
    }

    *len = _get_free_space();

    return 0;
}
