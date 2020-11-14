/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013, 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_sync_mutex Mutex
 * @ingroup     core_sync
 * @brief       Mutex for thread synchronization
 *
 * Mutex Implementation Basics
 * ===========================
 *
 * Data Structures and Encoding
 * ----------------------------
 *
 * A `mutex_t` contains basically a point, which can have one of the following
 * values:
 *
 * 1. `NULL`, in case it is unlocked
 * 2. `MUTEX_LOCKED` in case it is locked, but no other thread is waiting on it
 * 3. A pointer to the head of single linked list of threads (or more precisely
 *    their `thread_t` structures) blocked waiting for obtaining the mutex. This
 *    list is terminated by `NULL`, not by `MUTEX_LOCKED`
 *
 * The same information graphically:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Unlocked mutex:
 * +-------+
 * | Mutex | --> NULL
 * +-------+
 *
 * Locked mutex, no waiters:
 * +-------+
 * | Mutex | --> MUTEX_LOCKED
 * +-------+
 *
 * Locked mutex, one waiter:
 * +-------+     +--------+
 * | Mutex | --> | Waiter | --> NULL
 * +-------+     +--------+
 *
 * Locked mutex, 2 waiters:
 * +-------+     +--------+     +--------+
 * | Mutex | --> | Waiter | --> | Waiter | --> NULL
 * +-------+     +--------+     +--------+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Obtaining a Mutex
 * -----------------
 *
 * If a `mutex_lock()` is called, one of the following happens:
 *
 * 1. If the mutex was unlocked (value of `NULL`), its value is changed to
 *    `MUTEX_LOCKED` and the call to `mutex_lock()` returns right away without
 *    blocking.
 * 2. If the mutex as a vale of `MUTEX_LOCKED`, it will be changed to point to
 *    the `thread_t` of the running thread. The single item list is terminated
 *    be setting the `thread_t::rq_entry.next` of the running thread to `NULL`.
 *    The running thread blocks as described below.
 * 3. Otherwise, the current thread is inserted into the list of waiting
 *    threads sorted by thread priority. The running thread blocks as described
 *    below.
 *
 * In case 2) and 3), the running thread will mark itself as blocked (waiting
 * for a mutex) and yields. Once control is transferred back to this thread
 * (which is done in the call to `mutex_unlock()`), it has the mutex and the
 * function `mutex_lock()` returns.
 *
 * Returning a Mutex
 * -----------------
 *
 * If `mutex_unlock()` is called, one of the following happens:
 *
 * 1. If the mutex was already unlocked (value of `NULL`), the call returns
 *    without modifying the mutex.
 * 2. If the mutex was locked without waiters (value of `MUTEX_LOCKED`), it is
 *    unlocked by setting its value to `NULL`.
 * 3. Otherwise the first `thread_t` from the linked list of waiters is removed
 *    from the list.
 *     - This thread is the one with the highest priority, as the list is sorted
 *       by priority.
 *     - This thread's status is set to pending and its added to the appropriate
 *       run queue.
 *     - If that thread was the last item in the list, the mutex is set to
 *       `MUTEX_LOCK`.
 *     - The scheduler is run, so that if the unblocked waiting thread can
 *       run now, in case it has a higher priority than the running thread.
 * @{
 *
 * @file
 * @brief       Mutex for thread synchronization
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <stddef.h>
#include <stdint.h>

#include "irq.h"
#include "kernel_defines.h"
#include "list.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mutex structure. Must never be modified by the user.
 */
typedef struct {
    /**
     * @brief   The process waiting queue of the mutex. **Must never be changed
     *          by the user.**
     * @internal
     */
    list_node_t queue;
} mutex_t;

/**
 * @brief Static initializer for mutex_t.
 * @details This initializer is preferable to mutex_init().
 */
#define MUTEX_INIT { { NULL } }

/**
 * @brief Static initializer for mutex_t with a locked mutex
 */
#define MUTEX_INIT_LOCKED { { MUTEX_LOCKED } }

/**
 * @cond INTERNAL
 * @brief This is the value of the mutex when locked and no threads are waiting
 *        for it
 */
#define MUTEX_LOCKED ((list_node_t *)-1)
/**
 * @endcond
 */

/**
 * @brief   Initializes a mutex object.
 * @details For initialization of variables use MUTEX_INIT instead.
 *          Only use the function call for dynamically allocated mutexes.
 * @param[out]      mutex   pre-allocated mutex structure, must not be NULL.
 */
static inline void mutex_init(mutex_t *mutex)
{
    mutex->queue.next = NULL;
}

/**
 * @brief   Tries to get a mutex, non-blocking.
 *
 * @param[in,out]   mutex   Mutex object to lock.
 *
 * @retval  1               if mutex was unlocked, now it is locked.
 * @retval  0               if the mutex was locked.
 *
 * @pre     @p mutex is not `NULL`
 * @pre     Mutex at @p mutex has been initialized
 * @pre     Must be called in thread context
 */
static inline int mutex_trylock(mutex_t *mutex)
{
    unsigned irq_state = irq_disable();
    int retval = 0;
    if (mutex->queue.next == NULL) {
        mutex->queue.next = MUTEX_LOCKED;
        retval = 1;
    };
    irq_restore(irq_state);
    return retval;
}

/**
 * @brief   Locks a mutex, blocking.
 *
 * @param[in,out]   mutex   Mutex object to lock.
 *
 * @retval  0               The mutex was locked by the caller
 * @retval  -ECANCELED      The mutex was ***NOT*** locked, operation was
 *                          canceled. See @ref mutex_cancel
 *
 * @pre     @p mutex is not `NULL`
 * @pre     Mutex at @p mutex has been initialized
 * @pre     Must be called in thread context
 *
 * @post    The mutex @p is locked and held by the calling thread, unless
 *          `-ECANCELED` was returned. This can only occur when
 *          @ref mutex_cancel is called for the given mutex and the calling
 *          thread.
 */
int mutex_lock(mutex_t *mutex);

/**
 * @brief   Unlocks the mutex.
 *
 * @param[in,out]   mutex   Mutex object to unlock.
 *
 * @pre     @p mutex is not `NULL`
 * @note    It is safe to unlock a mutex held by a different thread.
 * @note    It is safe to call this function from IRQ context.
 */
void mutex_unlock(mutex_t *mutex);

/**
 * @brief   Unlocks the mutex and sends the current thread to sleep
 *
 * @param[in,out]   mutex   Mutex object to unlock.
 * @pre     @p mutex is not `NULL`
 * @pre     Must be called in thread context.
 */
void mutex_unlock_and_sleep(mutex_t *mutex);

/**
 * @brief   If @p thread is currently blocked waiting for @p mutex, it will
 *          be unblocked without obtaining the mutex
 *
 * @param[in,out]   mutex   Mutex to perform the cancel action on
 * @param[in,out]   thread  Thread to remove from the @p mutex 's wait queue
 *
 * @note    This function is only provided when module `core_mutex_cancel` is
 *          used.
 * @note    It is safe to call this function from IRQ context, e.g. from a timer
 *          interrupt.
 *
 * @details If @p thread is currently running (or pending), a subsequent call
 *          from @p thread to @ref mutex_lock will also be canceled if @p mutex
 *          is passed as parameter. This avoids race conditions when @ref
 *          mutex_cancel get called just before @ref mutex_lock is called.
 */
void mutex_cancel(mutex_t *mutex, thread_t *thread);

#ifdef __cplusplus
}
#endif

#endif /* MUTEX_H */
/** @} */
