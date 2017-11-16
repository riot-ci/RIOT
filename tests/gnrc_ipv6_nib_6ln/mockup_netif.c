/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "common.h"
#include "msg.h"
#include "net/gnrc.h"
#include "net/ethernet.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc/netif/internal.h"
#include "net/netdev_test.h"
#include "sched.h"
#include "thread.h"

#define _MSG_QUEUE_SIZE  (2)

gnrc_netif_t *_mock_netif = NULL;

static netdev_test_t _mock_netdev;
static char _mock_netif_stack[THREAD_STACKSIZE_DEFAULT];
static gnrc_netreg_entry_t dumper;
static msg_t _main_msg_queue[_MSG_QUEUE_SIZE];

void _common_set_up(void)
{
    assert(_mock_netif != NULL);
    gnrc_ipv6_nib_init();
    gnrc_netif_acquire(_mock_netif);
    gnrc_ipv6_nib_init_iface(_mock_netif);
    gnrc_netif_release(_mock_netif);
}

int _get_device_type(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    assert(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
    return sizeof(uint16_t);
}

int _get_max_packet_size(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    assert(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = 102U;
    return sizeof(uint16_t);
}

int _get_src_len(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    assert(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = IEEE802154_LONG_ADDRESS_LEN;
    return sizeof(uint16_t);
}

int _get_address_long(netdev_t *dev, void *value, size_t max_len)
{
    static const uint8_t addr[] = { _LL0, _LL1, _LL2, _LL3,
                                    _LL4, _LL5, _LL6, _LL7 };

    (void)dev;
    assert(max_len >= sizeof(addr));
    memcpy(value, addr, sizeof(addr));
    return sizeof(addr);
}

void _tests_init(void)
{
    msg_init_queue(_main_msg_queue, _MSG_QUEUE_SIZE);
    netdev_test_setup(&_mock_netdev, 0);
    netdev_test_set_get_cb(&_mock_netdev, NETOPT_DEVICE_TYPE,
                           _get_device_type);
    netdev_test_set_get_cb(&_mock_netdev, NETOPT_MAX_PACKET_SIZE,
                           _get_max_packet_size);
    netdev_test_set_get_cb(&_mock_netdev, NETOPT_SRC_LEN,
                           _get_src_len);
    netdev_test_set_get_cb(&_mock_netdev, NETOPT_ADDRESS_LONG,
                           _get_address_long);
    _mock_netif = gnrc_netif_ieee802154_create(
           _mock_netif_stack, THREAD_STACKSIZE_DEFAULT, GNRC_NETIF_PRIO,
            "mockup_wpan", &_mock_netdev.netdev.netdev
        );
    assert(_mock_netif != NULL);
    gnrc_netreg_entry_init_pid(&dumper, GNRC_NETREG_DEMUX_CTX_ALL,
                               sched_active_pid);
    gnrc_netreg_register(GNRC_NETTYPE_NDP2, &dumper);
}

/** @} */
