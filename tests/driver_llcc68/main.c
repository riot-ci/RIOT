/*
 * Copyright (C) 2021 Inria
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
 * @brief       Test application for the llcc68 radio driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "msg.h"
#include "thread.h"
#include "shell.h"
#include "xtimer.h"

#include "net/lora.h"
#include "net/netdev.h"
#include "net/netdev/lora.h"

#include "llcc68.h"
#include "llcc68_params.h"
#include "llcc68_netdev.h"

#define LLCC68_MSG_QUEUE        (8U)
#define LLCC68_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)
#define LLCC68_MSG_TYPE_ISR     (0x3456)
#define LLCC68_MAX_PAYLOAD_LEN  (128U)

static char stack[LLCC68_STACKSIZE];
static kernel_pid_t _recv_pid;

static char message[LLCC68_MAX_PAYLOAD_LEN];

static llcc68_t llcc68;

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;
        msg.type = LLCC68_MSG_TYPE_ISR;
        if (msg_send(&msg, _recv_pid) <= 0) {
            puts("llcc68_netdev: possibly lost interrupt.");
        }
    }
    else {
        switch (event) {
            case NETDEV_EVENT_RX_STARTED:
                puts("Data reception started");
                break;

            case NETDEV_EVENT_RX_COMPLETE:
                {
                    size_t len = dev->driver->recv(dev, NULL, 0, 0);
                    netdev_lora_rx_info_t packet_info;
                    dev->driver->recv(dev, message, len, &packet_info);
                    printf(
                        "Received: \"%s\" (%d bytes) - [RSSI: %i, SNR: %i]\n",
                        message, (int)len,
                        packet_info.rssi, (int)packet_info.snr
                    );
                    netopt_state_t state = NETOPT_STATE_RX;
                    dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
                }
                break;

            case NETDEV_EVENT_TX_COMPLETE:
                puts("Transmission completed");
                break;

            case NETDEV_EVENT_TX_TIMEOUT:
                puts("Transmission timeout");
                break;

            default:
                printf("Unexpected netdev event received: %d\n", event);
                break;
        }
    }
}

void *_recv_thread(void *arg)
{
    netdev_t *netdev = (netdev_t *)arg;

    static msg_t _msg_queue[LLCC68_MSG_QUEUE];
    msg_init_queue(_msg_queue, LLCC68_MSG_QUEUE);

    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == LLCC68_MSG_TYPE_ISR) {
            netdev->driver->isr(netdev);
        }
        else {
            puts("Unexpected msg type");
        }
    }
}

static void _get_usage(const char *cmd)
{
    printf("Usage: %s get <type|freq|bw|sf|cr|random>\n", cmd);
}

static int llcc68_get_cmd(netdev_t *netdev, int argc, char **argv)
{
    if (argc == 2) {
        _get_usage(argv[0]);
        return -1;
    }

    if (!strcmp("type", argv[2])) {
        uint16_t type;
        netdev->driver->get(netdev, NETOPT_DEVICE_TYPE, &type, sizeof(uint16_t));
        printf("Device type: %s\n", (type == NETDEV_TYPE_LORA) ? "lora" : "fsk");
    }
    else if (!strcmp("freq", argv[2])) {
        uint32_t freq;
        netdev->driver->get(netdev, NETOPT_CHANNEL_FREQUENCY, &freq, sizeof(uint32_t));
        printf("Frequency: %" PRIu32 "Hz\n", freq);
    }
    else if (!strcmp("bw", argv[2])) {
        uint8_t bw;
        netdev->driver->get(netdev, NETOPT_BANDWIDTH, &bw, sizeof(uint8_t));
        uint16_t bw_val = 0;
        switch (bw) {
            case LORA_BW_125_KHZ:
                bw_val = 125;
                break;
            case LORA_BW_250_KHZ:
                bw_val = 250;
                break;
            case LORA_BW_500_KHZ:
                bw_val = 500;
                break;
            default:
                break;
        }
        printf("Bandwidth: %ukHz\n", bw_val);
    }
    else if (!strcmp("sf", argv[2])) {
        uint8_t sf;
        netdev->driver->get(netdev, NETOPT_SPREADING_FACTOR, &sf, sizeof(uint8_t));
        printf("Spreading factor: %d\n", sf);
    }
    else if (!strcmp("cr", argv[2])) {
        uint8_t cr;
        netdev->driver->get(netdev, NETOPT_CODING_RATE, &cr, sizeof(uint8_t));
        printf("Coding rate: %d\n", cr);
    }
    else if (!strcmp("random", argv[2])) {
        uint32_t rand;
        netdev->driver->get(netdev, NETOPT_RANDOM, &rand, sizeof(uint32_t));
        printf("random number: %"PRIu32"\n", rand);
    }
    else {
        _get_usage(argv[0]);
        return -1;
    }

    return 0;
}

static void _set_usage(const char *cmd)
{
    printf("Usage: %s set <freq|bw|sf|cr> <value>\n", cmd);
}

static int llcc68_set_cmd(netdev_t *netdev, int argc, char **argv)
{
    if (argc != 4) {
        _set_usage(argv[0]);
        return -1;
    }

    int ret = 0;
    if (!strcmp("freq", argv[2])) {
        uint32_t freq = atoi(argv[3]);
        ret = netdev->driver->set(netdev, NETOPT_CHANNEL_FREQUENCY, &freq, sizeof(uint32_t));
    }
    else if (!strcmp("bw", argv[2])) {
        uint8_t bw;
        if (!strcmp("125", argv[3])) {
            bw = LORA_BW_125_KHZ;
        }
        else if (!strcmp("250", argv[3])) {
            bw = LORA_BW_250_KHZ;
        }
        else if (!strcmp("500", argv[3])) {
            bw = LORA_BW_500_KHZ;
        }
        else {
            puts("invalid bandwith, use 125, 250 or 500");
            return -1;
        }
        ret = netdev->driver->set(netdev, NETOPT_BANDWIDTH, &bw, sizeof(uint8_t));
    }
    else if (!strcmp("sf", argv[2])) {
        uint8_t sf = atoi(argv[3]);
        ret = netdev->driver->set(netdev, NETOPT_SPREADING_FACTOR, &sf, sizeof(uint8_t));
    }
    else if (!strcmp("cr", argv[2])) {
        uint8_t cr = atoi(argv[3]);;
        ret = netdev->driver->set(netdev, NETOPT_CODING_RATE, &cr, sizeof(uint8_t));
    }
    else {
        _set_usage(argv[0]);
        return -1;
    }

    if (ret < 0) {
        printf("cannot set %s\n", argv[2]);
        return ret;
    }

    printf("%s set\n", argv[2]);
    return 0;
}

static void _rx_usage(const char *cmd)
{
    printf("Usage: %s rx <start|stop>\n", cmd);
}

static int llcc68_rx_cmd(netdev_t *netdev, int argc, char **argv)
{
    if (argc == 2) {
        _rx_usage(argv[0]);
        return -1;
    }

    if (!strcmp("start", argv[2])) {
        /* Switch to RX state */
        netopt_state_t state = NETOPT_STATE_IDLE;
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));
        printf("Listen mode started\n");
    }
    else if (!strcmp("stop", argv[2])) {
        /* Switch to RX state */
        netopt_state_t state = NETOPT_STATE_STANDBY;
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));
        printf("Listen mode stopped\n");
    }
    else {
        _rx_usage(argv[0]);
        return -1;
    }

    return 0;
}

