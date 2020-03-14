/*
 * Copyright 2013 INRIA.
 * Copyright 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
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
 * @brief       Shell command implementation for the peripheral rtt interface
 *
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>#
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "periph/rtt.h"

static void _alarm_handler(void *arg)
{
    (void) arg;

    puts("The alarm rang");
}

static void _overflow_handler(void *arg)
{
    (void) arg;

    puts("RTT overflow");
}

static int _rtt_get_alarm(void)
{
    printf("%" PRIu32 "\n", rtt_get_alarm());
    return 0;
}

static int _rtt_set_alarm(char **argv)
{
    uint32_t alarm = strtol(argv[0], NULL, 10);
    rtt_set_alarm(alarm, _alarm_handler, NULL);
    return 0;
}

static int _rtt_get_counter(void)
{
    printf("%" PRIu32 "\n", rtt_get_counter());
    return 0;
}

static int _rtt_set_counter(char **argv)
{
    uint32_t counter = strtol(argv[0], NULL, 10);
    rtt_set_counter(counter);
    return 0;
}

static int _rtt_usage(void)
{
    puts("usage: rtt <command> [arguments]");
    puts("commands:");
    puts("\tinit\t\tinit the interface");
    puts("\tpoweron\t\tpower the interface on");
    puts("\tpoweroff\tpower the interface off");
    puts("\tclearoverflow\tdeactivate the overflow interrupt");
    puts("\tsetoverflow\tset the overflow interrupt");
    puts("\tclearalarm\tdeactivate the current alarm");
    puts("\tgetalarm\tprint the currentl alarm counter value");
    puts("\tsetalarm <val>\tset an alarm for the specified counter value");
    puts("\tget\t\tprint the current counter value");
    puts("\tset\t\tset the current counter value");
    return 0;
}

int _rtt_handler(int argc, char **argv)
{
    if (argc < 2) {
        _rtt_usage();
        return 1;
    }
    else if (strncmp(argv[1], "init", 4) == 0) {
        rtt_init();
    }
    else if (strncmp(argv[1], "poweron", 7) == 0) {
        rtt_poweron();
    }
    else if (strncmp(argv[1], "poweroff", 8) == 0) {
        rtt_poweroff();
    }
    else if (strncmp(argv[1], "clearoverflow", 13) == 0) {
        rtt_clear_overflow_cb();
    }
    else if (strncmp(argv[1], "setoverflow", 11) == 0) {
        rtt_set_overflow_cb(_overflow_handler, NULL);
    }
    else if (strncmp(argv[1], "clearalarm", 10) == 0) {
        rtt_clear_alarm();
    }
    else if (strncmp(argv[1], "getalarm", 8) == 0) {
        _rtt_get_alarm();
    }
    else if ((strncmp(argv[1], "setalarm", 8) == 0) && (argc == 3)) {
        _rtt_set_alarm(argv + 2);
    }
    else if (strncmp(argv[1], "get", 3) == 0) {
        _rtt_get_counter();
    }
    else if ((strncmp(argv[1], "set", 3) == 0)  && (argc == 3)) {
        _rtt_set_counter(argv + 2);
    }
    else {
        printf("unknown command or missing parameters: %s\n\n", argv[1]);
        _rtt_usage();
        return 1;
    }
    return 0;
}
