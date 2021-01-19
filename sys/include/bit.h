/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Bit access macros with bit-banding support for Cortex-M based CPUs
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef BIT_H
#define BIT_H

#include <stdint.h>
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#if DOXYGEN
/** @brief Flag for telling if the CPU has hardware bit band support */
#define CPU_HAS_BITBAND 1 || 0 (1 if CPU implements bit-banding, 0 if not)
#endif

#if CPU_HAS_BITBAND || DOXYGEN
/* Some MCUs provide a bitband address space for atomically accessing
 * single bits of peripheral registers, and sometimes for RAM as well */
/**
 * @name Bit manipulation functions
 * @{
 */
/* Generic bit band conversion routine */
/**
 * @brief Convert bit band region address and bit number to bit band alias address
 *
 * @param[in] ptr  base address in non bit banded memory
 * @param[in] bit  bit number within the word
 *
 * @return Address of the bit within the bit band memory region
 */
static inline volatile void *bitband_addr(volatile void *ptr, uintptr_t bit)
{
    return (volatile void *)((((uintptr_t)ptr) & 0xF0000000ul) + 0x2000000ul +
        ((((uintptr_t)ptr) & 0xFFFFFul) << 5) + (bit << 2));
}

/**
 * @brief Set a single bit in the 32 bit word pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr |= (1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target word
 * @param[in]  bit bit number within the word
 */
static inline void bit_set32(volatile uint32_t *ptr, uint8_t bit)
{
    *((volatile uint32_t *)bitband_addr(ptr, bit)) = 1;
}

/**
 * @brief Set a single bit in the 16 bit word pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr |= (1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target word
 * @param[in]  bit bit number within the word
 */
static inline void bit_set16(volatile uint16_t *ptr, uint8_t bit)
{
    *((volatile uint16_t *)bitband_addr(ptr, bit)) = 1;
}

/**
 * @brief Set a single bit in the 8 bit byte pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr |= (1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target byte
 * @param[in]  bit bit number within the byte
 */
static inline void bit_set8(volatile uint8_t *ptr, uint8_t bit)
{
    *((volatile uint8_t *)bitband_addr(ptr, bit)) = 1;
}

/**
 * @brief Clear a single bit in the 32 bit word pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr &= ~(1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target word
 * @param[in]  bit bit number within the word
 */
static inline void bit_clear32(volatile uint32_t *ptr, uint8_t bit)
{
    *((volatile uint32_t *)bitband_addr(ptr, bit)) = 0;
}

/**
 * @brief Clear a single bit in the 16 bit word pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr &= ~(1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target word
 * @param[in]  bit bit number within the word
 */
static inline void bit_clear16(volatile uint16_t *ptr, uint8_t bit)
{
    *((volatile uint16_t *)bitband_addr(ptr, bit)) = 0;
}

/**
 * @brief Clear a single bit in the 8 bit byte pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr &= ~(1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target byte
 * @param[in]  bit bit number within the byte
 */
static inline void bit_clear8(volatile uint8_t *ptr, uint8_t bit)
{
    *((volatile uint8_t *)bitband_addr(ptr, bit)) = 0;
}

/** @} */

#else /* CPU_HAS_BITBAND */
/* CPU does not have bitbanding, fall back to plain C */
static inline void bit_set32(volatile uint32_t *ptr, uint8_t bit)
{
    *ptr |= (1 << (bit));
}

static inline void bit_set16(volatile uint16_t *ptr, uint8_t bit)
{
    *ptr |= (1 << (bit));
}

static inline void bit_set8(volatile uint8_t *ptr, uint8_t bit)
{
    *ptr |= (1 << (bit));
}

static inline void bit_clear32(volatile uint32_t *ptr, uint8_t bit)
{
    *ptr &= ~(1 << (bit));
}

static inline void bit_clear16(volatile uint16_t *ptr, uint8_t bit)
{
    *ptr &= ~(1 << (bit));
}

static inline void bit_clear8(volatile uint8_t *ptr, uint8_t bit)
{
    *ptr &= ~(1 << (bit));
}

#endif /* CPU_HAS_BITBAND */

#ifdef __cplusplus
}
#endif

#endif /* BIT_H */
/** @} */
