/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       Tests 6LoWPAN fragmentation handling of gnrc stack.
 *
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include "embUnit.h"
#include "net/gnrc.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc/sixlowpan.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/netdev_test.h"
#include "thread.h"

#define TEST_DST    { 0x5a, 0x9d, 0x93, 0x86, 0x22, 0x08, 0x65, 0x79 }
#define TEST_SRC    { 0x2a, 0xab, 0xdc, 0x15, 0x54, 0x01, 0x64, 0x79 }
#define TEST_6LO_PAYLOAD { \
        /* 6LoWPAN, Src: 2001:db8::1, Dest: 2001:db8::2
         *    Fragmentation Header
         *        1100 0... = Pattern: First fragment (0x18)
         *        Datagram size: 188
         *        Datagram tag: 0x000f
         *    IPHC Header
         *        011. .... = Pattern: IP header compression (0x03)
         *        ...1 1... .... .... = Version, traffic class, and flow label compressed (0x3)
         *        .... .0.. .... .... = Next header: Inline
         *        .... ..10 .... .... = Hop limit: 64 (0x2)
         *        .... .... 0... .... = Context identifier extension: False
         *        .... .... .0.. .... = Source address compression: Stateless
         *        .... .... ..00 .... = Source address mode: Inline (0x0000)
         *        .... .... .... 0... = Multicast address compression: False
         *        .... .... .... .0.. = Destination address compression: Stateless
         *        .... .... .... ..00 = Destination address mode: Inline (0x0000)
         *    Next header: ICMPv6 (0x3a)
         *    Source: 2001:db8::1
         *    Destination: 2001:db8::2 */ \
        0xc0, 0xbc, 0x00, 0x0f, \
        /*    IPHC Header
         *        011. .... = Pattern: IP header compression (0x03)
         *        ...1 1... .... .... = Version, traffic class, and flow label compressed (0x3)
         *        .... .0.. .... .... = Next header: Inline
         *        .... ..10 .... .... = Hop limit: 64 (0x2)
         *        .... .... 0... .... = Context identifier extension: False
         *        .... .... .0.. .... = Source address compression: Stateless
         *        .... .... ..00 .... = Source address mode: Inline (0x0000)
         *        .... .... .... 0... = Multicast address compression: False
         *        .... .... .... .0.. = Destination address compression: Stateless
         *        .... .... .... ..00 = Destination address mode: Inline (0x0000)
         *    Next header: ICMPv6 (0x3a) */ \
        0x7a, 0x00, 0x3a, \
        /*    Source: 2001:db8::1 */ \
        0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, \
        /*    Destination: 2001:db8::2 */ \
        0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, \
        /* Internet Control Message Protocol v6
         *    Type: Echo (ping) request (128)
         *    Code: 0
         *    Checksum: 0x8ea0
         *    Identifier: 0x238f
         *    Sequence: 2
         *    [No response seen]
         *    Data (140 bytes)
         *        Data: 9d4bb21c5353535353535353535353535353535353535353…
         */ \
        0x80, 0x00, 0x8e, 0xa0, 0x23, 0x8f, 0x00, 0x02, \
        0x9d, 0x4b, 0xb2, 0x1c, 0x53, 0x53, 0x53, 0x53, \
        0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, \
        0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, \
        0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, \
        0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, \
        0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, \
    }
#define TEST_TGT_IPV6    { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                           0x48, 0x3d, 0x1d, 0x0c, 0x98, 0x31, 0x58, 0xae }

const uint8_t _test_src[] = TEST_SRC;
const uint8_t _test_dst[] = TEST_DST;
const uint8_t _test_6lo_payload[] = TEST_6LO_PAYLOAD;
const ipv6_addr_t _test_tgt_ipv6 = { .u8 = TEST_TGT_IPV6 };

static char _mock_netif_stack[THREAD_STACKSIZE_DEFAULT];
static netdev_test_t _mock_dev;
static gnrc_netif_t *_mock_netif;


void _set_up(void)
{
    /* Add default route for the VRB entry created from */
    gnrc_ipv6_nib_ft_add(NULL, 0, &_test_tgt_ipv6, _mock_netif->pid, 0);
}

void _tear_down(void)
{
    gnrc_ipv6_nib_ft_del(NULL, 0);
}

gnrc_pktsnip_t *_create_fragment(void)
{
    gnrc_pktsnip_t *res = gnrc_netif_hdr_build(_test_src, sizeof(_test_src),
                                               _test_dst, sizeof(_test_dst));
    if (res == NULL) {
        return NULL;
    }
    gnrc_netif_hdr_set_netif(res->data, _mock_netif);
    res = gnrc_pktbuf_add(res, _test_6lo_payload, sizeof(_test_6lo_payload),
                          GNRC_NETTYPE_SIXLOWPAN);
    return res;
}

void test_recv__success(void)
{
    gnrc_pktsnip_t *pkt = _create_fragment();

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(
            1, gnrc_netapi_dispatch_receive(GNRC_NETTYPE_SIXLOWPAN,
                                            GNRC_NETREG_DEMUX_CTX_ALL,
                                            pkt)
        );
    thread_yield_higher();
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void run_unittests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_recv__success),
    };

    EMB_UNIT_TESTCALLER(sixlo_iphc_vrb_tests, _set_up, _tear_down, fixtures);
    TESTS_START();
    TESTS_RUN((Test *)&sixlo_iphc_vrb_tests);
    TESTS_END();
}

static int _get_netdev_device_type(netdev_t *netdev, void *value, size_t max_len)
{
    assert(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
    return sizeof(uint16_t);
}

static int _get_netdev_proto(netdev_t *netdev, void *value, size_t max_len)
{
    assert(max_len == sizeof(gnrc_nettype_t));
    (void)netdev;

    *((gnrc_nettype_t *)value) = GNRC_NETTYPE_SIXLOWPAN;
    return sizeof(gnrc_nettype_t);
}

static int _get_netdev_max_pdu_size(netdev_t *netdev, void *value,
                                    size_t max_len)
{
    assert(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = sizeof(_test_6lo_payload);
    return sizeof(uint16_t);
}

static int _get_netdev_src_len(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    assert(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = sizeof(_test_dst);
    return sizeof(uint16_t);
}

static int _get_netdev_addr_long(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    assert(max_len >= sizeof(_test_dst));
    memcpy(value, _test_dst, sizeof(_test_dst));
    return sizeof(_test_dst);
}

static void _init_mock_netif(void)
{
    netdev_test_setup(&_mock_dev, NULL);
    netdev_test_set_get_cb(&_mock_dev, NETOPT_DEVICE_TYPE,
                           _get_netdev_device_type);
    netdev_test_set_get_cb(&_mock_dev, NETOPT_PROTO,
                           _get_netdev_proto);
    netdev_test_set_get_cb(&_mock_dev, NETOPT_MAX_PDU_SIZE,
                           _get_netdev_max_pdu_size);
    netdev_test_set_get_cb(&_mock_dev, NETOPT_SRC_LEN,
                           _get_netdev_src_len);
    netdev_test_set_get_cb(&_mock_dev, NETOPT_ADDRESS_LONG,
                           _get_netdev_addr_long);
    _mock_netif = gnrc_netif_ieee802154_create(
            _mock_netif_stack, THREAD_STACKSIZE_DEFAULT, GNRC_NETIF_PRIO,
            "mock_netif", (netdev_t *)&_mock_dev);
    thread_yield_higher();
}

int main(void)
{
    _init_mock_netif();
    run_unittests();
    return 0;
}
