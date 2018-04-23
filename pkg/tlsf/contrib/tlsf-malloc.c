/*
 * Copyright (C) 2014-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup  pkg_tlsf_malloc
 * @ingroup  pkg
 * @ingroup  sys
 * @file
 * @{
 *
 * @brief   TLSF-based global memory allocator.
 * @author  René Kijewski
 * @author  Juan I Carrano
 *
 */

#include <stdio.h>
#include <string.h>

#include "irq.h"
#include "tlsf.h"
#include "tlsf-malloc.h"

/**
 * Global memory heap (really a collection of pools, or areas)
 **/
static tlsf_t gheap = NULL;


/* Replace system memory routines
 *
 * Doing it this way (instead of changing the function names) has the
 * advantage of not breaking programs that use tlsf_g* just because someone
 * decides to use them as the default allocation routuines.
 * */
#ifndef TLSF_MALLOC_NOSYSTEM

#ifdef __GNUC__

#define ALIAS(n, args)  __attribute__ ((alias (#n)));

#else /* No GNU C -> no alias attribute */

#define ALIAS(n, args) { return n args; }
#define VALIAS(n, args) { n args; }

#endif /* __GNUC__ */

/**
 * Allocate dynamic memory via TLSF.
 */
void *malloc(size_t size) ALIAS(tlsf_gmalloc, (size))

/**
 * Allocate and clear memory via TLSF
 */
void *calloc(size_t count, size_t bytes) ALIAS(tlsf_gcalloc, (count, bytes))

/**
 * Allocate an aligned block via TLSF.
 */
void *memalign(size_t align, size_t bytes) ALIAS(tlsf_gmemalign, (align, bytes))

/**
 * Reallocate a block of memory via TLSF.
 */
void *realloc(void *ptr, size_t size) ALIAS(tlsf_grealloc, (ptr, size))

/**
 * Free dynamic memory allocated via TLSF.
 */
void free(void *ptr) VALIAS(tlsf_gfree, (ptr))

#endif /* TLSF_MALLOC_NOSYSTEM */

int tlsf_add_global_pool(void* mem, size_t bytes)
{
    if (gheap == NULL) {
        gheap = tlsf_create_with_pool(mem, bytes);
        return gheap == NULL;
    } else  {
        return tlsf_add_pool(gheap, mem, bytes) == NULL;
    }
}

tlsf_t *_tlsf_get_global_control(void)
{
    return gheap;
}

void *tlsf_gmalloc(size_t bytes)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_malloc(gheap, bytes);
    irq_restore(old_state);
    return result;
}

void *tlsf_gcalloc(size_t count, size_t bytes)
{
    void *result = tlsf_gmalloc(count * bytes);
    if (result) {
        memset(result, 0, count * bytes);
    }
    return result;
}

void *tlsf_gmemalign(size_t align, size_t bytes)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_memalign(gheap, align, bytes);
    irq_restore(old_state);
    return result;
}

void *tlsf_grealloc(void *ptr, size_t size)
{
    unsigned old_state = irq_disable();
    void *result = tlsf_realloc(gheap, ptr, size);
    irq_restore(old_state);
    return result;
}

void tlsf_gfree(void *ptr)
{
    unsigned old_state = irq_disable();
    tlsf_free(gheap, ptr);
    irq_restore(old_state);
}
