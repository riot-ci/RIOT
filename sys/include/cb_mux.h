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
 * @brief   cb_mux interface definitions
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef CB_MUX_H
#define CB_MUX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   cb_mux callback type
 */
typedef void (*cb_mux_cb_t)(void *);

/**
 * @brief   cb_mux list entry structure
 */
typedef struct cb_mux {
    struct cb_mux *next;
    uint8_t flags;
    uint8_t cbid;
    cb_mux_cb_t cb;
    void *arg;
} cb_mux_t;

/**
 * @brief   cb_mux update function callback type for cb_mux_update
 */
typedef void (*cb_mux_ud_func_t)(cb_mux_t *, void *);

/**
 * @brief   Add a new entry to the end of a cb_mux list
 *
 * @param[in] head   pointer to first list entry
 * @param[in] entry  entry to add
 */
void cb_mux_add(cb_mux_t *head, cb_mux_t *entry);

/**
 * @brief   Remove a entry from a cb_mux list
 *
 * @param[in] head   pointer to first list entry
 * @param[in] entry  entry to remove
 */
void cb_mux_del(cb_mux_t *head, cb_mux_t *entry);

/**
 * @brief   Find the next highest ID unused in the list
 *
 * @param[in] head  pointer to first list entry
 *
 * @return the next highest unused ID
 */
uint8_t cb_mux_nextid(cb_mux_t *head);

/**
 * @brief   Find an entry in the list by ID
 *
 * @param[in] head      pointer to first list entry
 * @param[in] cbid_val  ID to find
 *
 * @return pointer to the list entry
 */
cb_mux_t *cb_mux_find_cbid(cb_mux_t *head, uint8_t cbid_val);

/**
 * @brief   Find an entry in the list by flags set
 *
 * Returns the oldest matching entry
 *
 * @param[in] head   pointer to first list entry
 * @param[in] flags  flags to match
 * @param[in] mask   mask of which flag bits to check
 *
 * @return pointer to the list entry
 */
cb_mux_t *cb_mux_find_flags(cb_mux_t *head, uint8_t flags, uint8_t mask);

/**
 * @brief   Run a function on every item in the cb_mux list
 *
 * @param[in] head  pointer to first list entry
 * @param[in] func  function to run on each entry
 * @param[in] arg   argument for the function
 */
void cb_mux_update(cb_mux_t *head, cb_mux_ud_func_t func, void *arg);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* CB_MUX_H */
