/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_shell_lock Shell lock
 * @ingroup     sys
 * @brief       Simple module to provide a password protection for the shell.
 *
 * @{
 *
 * @file
 * @brief       Shell interface definition
 */

#ifndef SHELL_LOCK_H
#define SHELL_LOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "shell.h"

/**
 * @brief Lock the login process after given attempts of failed logins for
 * a few seconds
 */
#define ATTEMPTS_BEFORE_TIME_LOCK 3

#ifndef SHELL_LOCK_PASSWORD
    #error Using MODULE_SHELL_LOCK requires defining SHELL_LOCK_PASSWORD
#endif /* SHELL_LOCK_PASSWORD */

void shell_lock_checkpoint(char *line_buf, int len);
bool shell_lock_is_locked(void);

/* This should be independent from the module "shell_commands", as the user may
 * don't want any other command than the mandatory "lock"-command. */
extern const shell_command_t _shell_lock_command_list[];

#ifdef __cplusplus
}
#endif

#endif /* SHELL_LOCK_H */
/** @} */
