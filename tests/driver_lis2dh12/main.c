/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
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
 * @brief       Test application for LIS2DH12 accelerometer driver
 *
 * @author      Jan Mohr <jan.mohr@ml-pa.com>
 *
 * @}
 */

#include "shell.h"

#include "thread.h"

#include "include/lis2dh12_test.h"

kernel_pid_t lis2dh12_process;

static const shell_command_t shell_commands[] = {
                { "lis", "Command with multiple subcommands.", shell_lis2dh12_cmd },
                { NULL, NULL, NULL },
};

char lis2dh12_process_stack[THREAD_STACKSIZE_MAIN];

int main(void)
{
    /* processing lis2dh12 acceleration data */
    lis2dh12_process = thread_create(lis2dh12_process_stack, sizeof(lis2dh12_process_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_SLEEPING,
                  lis2dh12_test_process, NULL, "lis2dh12_process");

    /* init lis */
    lis2dh12_test_init();

    /* running shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    printf("THIS SHOULD NEVER BE REACHED!\n");
    return 0;
}
