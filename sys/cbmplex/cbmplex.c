/*
 * Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_cbmplex
 * @{
 *
 * @file
 * @brief   cbmplex implementation
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include "cbmplex.h"
#include "utlist.h"

void cbmplex_add(cbmplex_t *head, cbmplex_t *entry)
{
    LL_APPEND(head, entry);
}

void cbmplex_del(cbmplex_t *head, cbmplex_t *entry)
{
    LL_DELETE(head, entry);
}

uint8_t cbmplex_nextid(cbmplex_t *head)
{
    cbmplex_t *entry;
    uint8_t id = 0;

    LL_FOREACH(head, entry) {
        if (entry->cbid >= id) {
            id = entry->cbid + 1;
        }
    }

    return id;
}

cbmplex_t *cbmplex_find_cbid(cbmplex_t *head, uint8_t cbid_val)
{
    cmbplex_t *entry;

    LL_SEARCH_SCALAR(head, entry, cbid, cbid_val);

    return entry;
}

cbmplex_t *cbmplex_find_flags(cbmplex_t *head, uint8_t flags, uint8_t mask)
{
    cbmplex_t *entry;

    LL_FOREACH(head, entry) {
        if ((entry->flags & mask) == flags) {
            return entry;
        }
    }

    return NULL;
}

void cbmplex_update(cbmplex_t *head, cbmplex_ud_func_t func, void *arg)
{
    cbmplex_t *entry;

    LL_FOREACH(head, entry) {
        func(entry, arg);
    }
}
