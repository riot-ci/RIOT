/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for testing the core_mutex_cancel module
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "mutex.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "xtimer.h"

static mutex_t testlock = MUTEX_INIT;

static void cb_unlock(void *unused)
{
    (void)unused;
    mutex_unlock(&testlock);
}

static void cb_cancel(void *main_thread)
{
    mutex_cancel(&testlock, main_thread);
}

int main(void)
{
    xtimer_t xt = { .arg = thread_get_active() };
    puts(
        "Test Application for core_mutex_cancel\n"
        "======================================\n"
    );

    expect(mutex_lock(&testlock) == 0);


    /* If the cancel hits just before the call to mutex_lock(), the call to
     * mutex_lock() should still fail. This avoids races in code like:
     *
     *     setup_isr_for_cancel();
     *     mutex_lock()
     *
     * If the IRQ is triggered before the call to `mutex_lock()` is done, we
     * still one the `mutex_lock()` to fail.
     */
    mutex_cancel(&testlock, thread_get_active());
    expect(mutex_lock(&testlock) == -ECANCELED);

    /* The `mutex_cancel()` must only affects a single call to `mutex_lock()`.
     * Hence, this time `mutex_lock()` must succeed (once `cb_unlock()` is 
     * run).
     */
    xt.callback = cb_unlock;
    xtimer_set(&xt, US_PER_MS * 10);
    expect(mutex_lock(&testlock) == 0);

    /* Also check the default case: A thread is blocked waiting for the mutex
     * when `mutex_cancel()` is called:
     */
    xt.callback = cb_cancel;
    xtimer_set(&xt, US_PER_MS * 10);
    expect(mutex_lock(&testlock) == -ECANCELED);

    puts("TEST PASSED");

    return 0;
}
