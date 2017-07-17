/*
 * Copyright (C) 2017 Inria
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
 * @brief       Shell commands for interacting with SX1272/1276 interfaces
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "thread.h"
#include "sx127x.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netapi.h"
#include "net/netopt.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"


/* utility functions */
static bool _is_number(char *str)
{
    for (; *str; str++) {
        if (*str < '0' || *str > '9') {
            return false;
        }
    }

    return true;
}

static bool _is_iface(kernel_pid_t dev)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t numof = gnrc_netif_get(ifs);

    for (size_t i = 0; i < numof && i < GNRC_NETIF_NUMOF; i++) {
        if (ifs[i] == dev) {
            return true;
        }
    }

    return false;
}

static void _set_usage(char *cmd_name)
{
    printf("usage: %s <if_id> set <key> <value>\n", cmd_name);
    puts("      Sets an hardware specific specific value\n"
         "      <key> may be one of the following\n"
         "       * \"channel\" - frequency channel\n"
         "       * \"chan\" - alias for \"channel\"\n"
         "       * \"bandwidth\" - bandwidth value\n"
         "       * \"bw\" - alias for \"bandwidth\"\n"
         "       * \"spreading_factor\" - spreading factor value\n"
         "       * \"sf\" - alias for \"spreading_factor\"\n"
         "       * \"coding_rate\" - coding rate value\n"
         "       * \"cr\" - alias for \"coding_rate\"\n"
         "       * \"power\" - TX power in dBm\n"
         "       * \"rx_mode\" - Reception mode (single, continuous)\n"
         "       * \"inverted_iq\" - Inverted IQ\n"
         "       * \"state\" - set the device state\n");
}

static void _print_netopt(netopt_t opt)
{
    switch (opt) {
        case NETOPT_CHANNEL:
            printf("Frequency channel [in Hz]");
            break;

        case NETOPT_BANDWIDTH:
            printf("Bandwidth [in kHz]");
            break;

        case NETOPT_SPREADING_FACTOR:
            printf("Spreading factor [7..12]");
            break;

        case NETOPT_CODING_RATE:
            printf("Coding rate [5..8]");
            break;

        case NETOPT_TX_POWER:
            printf("TX power [in dBm]");
            break;

        case NETOPT_SINGLE_RECEIVE:
            printf("RX mode [single or continuous]");
            break;

        case NETOPT_IQ_INVERT:
            printf("Inverted IQ [on, off]");
            break;

        default:
            /* we don't serve these options here */
            break;
    }
}

static void _print_netopt_state(netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_OFF:
            printf("OFF");
            break;
        case NETOPT_STATE_SLEEP:
            printf("SLEEP");
            break;
        case NETOPT_STATE_STANDBY:
            printf("STANDBY");
            break;
        case NETOPT_STATE_IDLE:
            printf("IDLE");
            break;
        case NETOPT_STATE_RX:
            printf("RX");
            break;
        case NETOPT_STATE_TX:
            printf("TX");
            break;
        case NETOPT_STATE_RESET:
            printf("RESET");
            break;
        default:
            /* Do nothing */
            break;
    }
}

static void _print_netopt_bandwidth(uint8_t bw)
{
    switch (bw) {
        case SX127X_BW_125_KHZ:
            printf("125kHz");
            break;
        case SX127X_BW_250_KHZ:
            printf("250kHz");
            break;
        case SX127X_BW_500_KHZ:
            printf("500kHz");
            break;
        default:
            /* Do nothing */
            break;
    }
}

static void _print_netopt_cr(uint8_t cr)
{
    switch (cr) {
        case SX127X_CR_4_5:
            printf("4/5");
            break;
        case SX127X_CR_4_6:
            printf("4/6");
            break;
        case SX127X_CR_4_7:
            printf("4/7");
            break;
        case SX127X_CR_4_8:
            printf("4/8");
            break;
        default:
            /* Do nothing */
            break;
    }
}

