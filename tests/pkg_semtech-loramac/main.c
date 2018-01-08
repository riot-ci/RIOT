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
static char print_buf[48] = { 0 };

static void _hex_to_bytes(const char *hex, uint8_t *byte_array)
{
    const uint8_t charmap[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* 01234567 */
        0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 89:;<=>? */
        0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, /* @ABCDEFG */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* HIJKLMNO */
    };

    size_t len = strlen(hex);
    for (uint8_t pos = 0; pos < len; pos += 2) {
        uint8_t idx0 = ((uint8_t)hex[pos + 0] & 0x1F) ^ 0x10;
        uint8_t idx1 = ((uint8_t)hex[pos + 1] & 0x1F) ^ 0x10;
        byte_array[pos / 2] = (uint8_t)(charmap[idx0] << 4) | charmap[idx1];
    };
}

void _bytes_to_hex(const uint8_t *byte_array, char *hex, uint8_t max_len)
{
    char *pos = &hex[0];
    const char *hex_chars = "0123456789ABCDEF";

    for (unsigned i = 0; i < max_len; ++i) {
        *pos++ = hex_chars[(*byte_array >> 4) & 0xF];
        *pos++ = hex_chars[(*byte_array++) & 0xF];
    }
    *pos = '\0';
}

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
    puts("Usage: set <deveui|appeui|appkey|appskey|nwkskey|devaddr|dr|adr|"
         "public|netid|tx_power> <value>");
}

static void _get_usage(void)
{
    puts("Usage: get <deveui|appeui|appkey|appskey|nwkskey|devaddr|dr|adr|"
         "public|netid|tx_power>");
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

    if (strcmp("deveui", argv[1]) == 0) {
        uint8_t deveui[8];
        _hex_to_bytes(argv[2], deveui);
        semtech_loramac_set_deveui(deveui);
    }
    else if (strcmp("appeui", argv[1]) == 0) {
        uint8_t appeui[8];
        _hex_to_bytes(argv[2], appeui);
        semtech_loramac_set_appeui(appeui);
    }
    else if (strcmp("appkey", argv[1]) == 0) {
        uint8_t appkey[16];
        _hex_to_bytes(argv[2], appkey);
        semtech_loramac_set_appkey(appkey);
    }
    else if (strcmp("appskey", argv[1]) == 0) {
        uint8_t appskey[16];
        _hex_to_bytes(argv[2], appskey);
        semtech_loramac_set_appskey(appskey);
    }
    else if (strcmp("nwkskey", argv[1]) == 0) {
        uint8_t nwkskey[16];
        _hex_to_bytes(argv[2], nwkskey);
        semtech_loramac_set_nwkskey(nwkskey);
    }
    else if (strcmp("devaddr", argv[1]) == 0) {
        uint8_t devaddr[4];
        _hex_to_bytes(argv[2], devaddr);
        semtech_loramac_set_devaddr(devaddr);
    }
    else if (strcmp("dr", argv[1]) == 0) {
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

    if (strcmp("deveui", argv[1]) == 0) {
        uint8_t deveui[8];
        semtech_loramac_get_deveui(deveui);
        _bytes_to_hex(deveui, print_buf, 8);
        printf("DEVEUI: %s\n", print_buf);
    }
    else if (strcmp("appeui", argv[1]) == 0) {
        uint8_t appeui[8];
        semtech_loramac_get_appeui(appeui);
        _bytes_to_hex(appeui, print_buf, 8);
        printf("APPEUI: %s\n", print_buf);
    }
    else if (strcmp("appkey", argv[1]) == 0) {
        uint8_t appkey[16];
        semtech_loramac_get_appkey(appkey);
        _bytes_to_hex(appkey, print_buf, 16);
        printf("APPKEY: %s\n", print_buf);
    }
    else if (strcmp("appskey", argv[1]) == 0) {
        uint8_t appskey[16];
        semtech_loramac_get_appskey(appskey);
        _bytes_to_hex(appskey, print_buf, 16);
        printf("APPSKEY: %s\n", print_buf);
    }
    else if (strcmp("nwkskey", argv[1]) == 0) {
        uint8_t nwkskey[16];
        semtech_loramac_get_nwkskey(nwkskey);
        _bytes_to_hex(nwkskey, print_buf, 16);
        printf("NWKSKEY: %s\n", print_buf);
    }
    else if (strcmp("devaddr", argv[1]) == 0) {
        uint8_t devaddr[4];
        semtech_loramac_get_devaddr(devaddr);
        _bytes_to_hex(devaddr, print_buf, 4);
        printf("DEVADDR: %s\n", print_buf);
    }
    else if (strcmp("dr", argv[1]) == 0) {
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
