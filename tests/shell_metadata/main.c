/*
 * Copyright (C) 2013 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       shows how to set up own and use the system shell commands.
 *              By typing help in the serial console, all the supported commands
 *              are listed.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 */

#include <stdio.h>
#include <string.h>

#include "shell_commands.h"
#include "shell.h"

int main(void)
{
    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
