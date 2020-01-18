/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_lock
 * @{
 *
 * @file
 * @brief       Module to lock the running shell with a password.
 *
 * The Shell is proceeded only when the valid password was entered by the user.
 * After 3 (default) failed attempts, the input is blocked for a few seconds to
 * slow down brute force attacks.
 * Does not make use of any cryptographic features yet.
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "xtimer.h"

#include "shell_lock.h"

static bool _shell_is_locked = true;

/* defined in shell.c */
extern void flush_if_needed(void);

/* defined in shell.c */
extern void print_prompt(void);

int _lock_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    _shell_is_locked = true;

    return 0;
}

/* This should be independent from the module "shell_commands", as the user may
 * don't want any other command than the mandatory "lock"-command. */
const shell_command_t _shell_lock_command_list[] = {
        {"lock", "lock the shell", _lock_handler},
        {NULL, NULL, NULL}
};

static bool _login(char *line_buf, size_t buf_size)
{
    int success = false;

    printf("Password: \n");
    flush_if_needed();

    print_prompt();

    if (fgets(line_buf, buf_size, stdin) != NULL) {

        char *line = strtok(line_buf, "\n");

        if (strcmp(line, SHELL_LOCK_PASSWORD) == 0) {
            success = true;
        }
    }

    return success;
}

/**
 * Repeatedly prompt for the password.
 *
 * This function won't return until the correct password has been
 * introduced.
 */
void _login_barrier(char *line_buf, size_t buf_size)
{
    while (1) {
        int attempts = ATTEMPTS_BEFORE_TIME_LOCK;

        while (attempts--) {
            if (_login(line_buf, buf_size)) {
                return;
            }
            puts("Wrong password");
            xtimer_sleep(1);
        }
        xtimer_sleep(7);
    }
}

bool shell_lock_is_locked(void)
{
    return _shell_is_locked;
}

void shell_lock_checkpoint(char *line_buf, int len)
{
    if (_shell_is_locked) {
        printf("The shell is locked. Enter a valid password to unlock.\n\n");

        _login_barrier(line_buf, len);

        _shell_is_locked = false;
    }
}
