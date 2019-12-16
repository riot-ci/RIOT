/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands interactive sync util
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <stdio.h>
#include "test_utils/interactive_sync.h"

#if defined(__AVR__)
#include <avr/pgmspace.h>
/* For small AVR targets the extra strings generated by test interactive
   can make the application overflow the .data section (RAM), we use puts_P()
   to write those constant string to .txt section (FLASH)*/
#define PUTS(_s)                       puts_P(PSTR(_s))
#else
#define PUTS(_s)                       puts(_s)
#endif

int _test_start(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    PUTS("START");

    return 0;
}


int _test_ready(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    PUTS("READY");
    return 0;
}
