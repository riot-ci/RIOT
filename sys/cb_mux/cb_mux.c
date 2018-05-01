/*
 * Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_cb_mux
 * @{
 *
 * @file
 * @brief   cb_mux implementation
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include "cb_mux.h"
#include "utlist.h"

void cb_mux_add(cb_mux_t *head, cb_mux_t *entry)
{
    LL_APPEND(head, entry);
}

void cb_mux_del(cb_mux_t *head, cb_mux_t *entry)
{
    LL_DELETE(head, entry);
}

uint8_t cb_mux_nextid(cb_mux_t *head)
{
    cb_mux_t *entry;
    uint8_t id = 0;

    LL_FOREACH(head, entry) {
        if (entry->cbid >= id) {
            id = entry->cbid + 1;
        }
    }

    return id;
}

cb_mux_t *cb_mux_find_cbid(cb_mux_t *head, uint8_t cbid_val)
{
    cb_mux_t *entry;

    LL_SEARCH_SCALAR(head, entry, cbid, cbid_val);

    return entry;
}

cb_mux_t *cb_mux_find_flags(cb_mux_t *head, uint8_t flags, uint8_t mask)
{
    cb_mux_t *entry;

    LL_FOREACH(head, entry) {
        if ((entry->flags & mask) == flags) {
            return entry;
        }
    }

    return NULL;
}

void cb_mux_update(cb_mux_t *head, cb_mux_ud_func_t func, void *arg)
{
    cb_mux_t *entry;

    LL_FOREACH(head, entry) {
        func(entry, arg);
    }
}