static int llcc68_tx_cmd(netdev_t *netdev, int argc, char **argv)
{
    if (argc == 2) {
        printf("Usage: %s tx <payload>\n", argv[0]);
        return -1;
    }

    printf("sending \"%s\" payload (%u bytes)\n",
            argv[2], (unsigned)strlen(argv[2]) + 1);
    iolist_t iolist = {
        .iol_base = argv[2],
        .iol_len = (strlen(argv[2]) + 1)
    };

    if (netdev->driver->send(netdev, &iolist) == -ENOTSUP) {
        puts("Cannot send: radio is still transmitting");
        return -1;
    }

    return 0;
}

int llcc68_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <get|set|rx|tx>\n", argv[0]);
        return -1;
    }

    netdev_t *netdev = (netdev_t *)&llcc68;
    if (!strcmp("get", argv[1])) {
        return llcc68_get_cmd(netdev, argc, argv);
    }
    else if (!strcmp("set", argv[1])) {
        return llcc68_set_cmd(netdev, argc, argv);
    }
    else if (!strcmp("rx", argv[1])) {
        return llcc68_rx_cmd(netdev, argc, argv);
    }
    else if (!strcmp("tx", argv[1])) {
        return llcc68_tx_cmd(netdev, argc, argv);
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "llcc68", "Control the LLCC68 radio",     llcc68_cmd},
    { NULL, NULL, NULL }
};

int main(void)
{
    llcc68_setup(&llcc68, &llcc68_params[0], 0);
    netdev_t *netdev = (netdev_t *)&llcc68;
    netdev->driver = &llcc68_driver;

    if (netdev->driver->init(netdev) < 0) {
        puts("Failed to initialize LLCC68 device, exiting");
        return 1;
    }

    netdev->event_callback = _event_cb;

    _recv_pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                              THREAD_CREATE_STACKTEST, _recv_thread, netdev,
                              "recv_thread");

    if (_recv_pid <= KERNEL_PID_UNDEF) {
        puts("Creation of receiver thread failed");
        return 1;
    }

    /* start the shell */
    puts("Initialization successful - starting the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
