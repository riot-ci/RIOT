/*
 * Copyright (C) 2017 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_memarray memory array allocator
 * @ingroup     sys
 * @brief       memory array allocator
 * @{
 *
 * @brief       pseudo dynamic allocation in static memory arrays
 * @author      Tobias Heider <heidert@nm.ifi.lmu.de>
 */

#ifndef MEMARRAY_H
#define MEMARRAY_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Memory pool
 */
typedef struct {
    void *first_free; /**< head of the free list */
} memarray_t;

/**
 * @brief Define static memory pool
 *
 * @param[out] name    name for defined memarray pool
 * @param[in]  size    size of single memory chunk
 * @param[in]  num     number of chunks in pool
 */
#define MEMARRAY(name, size, num)                                         \
    static uint8_t _data_ ## name[num * size];                            \
    static memarray_t name = { .first_free = _data_ ## name };

/**
 * @brief Initialize memarray pool with free list
 *
 * @param[out]  mem     memarray pool to initialize
 * @param[in]   size    size of single memory chunk
 * @param[in]   num     number of chunks in pool
 */
void memarray_init(memarray_t *mem, size_t num, size_t size);

/**
 * @brief Allocate memory chunk in memarray pool
 *
 * @param[in,out]  mem      memarray pool to allocate block in
 *
 * @return NULL on failure
 */
void *memarray_alloc(memarray_t *mem);

/**
 * @brief Free memory chunk in memarray pool
 *
 * @param[in,out]  mem      memarray pool to free block in
 * @param[in]      ptr      pointer to memarray chunk
 */
void memarray_free(memarray_t *mem, void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* MEMARRAY_H */

/**
 * @}
 */
