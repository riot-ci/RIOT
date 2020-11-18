/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Utility functions for non-atomic but volatile access
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 */

#ifndef VOLATILE_UTILS_H
#define VOLATILE_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Load an 8 bit value completely unoptimized
 * @param   var     Address to load the value from
 * @return  The value read unoptimized from address @p var
 */
static inline uint8_t volatile_load_u8(const volatile uint8_t *var)
{
    return *var;
}
/**
 * @brief   Load an 16 bit value completely unoptimized
 * @param   var     Address to load the value from
 * @return  The value read unoptimized from address @p var
 */
static inline uint16_t volatile_load_u16(const volatile uint16_t *var)
{
    return *var;
}
/**
 * @brief   Load an 32 bit value completely unoptimized
 * @param   var     Address to load the value from
 * @return  The value read unoptimized from address @p var
 */
static inline uint32_t volatile_load_u32(const volatile uint32_t *var)
{
    return *var;
}
/**
 * @brief   Load an 64 bit value completely unoptimized
 * @param   var     Address to load the value from
 * @return  The value read unoptimized from address @p var
 */
static inline uint64_t volatile_load_u64(const volatile uint64_t *var)
{
    return *var;
}

/**
 * @brief   Store an 8 bit value completely unoptimized
 * @param   dest    Address to write the given value unoptimized to
 * @param   val     Value to write unoptimized
 */
static inline void volatile_store_u8(volatile uint8_t *dest, uint8_t val)
{
    *dest = val;
/**
 * @brief   Store a 16 bit value completely unoptimized
 * @param   dest    Address to write the given value unoptimized to
 * @param   val     Value to write unoptimized
 */
}
static inline void volatile_store_u16(volatile uint16_t *dest, uint16_t val)
{
    *dest = val;
/**
 * @brief   Store a 32 bit value completely unoptimized
 * @param   dest    Address to write the given value unoptimized to
 * @param   val     Value to write unoptimized
 */
}
static inline void volatile_store_u32(volatile uint32_t *dest, uint32_t val)
{
    *dest = val;
}
/**
 * @brief   Store a 64 bit value completely unoptimized
 * @param   dest    Address to write the given value unoptimized to
 * @param   val     Value to write unoptimized
 */
static inline void volatile_store_u64(volatile uint64_t *dest, uint64_t val)
{
    *dest = val;
}

/**
 * @brief   Unoptimized version of `*dest += val`
 * @param   dest    Address of the value to add to
 * @param   val     Value to add
 */
static inline void volatile_fetch_add_u8(volatile uint8_t *dest, uint8_t val)
{
    *dest += val;
}
/**
 * @brief   Unoptimized version of `*dest -= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_sub_u8(volatile uint8_t *dest, uint8_t val)
{
    *dest -= val;
}
/**
 * @brief   Unoptimized version of `*dest |= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_or_u8(volatile uint8_t *dest, uint8_t val)
{
    *dest |= val;
}
/**
 * @brief   Unoptimized version of `*dest ^= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_xor_u8(volatile uint8_t *dest, uint8_t val)
{
    *dest ^= val;
}
/**
 * @brief   Unoptimized version of `*dest &= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_and_u8(volatile uint8_t *dest, uint8_t val)
{
    *dest &= val;
}

/**
 * @brief   Unoptimized version of `*dest += val`
 * @param   dest    Address of the value to add to
 * @param   val     Value to add
 */
static inline void volatile_fetch_add_u16(volatile uint16_t *dest, uint16_t val)
{
    *dest += val;
}
/**
 * @brief   Unoptimized version of `*dest -= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_sub_u16(volatile uint16_t *dest, uint16_t val)
{
    *dest -= val;
}
/**
 * @brief   Unoptimized version of `*dest |= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_or_u16(volatile uint16_t *dest, uint16_t val)
{
    *dest |= val;
}
/**
 * @brief   Unoptimized version of `*dest ^= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_xor_u16(volatile uint16_t *dest, uint16_t val)
{
    *dest ^= val;
}
/**
 * @brief   Unoptimized version of `*dest &= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_and_u16(volatile uint16_t *dest, uint16_t val)
{
    *dest &= val;
}

/**
 * @brief   Unoptimized version of `*dest += val`
 * @param   dest    Address of the value to add to
 * @param   val     Value to add
 */
static inline void volatile_fetch_add_u32(volatile uint32_t *dest, uint32_t val)
{
    *dest += val;
}
/**
 * @brief   Unoptimized version of `*dest -= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_sub_u32(volatile uint32_t *dest, uint32_t val)
{
    *dest -= val;
}
/**
 * @brief   Unoptimized version of `*dest |= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_or_u32(volatile uint32_t *dest, uint32_t val)
{
    *dest |= val;
}
/**
 * @brief   Unoptimized version of `*dest ^= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_xor_u32(volatile uint32_t *dest, uint32_t val)
{
    *dest ^= val;
}
/**
 * @brief   Unoptimized version of `*dest &= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_and_u32(volatile uint32_t *dest, uint32_t val)
{
    *dest &= val;
}

/**
 * @brief   Unoptimized version of `*dest += val`
 * @param   dest    Address of the value to add to
 * @param   val     Value to add
 */
static inline void volatile_fetch_add_u64(volatile uint64_t *dest, uint64_t val)
{
    *dest += val;
}
/**
 * @brief   Unoptimized version of `*dest -= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_sub_u64(volatile uint64_t *dest, uint64_t val)
{
    *dest -= val;
}
/**
 * @brief   Unoptimized version of `*dest |= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_or_u64(volatile uint64_t *dest, uint64_t val)
{
    *dest |= val;
}
/**
 * @brief   Unoptimized version of `*dest ^= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_xor_u64(volatile uint64_t *dest, uint64_t val)
{
    *dest ^= val;
}
/**
 * @brief   Unoptimized version of `*dest &= val`
 * @param   dest    Address of the value to apply the operation on
 * @param   val     Second operand
 */
static inline void volatile_fetch_and_u64(volatile uint64_t *dest, uint64_t val)
{
    *dest &= val;
}

#ifdef __cplusplus
}
#endif

#endif /* VOLATILE_UTILS_H */
/** @} */