static void _sx127x_list(kernel_pid_t dev)
{
    uint8_t u8;
    uint32_t u32;
    netopt_state_t state;
    netopt_enable_t enable = NETOPT_DISABLE;

    printf("Iface %2d  ", dev);

    if (gnrc_netapi_get(dev, NETOPT_CHANNEL, 0, &u32, sizeof(u32)) >= 0) {
        printf(" Channel: %" PRIu32 "Hz ", u32);
    }

    if (gnrc_netapi_get(dev, NETOPT_TX_POWER, 0, &u8, sizeof(u8)) >= 0) {
        printf(" TX-Power: %ddBm ", u8);
    }

    if (gnrc_netapi_get(dev, NETOPT_STATE, 0, &state, sizeof(state)) >= 0) {
        printf(" State: ");
        _print_netopt_state(state);
    }

    printf("\n           ");

    if (gnrc_netapi_get(dev, NETOPT_BANDWIDTH, 0, &u8, sizeof(u8)) >= 0) {
        printf("Bandwidth: ");
        _print_netopt_bandwidth(u8);
    }

    printf("\n           ");

    if (gnrc_netapi_get(dev, NETOPT_SPREADING_FACTOR, 0, &u8, sizeof(u8)) >= 0) {
        printf("Spreading factor: %d ", u8);
    }

    printf("\n           ");

    if (gnrc_netapi_get(dev, NETOPT_CODING_RATE, 0, &u8, sizeof(u8)) >= 0) {
        printf("Coding rate: 4/%d ", u8 + 4);
    }

    printf("\n           ");

    if (state == NETOPT_STATE_IDLE) {
        if (gnrc_netapi_get(dev, NETOPT_SINGLE_RECEIVE, 0, &enable, sizeof(enable)) >= 0) {
            printf("RX mode: ");
            if (enable == NETOPT_ENABLE) {
                printf("single");
            }
            else {
                printf("continuous");
            }
        }

        printf("\n           ");
    }

    if (gnrc_netapi_get(dev, NETOPT_IQ_INVERT, 0, &enable, sizeof(enable)) >= 0) {
        printf("Inverted IQ: ");
        if (enable == NETOPT_ENABLE) {
            printf("on");
        }
        else {
            printf("off");
        }
    }


    puts("");
}

