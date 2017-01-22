/*
 * Copyright (C) 2016-2018 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @brief       Power management peripheral test.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "periph/pm.h"
#include "pm_layered.h"

#include "shell.h"

#ifdef MODULE_PM_LAYERED
static int cmd_block(int argc, char **argv);
#endif
static int cmd_lowest(int argc, char **argv);
static int cmd_off(int argc, char **argv);
static int cmd_reboot(int argc, char **argv);
#ifdef MODULE_PM_LAYERED
static int cmd_set(int argc, char **argv);
static int cmd_unblock(int argc, char **argv);
#endif

/**
 * @brief   List of shell commands for this example.
 */
static const shell_command_t shell_commands[] = {
#ifdef MODULE_PM_LAYERED
    { "block", "block power mode", cmd_block },
#endif
    { "lowest", "enter lowest mode", cmd_lowest },
    { "off", "turn off", cmd_off },
    { "reboot", "reboot", cmd_reboot },
#ifdef MODULE_PM_LAYERED
    { "set", "set power mode", cmd_set },
    { "unblock", "unblock power mode", cmd_unblock },
#endif
    { NULL, NULL, NULL }
};

static int parse_mode(int argc, char **argv)
{
    if (argc != 2) {
        printf("Power mode expected (0 - %d).\n", PM_NUM_MODES - 1);
        fflush(stdout);

        return -1;
    }

    int mode = atoi(argv[1]);

    if (mode < 0 || mode >= (int) PM_NUM_MODES) {
        printf("Value not in range 0 - %d.\n", PM_NUM_MODES - 1);
        fflush(stdout);

        return -1;
    }

    return mode;
}

#ifdef MODULE_PM_LAYERED
static int cmd_block(int argc, char **argv)
{
    int mode = parse_mode(argc, argv);

    if (mode < 0) {
        return 1;
    }

    printf("Blocking power mode %d.\n", mode);
    fflush(stdout);

    pm_block(mode);

    return 0;
}
#endif

static int cmd_lowest(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("CPU will enter lowest power mode possible.");
    fflush(stdout);

    pm_set_lowest();

    return 0;
}

static int cmd_off(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("CPU will turn off.");
    fflush(stdout);

    pm_off();

    return 0;
}

static int cmd_reboot(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("CPU will reboot.");
    fflush(stdout);

    pm_reboot();

    return 0;
}

#ifdef MODULE_PM_LAYERED
static int cmd_set(int argc, char **argv)
{
    int mode = parse_mode(argc, argv);

    if (mode < 0) {
        return 1;
    }

    printf("CPU will enter power mode %d.\n", mode);
    fflush(stdout);

    pm_set(mode);

    return 0;
}

static int cmd_unblock(int argc, char **argv)
{
    int mode = parse_mode(argc, argv);

    if (mode < 0) {
        return 1;
    }

    printf("Unblocking power mode %d.\n", mode);
    fflush(stdout);

    pm_unblock(mode);

    return 0;
}
#endif

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* print some information about the modes */
    printf("This application allows you to test the power management\n"
           "peripheral. The available power modes are 0 - %d. Lower-numbered\n"
           "power modes save more power, but may require an event/interrupt\n"
           "to wake up the CPU. Reset the CPU if needed.\n",
           PM_NUM_MODES - 1);

    /* run the shell and wait for the user to enter a mode */
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
