/**
 * Native CPU EUI provider
 *
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup cpu_native
 * @{
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <assert.h>
#include <stdlib.h>

#include "net/l2util.h"

/* list of user supplied EUI-64s */
struct _native_eui64_list;
static struct _native_eui64_list {
    eui64_t addr;
    struct _native_eui64_list *prev;
} *_eui64_head;

/* parse EUI-64 from command line */
void native_add_eui64(const char *s)
{
    struct _native_eui64_list *e = malloc(sizeof(*_eui64_head));

    size_t res = l2util_addr_from_str(s, e->addr.uint8);
    assert(res <= sizeof(eui64_t));

    /* if the provided address exceeds eui64_t, l2util_addr_from_str()
     * *will* corrupt memory. */
    if (res > sizeof(eui64_t)) {
        exit(-1);
    }

    e->prev = _eui64_head;
    _eui64_head = e;
}

/* callback for EUI provider */
int native_get_eui64(const void *arg, eui64_t *addr, uint8_t index)
{
    (void) arg;

    uint8_t cnt = 0;
    for (struct _native_eui64_list *e = _eui64_head; e != NULL; e = e->prev) {
        if (cnt++ == index) {
            *addr = e->addr;
            return 0;
        }
    }

    return -1;
}