static int _sx127x_set_u32(kernel_pid_t dev, netopt_t opt, char *u32_str)
{
    uint32_t res;
    bool hex = false;

    if (_is_number(u32_str)) {
        if ((res = strtoul(u32_str, NULL, 10)) == ULONG_MAX) {
            puts("error: unable to parse value.\n"
                 "Must be a 32-bit unsigned integer (dec or hex)\n");
            return 1;
        }
    }
    else {
        if ((res = strtoul(u32_str, NULL, 16)) == ULONG_MAX) {
            puts("error: unable to parse value.\n"
                 "Must be a 32-bit unsigned integer (dec or hex)\n");
            return 1;
        }

        hex = true;
    }

    if (res > 0xffffffff) {
        puts("error: unable to parse value.\n"
             "Must be a 32-bit unsigned integer (dec or hex)\n");
        return 1;
    }

    if (gnrc_netapi_set(dev, opt, 0, (uint32_t *)&res, sizeof(uint32_t)) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to ", dev);

    if (hex) {
        printf("0x%08lx\n", res);
    }
    else {
        printf("%lu\n", res);
    }

    return 0;
}

static int _sx127x_set_u8(kernel_pid_t dev, netopt_t opt, char *u8_str)
{
    uint8_t val = atoi(u8_str);

    if (gnrc_netapi_set(dev, opt, 0, (uint8_t *)&val, sizeof(uint8_t)) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to %i\n", dev, val);

    return 0;
}

static int _sx127x_set_flag(kernel_pid_t dev, netopt_t opt,
                           netopt_enable_t set)
{
    if (gnrc_netapi_set(dev, opt, 0, &set, sizeof(netopt_enable_t)) < 0) {
        puts("error: unable to set option");
        return 1;
    }
    printf("success: %sset option\n", (set) ? "" : "un");
    return 0;
}

static int _sx127x_set_state(kernel_pid_t dev, char *state_str)
{
    netopt_state_t state;

    if ((strcmp("off", state_str) == 0) || (strcmp("OFF", state_str) == 0)) {
        state = NETOPT_STATE_OFF;
    }
    else if ((strcmp("sleep", state_str) == 0) ||
             (strcmp("SLEEP", state_str) == 0)) {
        state = NETOPT_STATE_SLEEP;
    }
    else if ((strcmp("idle", state_str) == 0) ||
             (strcmp("IDLE", state_str) == 0)) {
        state = NETOPT_STATE_IDLE;
    }
    else if ((strcmp("standby", state_str) == 0) ||
             (strcmp("STANDBY", state_str) == 0)) {
        state = NETOPT_STATE_STANDBY;
    }
    else if ((strcmp("reset", state_str) == 0) ||
             (strcmp("RESET", state_str) == 0)) {
        state = NETOPT_STATE_RESET;
    }
    else if ((strcmp("rx", state_str) == 0) ||
             (strcmp("RX", state_str) == 0)) {
        state = NETOPT_STATE_RX;
    }
    else {
        puts("usage: ifconfig <if_id> set state [off|sleep|idle|reset|rx]");
        return 1;
    }
    if (gnrc_netapi_set(dev, NETOPT_STATE, 0,
                        &state, sizeof(netopt_state_t)) < 0) {
        printf("error: unable to set state to ");
        _print_netopt_state(state);
        puts("");
        return 1;
    }
    printf("success: set state of interface %" PRIkernel_pid " to ", dev);
    _print_netopt_state(state);
    puts("");

    return 0;
}

static int _sx127x_set_bandwidth(kernel_pid_t dev, char *bw_str)
{
    uint8_t bw;
    if (strcmp("125", bw_str) == 0) {
        bw = SX127X_BW_125_KHZ;
    }
    else if (strcmp("250", bw_str) == 0) {
        bw = SX127X_BW_250_KHZ;
    }
    else if (strcmp("500", bw_str) == 0) {
        bw = SX127X_BW_500_KHZ;
    }
    else {
        puts("usage: ifconfig <if_id> set bandwidth [125|250|500]");
        return 1;
    }
    if (gnrc_netapi_set(dev, NETOPT_BANDWIDTH, 0,
                        &bw, sizeof(uint8_t)) < 0) {
        printf("error: unable to set bandwidth to ");
        _print_netopt_bandwidth(bw);
        puts("");
        return 1;
    }
    printf("success: set bandwidth of interface %" PRIkernel_pid " to ", dev);
    _print_netopt_bandwidth(bw);
    puts("");

    return 0;
}

static int _sx127x_set_cr(kernel_pid_t dev, char *cr_str)
{
    uint8_t cr;
    if (strcmp("5", cr_str) == 0) {
        cr = SX127X_CR_4_5;
    }
    else if (strcmp("6", cr_str) == 0) {
        cr = SX127X_CR_4_6;
    }
    else if (strcmp("7", cr_str) == 0) {
        cr = SX127X_CR_4_7;
    }
    else if (strcmp("8", cr_str) == 0) {
        cr = SX127X_CR_4_8;
    }
    else {
        puts("usage: ifconfig <if_id> set coding_rate [5|6|7|8]");
        return 1;
    }
    if (gnrc_netapi_set(dev, NETOPT_CODING_RATE, 0,
                        &cr, sizeof(uint8_t)) < 0) {
        printf("error: unable to set coding rate to ");
        _print_netopt_cr(cr);
        puts("");
        return 1;
    }
    printf("success: set coding rate of interface %" PRIkernel_pid " to ", dev);
    _print_netopt_cr(cr);
    puts("");

    return 0;
}

static int _sx127x_set(char *cmd_name, kernel_pid_t dev, char *key, char *value)
{
    if ((strcmp("channel", key) == 0) || (strcmp("chan", key) == 0)) {
        return _sx127x_set_u32(dev, NETOPT_CHANNEL, value);
    }
    else if ((strcmp("bandwidth", key) == 0) || (strcmp("bw", key) == 0)) {
        return _sx127x_set_bandwidth(dev, value);
    }
    else if ((strcmp("spreading_factor", key) == 0) ||
             (strcmp("sf", key) == 0)) {
        return _sx127x_set_u8(dev, NETOPT_SPREADING_FACTOR, value);
    }
    else if ((strcmp("coding_rate", key) == 0) ||
             (strcmp("cr", key) == 0)) {
        return _sx127x_set_cr(dev, value);
    }
    else if (strcmp("power", key) == 0) {
        return _sx127x_set_u8(dev, NETOPT_TX_POWER, value);
    }
    else if (strcmp("rx_mode", key) == 0) {
        if (strcmp(value, "single")) {
            return _sx127x_set_flag(dev, NETOPT_SINGLE_RECEIVE, NETOPT_ENABLE);
        }
        else if (strcmp(value, "continuous") == 0) {
            return _sx127x_set_flag(dev, NETOPT_SINGLE_RECEIVE, NETOPT_DISABLE);
        }
        else {
            printf("error: invalid option '%s'\n", value);
            return 1;
        }
    }
    else if (strcmp("inverted_iq", key) == 0) {
        if (strcmp(value, "on") == 0) {
            return _sx127x_set_flag(dev, NETOPT_IQ_INVERT, NETOPT_ENABLE);
        }
        else if (strcmp(value, "off") == 0) {
            return _sx127x_set_flag(dev, NETOPT_IQ_INVERT, NETOPT_DISABLE);
        }
        else {
            printf("error: invalid option '%s'\n", value);
            return 1;
        }
    }
    else if (strcmp("state", key) == 0) {
        return _sx127x_set_state(dev, value);
    }

    _set_usage(cmd_name);
    return 1;
}

/* shell commands */
int _sx127x_lora(int argc, char **argv)
{
    kernel_pid_t dev;
    gnrc_pktsnip_t *pkt;

    if (argc < 3) {
        printf("usage: %s <if> <send|listen>\n", argv[0]);
        return 1;
    }

    /* parse interface */
    dev = atoi(argv[1]);

    if (!_is_iface(dev)) {
        puts("error: invalid interface given");
        return 1;
    }

    if (strcmp(argv[2], "listen") == 0) {
        /* Switch to RX continuous mode */
        _sx127x_set_flag(dev, NETOPT_SINGLE_RECEIVE, NETOPT_DISABLE);

        /* Switch to RX state */
        _sx127x_set_state(dev, "RX");
    }
    else if (strcmp(argv[2], "send") == 0) {
        if (argc < 4) {
            printf("usage: %s <if> send <data>\n", argv[0]);
            return 1;
        }
        /* put packet together */
        pkt = gnrc_pktbuf_add(NULL, argv[3], strlen(argv[3]), GNRC_NETTYPE_UNDEF);
        if (pkt == NULL) {
            puts("error: packet buffer full");
            return 1;
        }

        /* and send it */
        if (gnrc_netapi_send(dev, pkt) < 1) {
            puts("error: unable to send");
            gnrc_pktbuf_release(pkt);
            return 1;
        }

        return 0;
    }
    else {
        puts("error: invalid subcommand given");
        return 1;
    }

    return 0;
}

int _sx127x_config(int argc, char **argv)
{
    if (argc < 2) {
        kernel_pid_t ifs[GNRC_NETIF_NUMOF];
        size_t numof = gnrc_netif_get(ifs);

        for (size_t i = 0; i < numof && i < GNRC_NETIF_NUMOF; i++) {
            _sx127x_list(ifs[i]);
        }

        return 0;
    }
    else if (_is_number(argv[1])) {
        kernel_pid_t dev = atoi(argv[1]);

        if (_is_iface(dev)) {
            if (argc < 3) {
                _sx127x_list(dev);
                return 0;
            }
            else if (strcmp(argv[2], "set") == 0) {
                if (argc < 5) {
                    _set_usage(argv[0]);
                    return 1;
                }
                return _sx127x_set(argv[0], dev, argv[3], argv[4]);
            }
        }
        else {
            puts("error: invalid interface given");
            return 1;
        }
    }

    printf("usage: %s [<if_id>]\n", argv[0]);
    _set_usage(argv[0]);
    return 1;
}
