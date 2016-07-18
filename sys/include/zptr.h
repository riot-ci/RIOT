/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_util_zptr Pointer Compression
 * @ingroup     sys
 * @brief       Provides 32bit -> 16bit pointer compression
 *
 * On many platforms, some pointers may have to be aligned, e.g., to 4 byte
 * bounderies.
 * On 32bit platforms, that makes it possible to store all possible aligned
 * 32bit pointers in a 16bit value as long as the total memory is small (e.g.,
 * with 4 byte alignment, all pointers within 256kb RAM can be represented by a
 * 16bit value). This can save memory, at the cost of some instructions for
 * compression/decompression.
 *
 * In order to use pointer compression, ZPTR_BASE needs to be defined to a (4
 * byte aligned) base address.
 *
 * If ZPTR_BASE is unset, @ref zptr_t / @ref zptrc() / @ref zptrd() will
 * transparently and without overhead compile to normal (uncompressed) pointer
 * operations.
 *
 * @{
 *
 * @file
 * @brief       32bit -> 16bit pointer compression implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ZPTR_H_
#define ZPTR_H_

#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#if ZPTR_BASE || defined(DOXYGEN)

/**
 * @name zptr type definition
 */
typedef uint16_t zptr_t;

/**
 * @name zptr printf format definition
 */
#define PRIzptr PRIu16

/**
 * @brief Compress a pointer (if possible)
 *
 * Substracts ZPTR_BASE, then right-shifts @p pointer by two.
 *
 * @param[in]   pointer     pointer to compress
 * @returns     compressed pointer
 */
static inline zptr_t zptrc(void *pointer)
{
    assert(!(pointer & 0x3));
    return (uint16_t)(((uint32_t)pointer - (uint32_t)ZPTR_BASE) >> 2);
}

/**
 * @brief Decompress a pointer
 *
 * Left-shifts zptr_t by two, then adds ZPTR_BASE.
 *
 * @param[in]   zptr    compressed pointer
 * @returns     decompressed pointer
 */
static inline void *zptrd(zptr_t zptr)
{
    return (void *)(ZPTR_BASE + ((uint32_t)zptr << 2));
}

#else /* ZPTR_BASE */
/* fallback implementation */
typedef void *zptr_t;
#define PRIzptr "p"
static inline zptr_t zptrc(void *pointer) { return (zptr_t)pointer; }
static inline void *zptrd(zptr_t zptr) { return (void *)zptr; }
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ZPTR_H_ */
