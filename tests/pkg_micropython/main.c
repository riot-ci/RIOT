/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       micropython example application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"

#include "micropython.h"

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "stmhal/pyexec.h"

#include "boot.py.h"

static char mp_heap[8192];

int main(void)
{
    mp_riot_init(sched_active_thread->sp, THREAD_STACKSIZE_MAIN,
            mp_heap, sizeof(mp_heap));

    printf("-- Executing boot.py\n");
    mp_do_str((const char *)boot_py, boot_py_len);

    printf("-- boot.py exited. Starting REPL...\n");
    pyexec_friendly_repl();

    return 0;
}
