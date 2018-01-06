/*
 * Copyright (C) 2018 Inria
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
 * @brief       Test application for MKRWAN LoRa module driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "timex.h"
#include "shell.h"
#include "shell_commands.h"
#include "fmt.h"

#include "mkrwan_params.h"
#include "mkrwan.h"

static mkrwan_t mkrwan_dev;

static uint8_t payload[MKRWAN_INT_BUF_SIZE];

static void _print_join_usage(void)
{
    puts("Usage: join <otaa|abp>");
}

static void _print_send_usage(void)
{
    puts("Usage: send <data> <uncnf|cnf> <port>");
}

static void _print_set_usage(void)
{
    puts("Usage: set "
         "<appeui|devaddr|appskey|nwkskey|class|public|duty_cycle|"
         "dr|adr> <value>");
}

static void _print_get_usage(void)
{
    puts("Usage: get "
         "<deveui|appeui|devaddr|appskey|nwkskey|class|public|duty_cycle|"
         "dr|adr>");
}

int mkrwan_join_cmd(int argc, char **argv)
{
    if (argc != 2) {
        _print_join_usage();
        return 1;
    }

    uint8_t mode;
    if (strcmp(argv[1], "otaa") == 0) {
        mode = LORAMAC_JOIN_OTAA;
    }
    else {
        mode = LORAMAC_JOIN_ABP;
    }

    if (mkrwan_join(&mkrwan_dev, mode) != MKRWAN_OK) {
        puts("Join procedure failed!");
    }
    else {
        puts("Join procedure succeeded!");
    }

    return 0;
}

int mkrwan_send_cmd(int argc, char **argv)
{
    if ((argc < 2) || (argc > 3)) {
        _print_send_usage();
        return 1;
    }

    uint8_t mode = LORAMAC_DEFAULT_TX_MODE;

    if (argc > 2) {
        if (strcmp(argv[2], "cnf") == 0) {
            mode = LORAMAC_TX_CNF;
        }
        else if (strcmp(argv[2], "uncnf") == 0) {
            mode = LORAMAC_TX_UNCNF;
        }
        else {
            _print_send_usage();
            return 1;
        }
    }

    if (mkrwan_send(&mkrwan_dev, (uint8_t*)argv[1], strlen(argv[1]),
                    mode) != MKRWAN_OK) {
        puts("Send failed!");
    }
    else {
        puts("Data sent with success");
    }

    return 0;
}

int mkrwan_set_cmd(int argc, char **argv)
{
    if (argc != 3) {
        _print_set_usage();
        return 1;
    }

    if (strcmp(argv[1], "deveui") == 0) {
        fmt_hex_bytes(payload, argv[2]);
        mkrwan_set_deveui(&mkrwan_dev, payload);
    }
    else if (strcmp(argv[1], "appeui") == 0) {
        fmt_hex_bytes(payload, argv[2]);
        mkrwan_set_appeui(&mkrwan_dev, payload);
    }
    else if (strcmp(argv[1], "appkey") == 0) {
        fmt_hex_bytes(payload, argv[2]);
        mkrwan_set_appkey(&mkrwan_dev, payload);
    }
    else if (strcmp(argv[1], "devaddr") == 0) {
        fmt_hex_bytes(payload, argv[2]);
        mkrwan_set_devaddr(&mkrwan_dev, payload);
    }
    else if (strcmp(argv[1], "appskey") == 0) {
        fmt_hex_bytes(payload, argv[2]);
        mkrwan_set_appskey(&mkrwan_dev, payload);
    }
    else if (strcmp(argv[1], "nwkskey") == 0) {
        fmt_hex_bytes(payload, argv[2]);
        mkrwan_set_nwkskey(&mkrwan_dev, payload);
    }
    else if (strcmp(argv[1], "class") == 0) {
        uint8_t class;
        if (strcmp(argv[2], "A") == 0) {
            class = LORAMAC_CLASS_A;
        }
        else if (strcmp(argv[2], "B") == 0) {
            class = LORAMAC_CLASS_B;
        }
        else if (strcmp(argv[2], "C") == 0) {
            class = LORAMAC_CLASS_C;
        }
        else {
            printf("Invalid class '%s'\n", argv[2]);
            return 1;
        }

        mkrwan_set_class(&mkrwan_dev, class);
    }
    else if (strcmp(argv[1], "public") == 0) {
        mkrwan_set_public_network(&mkrwan_dev, strcmp(argv[2], "on") == 0);
    }
    else if (strcmp(argv[1], "duty_cycle") == 0) {
        mkrwan_set_duty_cycle(&mkrwan_dev, strcmp(argv[2], "on") == 0);
    }
    else if (strcmp(argv[1], "dr") == 0) {
        mkrwan_set_datarate(&mkrwan_dev, atoi(argv[2]));
    }
    else if (strcmp(argv[1], "adr") == 0) {
        mkrwan_set_adr(&mkrwan_dev, (bool)atoi(argv[2]));
    }
    else {
        _print_set_usage();
        return 1;
    }

    return 0;
}

int mkrwan_get_cmd(int argc, char **argv)
{
    if (argc != 2) {
        _print_get_usage();
        return 1;
    }

    if (strcmp(argv[1], "deveui") == 0) {
        char buf[LORAMAC_DEVEUI_LEN * 2 + 1];
        mkrwan_get_deveui(&mkrwan_dev, payload);
        fmt_bytes_hex(buf, payload, LORAMAC_DEVEUI_LEN);
        buf[LORAMAC_DEVEUI_LEN * 2] = '\0';
        printf("device eui: %s\n", buf);
    }
    else if (strcmp(argv[1], "appeui") == 0) {
        char buf[LORAMAC_APPEUI_LEN * 2 + 1];
        mkrwan_get_appeui(&mkrwan_dev, payload);
        fmt_bytes_hex(buf, payload, LORAMAC_APPEUI_LEN);
        buf[LORAMAC_APPEUI_LEN * 2] = '\0';
        printf("application eui: %s\n", buf);
    }
    else if (strcmp(argv[1], "appkey") == 0) {
        char buf[LORAMAC_APPKEY_LEN * 2 + 1];
        mkrwan_get_appkey(&mkrwan_dev, payload);
        fmt_bytes_hex(buf, payload, LORAMAC_APPKEY_LEN);
        buf[LORAMAC_APPKEY_LEN * 2] = '\0';
        printf("application key: %s\n", buf);
    }
    else if (strcmp(argv[1], "devaddr") == 0) {
        char buf[LORAMAC_DEVADDR_LEN * 2 + 1];
        mkrwan_get_devaddr(&mkrwan_dev, payload);
        fmt_bytes_hex(buf, payload, LORAMAC_DEVADDR_LEN);
        buf[LORAMAC_DEVADDR_LEN * 2] = '\0';
        printf("device address: %s\n", buf);
    }
    else if (strcmp(argv[1], "appskey") == 0) {
        char buf[LORAMAC_APPSKEY_LEN * 2 + 1];
        mkrwan_get_appskey(&mkrwan_dev, payload);
        fmt_bytes_hex(buf, payload, LORAMAC_APPSKEY_LEN);
        buf[LORAMAC_APPSKEY_LEN * 2] = '\0';
        printf("application session key: %s\n", buf);
    }
    else if (strcmp(argv[1], "nwkskey") == 0) {
        char buf[LORAMAC_NWKSKEY_LEN * 2 + 1];
        mkrwan_get_nwkskey(&mkrwan_dev, payload);
        fmt_bytes_hex(buf, payload, LORAMAC_NWKSKEY_LEN);
        buf[LORAMAC_NWKSKEY_LEN * 2] = '\0';
        printf("network session key: %s\n", buf);
    }
    else if (strcmp(argv[1], "public") == 0) {
        bool enable = mkrwan_get_public_network(&mkrwan_dev);
        printf("Public network: %s\n", enable ? "on": "off");
    }
    else if (strcmp(argv[1], "duty_cycle") == 0) {
        bool enable = mkrwan_get_duty_cycle(&mkrwan_dev);
        printf("Duty cycle: %s\n", enable ? "on": "off");
    }
    else if (strcmp(argv[1], "dr") == 0) {
        uint8_t dr = mkrwan_get_datarate(&mkrwan_dev);
        printf("Datarate: %d\n", dr);
    }
    else if (strcmp(argv[1], "adr") == 0) {
        bool enable = mkrwan_get_adr(&mkrwan_dev);
        printf("Adaptive datarate: %s\n", enable ? "on": "off");
    }
    else {
        _print_get_usage();
        return 1;
    }

    return 0;
}

int mkrwan_reset_cmd(int argc, char **argv)
{
    if (argc != 1) {
        printf("Usage: %s\n", argv[0]);
        return 1;
    }

    mkrwan_reset(&mkrwan_dev);

    return 0;
}

int mkrwan_version_cmd(int argc, char **argv)
{
    if (argc != 1) {
        printf("Usage: %s\n", argv[0]);
        return 1;
    }

    char buf[MKRWAN_INT_BUF_SIZE];
    mkrwan_version(&mkrwan_dev, buf);
    printf("Version: %s\n", buf);

    return 0;
}

int mkrwan_at_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <at command>\n", argv[0]);
        return 1;
    }

    char buf[MKRWAN_INT_BUF_SIZE];
    at_send_cmd_get_resp(&mkrwan_dev.at_dev, argv[1],
                         buf, sizeof(buf), MKRWAN_TIMEOUT);

    printf("Response: %s\n", buf);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "join",       "Join a network",               mkrwan_join_cmd },
    { "send",       "Send data to the network",     mkrwan_send_cmd },
    { "set",        "Set mkrwan parameters",        mkrwan_set_cmd },
    { "get",        "Get mkrwan parameters",        mkrwan_get_cmd },
    { "reset",      "Reset the device",             mkrwan_reset_cmd },
    { "version",    "Read the device version",      mkrwan_version_cmd },
    { "at",   "Execute an AT command",              mkrwan_at_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("MKRWAN device driver test");

    mkrwan_setup(&mkrwan_dev, &mkrwan_params[0]);
    if (mkrwan_init(&mkrwan_dev) != MKRWAN_OK) {
        puts("MKRWAN initialization failed");
        return -1;
    }

    char buf[LORAMAC_DEVEUI_LEN * 2 + 1];
    mkrwan_get_deveui(&mkrwan_dev, payload);
    fmt_bytes_hex(buf, payload, LORAMAC_DEVEUI_LEN);
    buf[LORAMAC_DEVEUI_LEN * 2] = '\0';
    printf("\nDevice EUI: %s\n\n", buf);

    /* start the shell */
    puts("Initialization OK, starting shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
