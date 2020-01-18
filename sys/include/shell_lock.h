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

#ifdef MODULE_SHELL_LOCK_AUTO_LOCKING
    /**
     * @brief Lock the shell after this time span without user input
     *        Defaults to 5 minutes and can be overwritten by defining
     *        SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS in the applications Makefile
     */
    #define MAX_AUTO_LOCK_PAUSE_MS 5 * 60 * 1000

    #ifdef SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS
        #undef MAX_AUTO_LOCK_PAUSE_MS
        #define MAX_AUTO_LOCK_PAUSE_MS SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS
    #endif /* SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS */

    /**
     * @brief Offset used for the thread for automated locking, so that the
     *        thread is not woken up shortely before it has to lock the shell.
     */
    #define TIMER_SLEEP_OFFSET_MS 100
#endif /* MODULE_SHELL_LOCK_AUTO_LOCKING */

void shell_lock_checkpoint(char *line_buf, int len);
bool shell_lock_is_locked(void);

#ifdef MODULE_SHELL_LOCK_AUTO_LOCKING
void shell_lock_auto_lock_refresh(void);
#endif /* MODULE_SHELL_LOCK_AUTO_LOCKING */

/* This should be independent from the module "shell_commands", as the user may
 * don't want any other command than the mandatory "lock"-command. */
extern const shell_command_t _shell_lock_command_list[];

#ifdef __cplusplus
}
#endif

#endif /* SHELL_LOCK_H */
/** @} */
