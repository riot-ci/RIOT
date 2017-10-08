/*
 * Copyright (C) 2017 Inria
 *               2017 Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 *
 * @file
 * @brief       Semtech LoRaMAC test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Jose Alamos <jose.alamos@inria.cl>
 */

#include <string.h>

#include "msg.h"
#include "shell.h"

#include "net/netdev.h"
#include "net/loramac.h"
#include "semtech_loramac.h"

#include "sx127x.h"
#include "sx127x_params.h"
#include "sx127x_netdev.h"

sx127x_t sx127x;
static uint8_t rx_buf[242] = { 0 };

static void _join_usage(void)
{
    puts("Usage: join <otaa|abp>");
}

static void _send_usage(void)
{
    puts("Usage: send <cnf|uncnf> <port> <payload>");
}

static void _set_usage(void)
{
    puts("Usage: set <dr|adr|public|netid|tx_power> <value>");
}

static void _get_usage(void)
{
    puts("Usage: get <dr|adr|public|netid|tx_power>");
}

static int _cmd_loramac_join(int argc, char **argv)
{
    if (argc < 2) {
        _join_usage();
        return 1;
    }

    uint8_t join_type;
    if (strcmp(argv[1], "otaa") == 0) {
        join_type = LORAMAC_JOIN_OTAA;
    }
    else if (strcmp(argv[1], "abp") == 0) {
        join_type = LORAMAC_JOIN_ABP;
    }
    else {
        (void) join_type;
        _join_usage();
        return 1;
    }

    if (semtech_loramac_join(join_type) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed!");
        return 1;
    }

    puts("Join procedure succeeded!");
    return 0;
}

static int _cmd_loramac_send(int argc, char **argv)
{
    if (argc < 4) {
        _send_usage();
        return 1;
    }

    uint8_t cnf;
    if (strcmp(argv[1], "cnf") == 0) {
        cnf = LORAMAC_TX_CNF;
    }
    else if (strcmp(argv[1], "uncnf") == 0) {
        cnf = LORAMAC_TX_UNCNF;
    }
    else {
        (void) cnf;
        _send_usage();
        return 1;
    }

    uint8_t port = atoi(argv[2]);
    if (port == 0 || port >= 224) {
        printf("error: invalid port given '%d', "
               "port can only be between 1 and 223\n", port);
        return 1;
    }

    switch (semtech_loramac_send(cnf, port,
                                 (uint8_t*)argv[3], strlen(argv[3]), rx_buf)) {
        case SEMTECH_LORAMAC_RX_DATA:
            printf("Data received: %s\n", (char*)rx_buf);
            return 0;

        case SEMTECH_LORAMAC_TX_DONE:
            puts("TX done");
            return 0;

        case SEMTECH_LORAMAC_NOT_JOINED:
            puts("Failed: not joined");
            return 1;

        default:
            break;
    }

    return 0;
}

static int _cmd_loramac_set(int argc, char **argv)
{
    if (argc < 3) {
        _set_usage();
        return 1;
    }

    if (strcmp("dr", argv[1]) == 0) {
        uint8_t dr = atoi(argv[2]);
        semtech_loramac_set_dr(dr);
    }
    else if (strcmp("adr", argv[1]) == 0) {
        bool adr;
        if (strcmp("on", argv[2]) == 0) {
            adr = true;
        }
        else if (strcmp("off", argv[2]) == 0) {
            adr = false;
        }
        else {
            puts("Usage: set adr <on|off>");
            return 1;
        }
        semtech_loramac_set_adr(adr);
    }
    else if (strcmp("public", argv[1]) == 0) {
        bool public;
        if (strcmp("on", argv[2]) == 0) {
            public = true;
        }
        else if (strcmp("off", argv[2]) == 0) {
            public = false;
        }
        else {
            puts("Usage: set public <on|off>");
            return 1;
        }
        semtech_loramac_set_public_network(public);
    }
    else if (strcmp("netid", argv[1]) == 0) {
        semtech_loramac_set_netid(strtoul(argv[2], NULL, 0));
    }
    else if (strcmp("tx_power", argv[1]) == 0) {
        uint8_t power = atoi(argv[2]);
        if (power > LORAMAC_TX_PWR_15) {
            puts("Usage: set tx_power <0..16>");
            return 1;
        }

        semtech_loramac_set_tx_power(power);
    }
    else {
        _set_usage();
        return 1;
    }

    return 0;
}

static int _cmd_loramac_get(int argc, char **argv)
{
    if (argc < 2) {
        _get_usage();
        return 1;
    }

    if (strcmp("dr", argv[1]) == 0) {
        printf("DATARATE: %d\n",
               semtech_loramac_get_dr());
    }
    else if (strcmp("adr", argv[1]) == 0) {
        printf("ADR: %s\n",
               semtech_loramac_get_adr() ? "on" : "off");
    }
    else if (strcmp("public", argv[1]) == 0) {
        printf("Public network: %s\n",
               semtech_loramac_get_public_network() ? "on" : "off");
    }
    else if (strcmp("netid", argv[1]) == 0) {
        printf("NetID: %lu\n", semtech_loramac_get_netid());
    }
    else if (strcmp("tx_power", argv[1]) == 0) {
        printf("TX power index: %d\n", semtech_loramac_get_tx_power());
    }
    else {
        _set_usage();
        return 1;
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "join", "try to join lorawan network", _cmd_loramac_join },
    { "send", "send some data", _cmd_loramac_send },
    { "set",  "set MAC parameters", _cmd_loramac_set },
    { "get",  "get MAC parameters", _cmd_loramac_get },
    { NULL, NULL, NULL }
};

int main(void)
{
    sx127x_setup(&sx127x, &sx127x_params[0]);
    semtech_loramac_init(&sx127x);

    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}
