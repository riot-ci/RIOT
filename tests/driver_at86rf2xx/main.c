/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Test application for AT86RF2xx network device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "net/netdev.h"
#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "xtimer.h"

#include "common.h"

#define _STACKSIZE      (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#define MSG_TYPE_ISR    (0x3456)

static char stack[_STACKSIZE];
static kernel_pid_t _recv_pid;

at86rf2xx_devs_t at86rf2xx_devs;

static const shell_command_t shell_commands[] = {
    { "ifconfig", "Configure netdev", ifconfig },
    { "txtsnd", "Send IEEE 802.15.4 packet", txtsnd },
    { NULL, NULL, NULL }
};

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;

        msg.type = MSG_TYPE_ISR;
        msg.content.ptr = dev;

        if (msg_send(&msg, _recv_pid) <= 0) {
            puts("gnrc_netdev: possibly lost interrupt.");
        }
    }
    else {
        switch (event) {
            case NETDEV_EVENT_RX_COMPLETE:
            {
                recv(dev);

                break;
            }
            default:
                puts("Unexpected event received");
                break;
        }
    }
}

void *_recv_thread(void *arg)
{
    (void)arg;
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == MSG_TYPE_ISR) {
            netdev_t *dev = msg.content.ptr;
            dev->driver->isr(dev);
        }
        else {
            puts("unexpected message type");
        }
    }
}

int main(void)
{
    puts("AT86RF2xx device driver test");
    xtimer_init();

    at86rf2xx_setup_devs(&at86rf2xx_devs);

    unsigned dev_success = 0;
    uint8_t *dev = at86rf2xx_devs.mem_devs;
    for (unsigned i = 0; i < AT86RF2XX_NUM; i++) {
        netopt_enable_t en = NETOPT_ENABLE;
        netdev_t *netdev = (netdev_t *)(&(((at86rf2xx_t *)dev)->base.netdev));
        dev += at86rf2xx_get_size((at86rf2xx_t *)dev);
        printf("Initializing AT86RF2xx radio #%u\n", i);
        netdev->event_callback = _event_cb;
        if (netdev->driver->init(netdev) < 0) {
            printf("radio #%u: initialization failed\n", i);
            continue;
        }
        printf("radio #%u: initialization successful\n", i);
        dev_success++;
        netdev->driver->set(netdev, NETOPT_RX_END_IRQ, &en, sizeof(en));
    }

    if (!dev_success) {
        puts("No device could be initialized");
        return 1;
    }

    _recv_pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                              THREAD_CREATE_STACKTEST, _recv_thread, NULL,
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
