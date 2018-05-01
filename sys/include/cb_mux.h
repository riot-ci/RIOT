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
    uint32_t cbid;        /**< identifier for this callback */
    struct cb_mux *next;  /**< next entry in the cb_mux list */
    void *info;           /**< optional extra information */
    cb_mux_cb_t cb;       /**< callback function */
    void *arg;            /**< argument for callback function */
} cb_mux_t;

/**
 * @brief   cb_mux iterate function callback type for cb_mux_iter
 */
typedef void (*cb_mux_iter_t)(cb_mux_t *, void *);

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
 * @brief   Find an entry in the list by ID
 *
 * @param[in] head      pointer to first list entry
 * @param[in] cbid_val  ID to find
 *
 * @return pointer to the list entry
 */
cb_mux_t *cb_mux_find_cbid(cb_mux_t *head, uint32_t cbid_val);

/**
 * @brief   Find the entry with the highest or lowest ID
 *
 * If there are multiple hits, this returns the oldest.
 *
 * @param[in] head   pointer to first list entry
 * @param[in] order  0 for lowest ID, !0 for highest
 *
 * @return pointer to the list entry
 */
cb_mux_t *cb_mux_find_hilo_id(cb_mux_t *head, uint8_t order);

/**
 * @brief   Run a function on every item in the cb_mux list
 *
 * @param[in] head  pointer to first list entry
 * @param[in] func  function to run on each entry
 * @param[in] arg   argument for the function
 */
void cb_mux_iter(cb_mux_t *head, cb_mux_iter_t func, void *arg);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* CB_MUX_H */
