/*
 * Copyright (C) 2014-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup pkg_tlsf_malloc TLSF-based malloc.
 * @ingroup  pkg
 * @ingroup  sys
 *
 * @brief    TLSF-based global memory allocator.
 *
 * @file
 * @{
 *
 * @brief   TLSF-based global memory allocator.
 * @author  René Kijewski
 * @author  Juan I Carrano
 *
 * This is a malloc/free implementation built on top of the TLSF allocator.
 * It defines a global tlsf_control block and performs allocations on that
 * block.
 *
 * Additionally, the calls to TLSF are wrapped in irq_disable()/irq_restore(),
 * to make it thread-safe.
 *
 * By default, this implemetation replaces the system malloc. This behavior can
 * be changed by setting the TLSF_MALLOC_NOSYSTEM.
 *
 * If this module is used as the system memory allocator, then the global memory
 * control block should be initialized as the first thing before the stdlib is
 * used. Boards should use tlsf_add_pool() at startup to add all the memory
 * regions they want to make available for dynamic allocation via malloc().
 *
 */

#ifndef TLSF_MALLOC_H
#define TLSF_MALLOC_H

#include <stddef.h>
#include "tlsf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Add an area of memory to the global allocator pool.
 *
 * The first time this function is called, it will automatically perform a
 * tlsf_create() on the global tlsf_control block.
 *
 * @param   mem        Pointer to memory area. Should be aligned to 4 bytes.
 * @param   bytes      Size in bytes of the memory area.
 *
 * @return  0 on success, nonzero on failure.
 */
int tlsf_add_global_pool(void* mem, size_t bytes);

/**
 * Get a pointer to the global tlsf_control block.
 * 
 * Use for debugging purposes only.
 */
tlsf_t *_tlsf_get_global_control(void);

/**
 * Allocate a block of size "bytes"
 */
void *tlsf_gmalloc(size_t bytes);

/**
 * Allocate a block of size "bytes*count"
 */
void *tlsf_gcalloc(size_t count, size_t bytes);

/**
 * Allocate an aligned memory block.
 */
void *tlsf_gmemalign(size_t align, size_t bytes);

/**
 * Deallocate and reallocate with a different size.
 */
void *tlsf_grealloc(void *ptr, size_t size);

/**
 * Deallocate a block of data.
 */
void tlsf_gfree(void *ptr);


#ifdef __cplusplus
}
#endif

#endif /*TLSF_MALLOC_H*/
