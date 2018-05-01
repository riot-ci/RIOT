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
 * @brief   cbmplex interface definitions
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   cbmplex callback type
 */
typedef void (*cbmplex_cb_t)(void *);

/**
 * @brief   cbmplex list entry structure
 */
typedef struct cbmplex {
    struct cbmplex *next;
    uint8_t flags;
    uint8_t cbid;
    cbmplex_cb_t cb;
    void *arg;
} cbmplex_t;

/**
 * @brief   cbmplex update function callback type for cbmplex_update
 */
typedef void (*cbmplex_ud_func_t)(cbmplex_t *, void *);

/**
 * @brief   Add a new entry to the end of a cmbplex list
 *
 * @param[in] head   pointer to first list entry
 * @param[in] entry  entry to add
 */
void cbmplex_add(cbmplex_t *head, cbmplex_t *entry);

/**
 * @brief   Remove a entry from a cmbplex list
 *
 * @param[in] head   pointer to first list entry
 * @param[in] entry  entry to remove
 */
void cbmplex_del(cbmplex_t *head, cbmplex_t *entry);

/**
 * @brief   Find the next highest ID unused in the list
 *
 * @param[in] head  pointer to first list entry
 *
 * @return the next highest unused ID
 */
uint8_t cbmplex_nextid(cbmplex_t *head);

/**
 * @brief   Find an entry in the list by ID
 *
 * @param[in] head      pointer to first list entry
 * @param[in] cbid_val  ID to find
 *
 * @return pointer to the list entry
 */
cbmplex_t *cbmplex_find_cbid(cbmplex_t *head, uint8_t cbid_val);

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
cbmplex_t *cbmplex_find_flags(cbmplex_t *head, uint8_t flags, uint8_t mask);

/**
 * @brief   Run a function on every item in the cbmplex list
 *
 * @param[in] head  pointer to first list entry
 * @param[in] func  function to run on each entry
 * @param[in] arg   argument for the function
 */
void cbmplex_update(cbmplex_t *head, cbmplex_ud_func_t func, void *arg);

#ifdef __cplusplus
}
#endif

/** @} */
