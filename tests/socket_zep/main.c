/*
 * Copyright (C) 2016 Freie Universität Berlin
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
 * @brief       Test application for socket_zep network device driver
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>

#include "byteorder.h"
#include "net/ieee802154.h"
#include "sched.h"
#include "socket_zep.h"
#include "socket_zep_params.h"
#include "msg.h"
#include "od.h"

#define MSG_QUEUE_SIZE  (8)
#define MSG_TYPE_ISR    (0x3456)
#define RECVBUF_SIZE    (IEEE802154_FRAME_LEN_MAX)

static uint8_t _recvbuf[RECVBUF_SIZE];
static msg_t _msg_queue[MSG_QUEUE_SIZE];
static socket_zep_t _dev;
static kernel_pid_t _main_pid;

static void _event_cb(netdev_t *dev, netdev_event_t event);
static void _print_info(netdev_t *netdev);

static void test_init(void)
{
    const socket_zep_params_t *p = &socket_zep_params[0];
    netdev_t *netdev = (netdev_t *)(&_dev);

    printf("Initializing socket ZEP with (local: [%s]:%s, remote: [%s]:%s)\n",
           p->local_addr, p->local_port, p->remote_addr, p->remote_port);
    socket_zep_setup(&_dev, p);
    netdev->event_callback = _event_cb;
    assert(netdev->driver->init(netdev) >= 0);
    _print_info(netdev);
}

static void test_send__vector_NULL__count_0(void)
{
    netdev_t *netdev = (netdev_t *)(&_dev);
    int res;

    puts("Send zero-length packet");
    res = netdev->driver->send(netdev, NULL, 0);
    assert((res < 0) || (res == 0));
    if ((res < 0) && (errno == ECONNREFUSED)) {
        puts("No remote socket exists (use scripts in `tests/` to have proper tests)");
    }
}

static void test_send__vector_not_NULL__count_2(void)
{
    struct iovec vector[] = { { .iov_base = "Hello", .iov_len = sizeof("Hello") },
                              { .iov_base = "World", .iov_len = sizeof("World") } };
    netdev_t *netdev = (netdev_t *)(&_dev);
    int res;

    puts("Send 'Hello\\0World\\0'");
    res =  netdev->driver->send(netdev, vector,
                                sizeof(vector) / sizeof(struct iovec));
    assert((res < 0) || (res == (sizeof("Hello")) + sizeof("World")));
    if ((res < 0) && (errno == ECONNREFUSED)) {
        puts("No remote socket exists (use scripts in `tests/` to have proper tests)");
    }
}

static void test_recv(void)
{
    puts("Waiting for an incoming message (use `make test`)");
    while (1) {
        netdev_t *netdev = (netdev_t *)(&_dev);
        msg_t msg;

        msg_receive(&msg);
        if (msg.type == MSG_TYPE_ISR) {
            netdev->driver->isr(netdev);
        }
        else {
            puts("unexpected message type");
        }
    }
}

int main(void)
{
    puts("Socket ZEP device driver test");
    msg_init_queue(_msg_queue, MSG_QUEUE_SIZE);
    _main_pid = sched_active_pid;

    test_init();
    test_send__vector_NULL__count_0();
    test_send__vector_not_NULL__count_2();
    test_recv();    /* does not return */
    puts("ALL TESTS SUCCESSFUL");
    return 0;
}

static void _recv(netdev_t *dev)
{
    netdev_ieee802154_rx_info_t rx_info;
    const int exp_len = dev->driver->recv(dev, NULL, 0, NULL);
    int data_len;

    assert(exp_len >= 0);
    assert(((unsigned)exp_len) <= sizeof(_recvbuf));
    data_len = dev->driver->recv(dev, _recvbuf, sizeof(_recvbuf), &rx_info);
    if (data_len < 0) {
        puts("Received invalid packet");
    }
    else {
        assert(data_len <= exp_len);
        printf("RSSI: %u, LQI: %u, Data:\n", rx_info.rssi, rx_info.lqi);
        if (data_len > 0) {
            od_hex_dump(_recvbuf, data_len, OD_WIDTH_DEFAULT);
        }
    }
}

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;

        msg.type = MSG_TYPE_ISR;
        msg.content.ptr = dev;

        if (msg_send(&msg, _main_pid) <= 0) {
            puts("possibly lost interrupt.");
        }
    }
    else {
        switch (event) {
            case NETDEV_EVENT_RX_COMPLETE:
            {
                _recv(dev);
                break;
            }
            default:
                break;
        }
    }
}

static void _print_info(netdev_t *netdev)
{
    uint64_t long_addr;
    uint16_t short_addr;

    assert(netdev->driver->get(netdev, NETOPT_ADDRESS, &short_addr,
                               sizeof(short_addr)) == sizeof(uint16_t));
    assert(netdev->driver->get(netdev, NETOPT_ADDRESS_LONG, &long_addr,
                               sizeof(long_addr)) == sizeof(uint64_t));

    /* we are on native, so using PRIu* is okay */
    printf("(Hwaddrs: %04" PRIx16 ", %016" PRIx64 ")\n",
           byteorder_htons(short_addr).u16,
           byteorder_htonll(long_addr).u64);
}
