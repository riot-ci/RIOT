/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm_common
 *
 * @{
 *
 * @file
 * @brief       Implementation of fast atomic utility functions
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ATOMIC_UTILS_CPU_H
#define ATOMIC_UTILS_CPU_H

#include "bit.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAS_ATOMIC_LOAD_U8
static inline uint8_t atomic_load_u8(const uint8_t *var)
{
    return __atomic_load_1(var, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_LOAD_U16
static inline uint16_t atomic_load_u16(const uint16_t *var)
{
    return __atomic_load_2(var, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_LOAD_U32
static inline uint32_t atomic_load_u32(const uint32_t *var)
{
    return __atomic_load_4(var, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_STORE_U8
static inline void atomic_store_u8(uint8_t *dest, uint8_t val)
{
    __atomic_store_1(dest, val, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_STORE_U16
static inline void atomic_store_u16(uint16_t *dest, uint16_t val)
{
    __atomic_store_2(dest, val, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_STORE_U32
static inline void atomic_store_u32(uint32_t *dest, uint32_t val)
{
    __atomic_store_4(dest, val, __ATOMIC_SEQ_CST);
}

#if CPU_HAS_BITBAND
#define ATOMIC_BITMASK __attribute__((section(".srambb")))

#define HAS_ATOMIC_SET_BIT_U8
static inline void atomic_set_bit_u8(uint8_t *mask, uint8_t bit)
{
    __asm__ volatile ("" ::: "memory");
    volatile uint32_t *bbaddr = bitband_addr(mask, bit);
    *bbaddr = 1;
    __asm__ volatile ("" ::: "memory");
}

#define HAS_ATOMIC_SET_BIT_U16
static inline void atomic_set_bit_u16(uint16_t *mask, uint8_t bit)
{
    __asm__ volatile ("" ::: "memory");
    volatile uint32_t *bbaddr = bitband_addr(mask, bit);
    *bbaddr = 1;
    __asm__ volatile ("" ::: "memory");
}

#define HAS_ATOMIC_SET_BIT_U32
static inline void atomic_set_bit_u32(uint32_t *mask, uint8_t bit)
{
    __asm__ volatile ("" ::: "memory");
    volatile uint32_t *bbaddr = bitband_addr(mask, bit);
    *bbaddr = 1;
    __asm__ volatile ("" ::: "memory");
}

#define HAS_ATOMIC_SET_BIT_U64
static inline void atomic_set_bit_u64(uint64_t *mask, uint8_t bit)
{
    __asm__ volatile ("" ::: "memory");
    volatile uint32_t *bbaddr = bitband_addr(mask, bit);
    *bbaddr = 1;
    __asm__ volatile ("" ::: "memory");
}

#define HAS_ATOMIC_CLEAR_BIT_U8
static inline void atomic_clear_bit_u8(uint8_t *mask, uint8_t bit)
{
    __asm__ volatile ("" ::: "memory");
    volatile uint32_t *bbaddr = bitband_addr(mask, bit);
    *bbaddr = 0;
    __asm__ volatile ("" ::: "memory");
}

#define HAS_ATOMIC_CLEAR_BIT_U16
static inline void atomic_clear_bit_u16(uint16_t *mask, uint8_t bit)
{
    __asm__ volatile ("" ::: "memory");
    volatile uint32_t *bbaddr = bitband_addr(mask, bit);
    *bbaddr = 0;
    __asm__ volatile ("" ::: "memory");
}

#define HAS_ATOMIC_CLEAR_BIT_U32
static inline void atomic_clear_bit_u32(uint32_t *mask, uint8_t bit)
{
    __asm__ volatile ("" ::: "memory");
    volatile uint32_t *bbaddr = bitband_addr(mask, bit);
    *bbaddr = 0;
    __asm__ volatile ("" ::: "memory");
}

#define HAS_ATOMIC_CLEAR_BIT_U64
static inline void atomic_clear_bit_u64(uint64_t *mask, uint8_t bit)
{
    __asm__ volatile ("" ::: "memory");
    volatile uint32_t *bbaddr = bitband_addr(mask, bit);
    *bbaddr = 0;
    __asm__ volatile ("" ::: "memory");
}

#endif /* CPU_HAS_BITBAND */

#ifdef __cplusplus
}
#endif

#endif /* ATOMIC_UTILS_CPU_H */
/** @} */
