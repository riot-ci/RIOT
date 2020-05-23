/*
 * Copyright (C) 2020 Benjamin Valentin
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
 * @brief       Test application to activate the STM32 bootloader mode.
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "shell_commands.h"

extern void usb_board_reset_in_bootloader(void);

static int _bootloader_reset(int argc, char** argv)
{
    (void) argc;
    (void) argv;

    usb_board_reset_in_bootloader();

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "bootloader", "jump to STM32 bootloader", _bootloader_reset},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("STM32 bootloader test application.");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
