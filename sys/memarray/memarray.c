/*
 * Copyright (C) 2017 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include "memarray.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

void *memarray_alloc(memarray_t *mem)
{
    if (mem->first_free == NULL) {
        return NULL;
    }
    void *free = mem->first_free;
    mem->first_free = *((void **)mem->first_free);
    DEBUG("memarray: Allocate %zu Bytes at %p\n", mem->size, free);
    return free;
}

void memarray_init(memarray_t *mem)
{
    DEBUG("memarray: Initialize memarray of %zu times %zu Bytes at %p\n",
          mem->num,
          mem->size,
          mem->first_free);
    for (size_t i = 0; i < (mem->num - 1); i++) {
        void *next = ((char *)mem->first_free) + ((i + 1) * mem->size);
        memcpy(((char *)mem->first_free) + (i * mem->size), &next, sizeof(void *));
    }
}

void memarray_free(memarray_t *mem, void *ptr)
{
    memcpy(ptr, &mem->first_free, sizeof(void *));
    mem->first_free = ptr;
    DEBUG("memarray: Free %zu Bytes at %p\n",mem->size, ptr);
}
