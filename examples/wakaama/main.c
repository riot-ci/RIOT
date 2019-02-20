/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application  for Eclipse Wakaama LwM2M Client
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "msg.h"
#include "shell.h"

#define SHELL_QUEUE_SIZE (8)
static msg_t _shell_queue[SHELL_QUEUE_SIZE];

extern void lwm2m_cli_init(void);
extern int lwm2m_cli_cmd(int argc, char **argv);
static const shell_command_t my_commands[] = {
    { "lwm2m", "Start LWM2M client and control light resources", lwm2m_cli_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* initiates LWM2M client */
    lwm2m_cli_init();

    msg_init_queue(_shell_queue, SHELL_QUEUE_SIZE);
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(my_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
