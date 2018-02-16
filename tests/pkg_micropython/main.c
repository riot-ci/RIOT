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
#include "py/stackctrl.h"
#include "lib/utils/pyexec.h"

#include "boot.py.h"

static char mp_heap[MP_RIOT_HEAPSIZE];

int main(void)
{
    /* let micropython know the top of this thread's stack */
    uint32_t stack_dummy;
    mp_stack_set_top((char*)&stack_dummy);
    // Make MicroPython's stack limit somewhat smaller than full stack available
    mp_stack_set_limit(THREAD_STACKSIZE_MAIN - 512);

    mp_riot_init(mp_heap, sizeof(mp_heap));

    printf("-- Executing boot.py\n");
    mp_do_str((const char *)boot_py, boot_py_len);

    printf("-- boot.py exited. Starting REPL..\n");

    while (1) {
        if (pyexec_friendly_repl() != 0) {
            break;
        }
    }

    return 0;
}
