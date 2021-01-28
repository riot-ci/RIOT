/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Text application for gnrc_tx_sync
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

/* keep include of stdint.h before stdatomic.h for compatibility with broken
 * toolchains */
#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>

#include "net/af.h"
#include "net/gnrc/netif/raw.h"
#include "net/ipv6/addr.h"
#include "net/netdev_test.h"
#include "net/sock/udp.h"
#include "test_utils/expect.h"
#include "xtimer.h"

#define NETIF_STACKSIZE     THREAD_STACKSIZE_DEFAULT
#define NETIF_PRIO          (THREAD_PRIORITY_MAIN - 4)
#define MAIN_QUEUE_SIZE     (8)

static char netif_stack[NETIF_STACKSIZE];
static msg_t main_msg_queue[MAIN_QUEUE_SIZE];

static atomic_int sends_completed = ATOMIC_VAR_INIT(0);
static gnrc_netif_t netif;
static netdev_test_t netdev;

static int netdev_send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev; (void)iolist;
    xtimer_msleep(100);
    xtimer_msleep(100);
    atomic_fetch_add(&sends_completed, 1);
    return iolist_size(iolist);
}

static int netdev_get_device_type(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    const uint16_t type_ipv6 = NETDEV_TYPE_TEST;
    const uint16_t type_6lo = NETDEV_TYPE_TEST_6LO;
    assert(max_len == sizeof(uint16_t));
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG)) {
        memcpy(value, &type_6lo, sizeof(type_6lo));
    }
    else {
        memcpy(value, &type_ipv6, sizeof(type_ipv6));
    }
    return sizeof(uint16_t);
}

static int netdev_get_max_pdu_size(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    const uint16_t pdu_size_ipv6 = 1280;
    const uint16_t pdu_size_6lo = 32;
    assert(max_len == sizeof(uint16_t));
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG)) {
        memcpy(value, &pdu_size_6lo, sizeof(pdu_size_6lo));
    }
    else {
        memcpy(value, &pdu_size_ipv6, sizeof(pdu_size_ipv6));
    }
    return sizeof(uint16_t);
}

static int netdev_get_proto(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    const uint16_t proto_ipv6 = GNRC_NETTYPE_IPV6;
    const uint16_t proto_6lo = GNRC_NETTYPE_SIXLOWPAN;
    assert(max_len == sizeof(uint16_t));
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG)) {
        memcpy(value, &proto_6lo, sizeof(proto_6lo));
    }
    else {
        memcpy(value, &proto_ipv6, sizeof(proto_ipv6));
    }
    return sizeof(uint16_t);
}

int main(void)
{
    /* With 6LoWPAN, This test message needs exactly two 32 byte fragments to be transmitted */
    static const char test_msg[33] = { 'T', 'e', 's', 't' };
    msg_init_queue(main_msg_queue, MAIN_QUEUE_SIZE);
    netdev_test_setup(&netdev, NULL);
    netdev_test_set_send_cb(&netdev, netdev_send);
    netdev_test_set_get_cb(&netdev, NETOPT_DEVICE_TYPE, netdev_get_device_type);
    netdev_test_set_get_cb(&netdev, NETOPT_MAX_PDU_SIZE, netdev_get_max_pdu_size);
    netdev_test_set_get_cb(&netdev, NETOPT_PROTO, netdev_get_proto);
    gnrc_netif_raw_create(&netif, netif_stack, sizeof(netif_stack), NETIF_PRIO,
                          "netdev_test", &netdev.netdev);

    sock_udp_t sock;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = { .family = AF_INET6 };
    remote.port = 12345;
    ipv6_addr_set_all_nodes_multicast((ipv6_addr_t *)&remote.addr.ipv6,
                                      IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
    expect(sock_udp_create(&sock, &local, NULL, 0) == 0);
    expect(sock_udp_send(&sock, test_msg, sizeof(test_msg), &remote) > 0);

    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG)) {
        /* with 6LoWPAN fragmentation, we send two fragments, otherwise the no link layer
         * fragmentation is used and only a single transmission should occur */
        expect(atomic_load(&sends_completed) == 2);
    }
    else {
        expect(atomic_load(&sends_completed) == 1);
    }

    puts("TEST PASSED");

    return 0;
}
