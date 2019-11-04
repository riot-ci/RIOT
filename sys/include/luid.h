/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_luid Locally Unique ID Generator
 * @ingroup     sys
 * @brief       Generate system wide (locally) unique IDs
 *
 * This module generates system wide, variable length unique IDs based on on the
 * cpuid module. If the cpuid module is not present, the module falls back to a
 * pre-defined, constant seed for creating unique IDs.
 *
 * The main purpose of this module is to have a unified way for e.g. creating
 * hardware addresses and similar.
 *
 * The IDs generated by this module are base created from a constant base part
 * and a dynamic portion, which is XORed into the base ID. If the cpuid module
 * is present, the base ID is created from the CPU's unique ID. If not, the
 * base ID is created by simple 'memsetting' the base ID with the defined
 * backup seed value.
 *
 * Once the base ID is generated, a LUID is generated by (i) XORing a counter
 * value with the LSB of the base ID, or (ii) by XORing the least significant
 * byes with a value given by the user.
 *
 * Example: Calling `luid_base(&buf, 8)` will always yield an identical  value,
 * independent how often the function is called. But calling
 * `luid_base(&buf, 2)` afterwards will results in a different value, if the
 * cpuid module is present, and in the same (but shorter) value if not.
 *
 * Example: Calling `luid_get(&buf, 8)` four times in a row, will yield four
 * different IDs, differing in their LSB.
 *
 * Example: Calling `luid_custom(&buf, 8, 123)` will always yield the same
 * value, but calling `luid_custom(&buf, 8, 124)` will differ.
 *
 * @note        This module generates locally unique IDs without any guarantees
 *              on their structure. These LUIDs are not compatible nor conform
 *              to UUIDs as defined in RFC4122.
 *
 * @{
 * @file
 * @brief       Header of the unique ID generation module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LUID_H
#define LUID_H

#include <stddef.h>

#include "net/eui48.h"
#include "net/eui64.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Value used as based for the LUIDs in case the cpuid module is not
 *          present
 */
#ifndef LUID_BACKUP_SEED
#define LUID_BACKUP_SEED        0x23
#endif

/**
 * @brief   Get a unique ID
 *
 * The resulting ID is built from the base ID generated with luid_base(), which
 * isXORed with an 8-bit incrementing counter value into the most significant
 * byte.
 *
 * @note    The resulting LUID will repeat after 255 calls.
 *
 * @param[out] buf      memory location to copy the LUID into. MUST be able to
 *                      hold at least @p len bytes
 * @param[in]  len      length of the LUID in bytes
 */
void luid_get(void *buf, size_t len);

/**
 * @brief   Get a unique short unicast address
 *
 * The resulting address is built from the base ID generated with luid_base(), which
 * isXORed with an 8-bit incrementing counter value into the least significant
 * byte.
 *
 * @note    The resulting address will repeat after 255 calls.
 *
 * @param[out] addr     memory location to copy the address into.
 */
void luid_get_short(network_uint16_t *addr);

/**
 * @brief   Get a unique EUI48 address
 *
 * The resulting address is built from the base ID generated with luid_base(), which
 * isXORed with an 8-bit incrementing counter value into the least significant byte.
 *
 * @note    The resulting address will repeat after 255 calls.
 *
 * @param[out] addr     memory location to copy the address into.
 */
void luid_get_eui48(eui48_t *addr);

/**
 * @brief   Get a unique EUI64 address
 *
 * The resulting address is built from the base ID generated with luid_base(), which
 * isXORed with an 8-bit incrementing counter value into the least significant byte.
 *
 * @note    The resulting address will repeat after 255 calls.
 *
 * @param[out] addr     memory location to copy the address into.
 */
void luid_get_eui64(eui64_t *addr);

/**
 * @brief   Get a custom unique ID based on a user given generator value
 *
 * The resulting ID is built from the base ID XORed with @p gen in the base
 * ID's most significant bytes.
 *
 * @note    Calling this function with identical values for @p gen and @p len
 *          will always result in identical LUIDs.
 *
 * @param[out] buf      memory location to copy the LUID into. MUST be able to
 *                      hold at least @p len bytes
 * @param[in]  len      length of the LUID in bytes
 * @param[in]  gen      custom LUID generator value
 */
void luid_custom(void *buf, size_t len, int gen);

/**
 * @brief   Get a LUID base value
 *
 * The luid module creates the value dependent on the given @p len value using
 * the cpuid module if present or a static seed value (@ref LUID_BACKUP_SEED) if
 * not.
 *
 * @param[out] buf      memory location to copy the LUID into. MUST be able to
 *                      hold at least @p len bytes
 * @param[in]  len      length of the LUID in bytes
 */
void luid_base(void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* LUID_H */
/** @} */
