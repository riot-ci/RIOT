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
 * @brief       Tests extension header handling of gnrc stack.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * @}
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "byteorder.h"
#include "embUnit.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/ext/frag.h"
#include "net/protnum.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/ext/frag.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc/udp.h"
#include "net/netdev_test.h"
#include "od.h"
#include "random.h"
#include "shell.h"
#include "xtimer.h"

#define TEST_SAMPLE         "This is a test. Failure might sometimes be an " \
                            "option, but not today. "
#define TEST_PORT           (20908U)

extern int udp_cmd(int argc, char **argv);
/* shell_test_cmd is used to test weird snip configurations,
 * the rest can just use udp_cmd */
static int shell_test_cmd(int argc, char **argv);

static netdev_test_t mock_netdev;
static gnrc_netif_t *eth_netif, *mock_netif;
static ipv6_addr_t *local_addr;
static char mock_netif_stack[THREAD_STACKSIZE_DEFAULT];
static char line_buf[SHELL_DEFAULT_BUFSIZE];
static const shell_command_t shell_commands[] = {
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
    { "test", "sends data according to a specified numeric test", shell_test_cmd },
    { NULL, NULL, NULL }
};

static gnrc_pktsnip_t *_build_udp_packet(const ipv6_addr_t *dst,
                                         unsigned payload_size,
                                         gnrc_pktsnip_t *payload)
{
    udp_hdr_t *udp_hdr;
    ipv6_hdr_t *ipv6_hdr;
    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *hdr;

    if (payload == NULL) {
        uint8_t *data;

        payload = gnrc_pktbuf_add(NULL, NULL, payload_size, GNRC_NETTYPE_UNDEF);
        if (payload == NULL) {
            return NULL;
        }
        data = payload->data;
        while (payload_size) {
            unsigned test_sample_len = sizeof(TEST_SAMPLE) - 1;

            if (test_sample_len > payload_size) {
                test_sample_len = payload_size;
            }

            memcpy(data, TEST_SAMPLE, test_sample_len);
            data += test_sample_len;
            payload_size -= test_sample_len;
        }
    }
    hdr = gnrc_udp_hdr_build(payload, TEST_PORT, TEST_PORT);
    if (hdr == NULL) {
        gnrc_pktbuf_release(payload);
        return NULL;
    }
    udp_hdr = hdr->data;
    udp_hdr->length = byteorder_htons(gnrc_pkt_len(hdr));
    payload = hdr;
    hdr = gnrc_ipv6_hdr_build(payload, local_addr, dst);
    if (hdr == NULL) {
        gnrc_pktbuf_release(payload);
        return NULL;
    }
    ipv6_hdr = hdr->data;
    ipv6_hdr->len = byteorder_htons(gnrc_pkt_len(payload));
    ipv6_hdr->nh = PROTNUM_UDP;
    ipv6_hdr->hl = GNRC_NETIF_DEFAULT_HL;
    gnrc_udp_calc_csum(payload, hdr);
    payload = hdr;
    hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    if (hdr == NULL) {
        gnrc_pktbuf_release(payload);
        return NULL;
    }
    netif_hdr = hdr->data;
    netif_hdr->if_pid = eth_netif->pid;
    netif_hdr->flags |= GNRC_NETIF_HDR_FLAGS_MULTICAST;
    hdr->next = payload;
    return hdr;
}

static void test_ipv6_ext_frag_send_pkt_single_frag(const ipv6_addr_t *dst)
{
    gnrc_pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL(local_addr);
    pkt = _build_udp_packet(dst, sizeof(TEST_SAMPLE) - 1, NULL);
    TEST_ASSERT_NOT_NULL(pkt);
    gnrc_ipv6_ext_frag_send_pkt(pkt, eth_netif->ipv6.mtu);
}

static void test_ipv6_ext_frag_payload_snips_not_divisible_of_8(const ipv6_addr_t *dst)
{
    gnrc_pktsnip_t *pkt, *payload = NULL;
    unsigned payload_size = 0;

    TEST_ASSERT_NOT_NULL(local_addr);
    /* TEST_SAMPLE's string length is not a multiple of 8*/
    TEST_ASSERT((sizeof(TEST_SAMPLE) - 1) & 0x7);

    while (payload_size <= eth_netif->ipv6.mtu) {
        pkt = gnrc_pktbuf_add(payload, TEST_SAMPLE, sizeof(TEST_SAMPLE) - 1,
                              GNRC_NETTYPE_UNDEF);
        TEST_ASSERT_NOT_NULL(pkt);
        payload_size += pkt->size;
        payload = pkt;
    }
    pkt = _build_udp_packet(dst, 0, payload);
    TEST_ASSERT_NOT_NULL(pkt);
    gnrc_ipv6_ext_frag_send_pkt(pkt, eth_netif->ipv6.mtu);
}

static int shell_test_cmd(int argc, char **argv)
{
    static ipv6_addr_t dst;
    static void (* const _shell_tests[])(const ipv6_addr_t *) = {
        test_ipv6_ext_frag_send_pkt_single_frag,
        test_ipv6_ext_frag_payload_snips_not_divisible_of_8,
    };
    int test_num;

    if ((argc < 3) || (ipv6_addr_from_str(&dst, argv[1]) == NULL)) {
        puts("usage: test <dst_addr> [<num>]");
        return 1;
    }
    test_num = atoi(argv[2]);
    if ((unsigned)test_num >=
        (sizeof(_shell_tests) / sizeof(_shell_tests[0]))) {
        printf("<num> must be between 0 and %u\n",
               (unsigned)(sizeof(_shell_tests) / sizeof(_shell_tests[0])) - 1);
        return 1;
    }
    printf("Running test %d\n", test_num);
    _shell_tests[test_num](&dst);
    return 0;
}

/* TODO: test if forwarded packet is not fragmented */

static int mock_get_device_type(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    assert(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = NETDEV_TYPE_TEST;
    return sizeof(uint16_t);
}

static int mock_get_max_packet_size(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    assert(max_len == sizeof(uint16_t));
    assert(eth_netif != NULL);
    *((uint16_t *)value) = eth_netif->ipv6.mtu - 8;
    return sizeof(uint16_t);
}

static int mock_send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev;
    int res = 0;
    while(iolist != NULL) {
        od_hex_dump(iolist->iol_base, iolist->iol_len,
                    OD_WIDTH_DEFAULT);
        res += iolist->iol_len;
        iolist = iolist->iol_next;
    }
    return res;
}

int main(void)
{
    eth_netif = gnrc_netif_iter(NULL);
    /* create mock netif to test forwarding too large fragments */
    netdev_test_setup(&mock_netdev, 0);
    netdev_test_set_get_cb(&mock_netdev, NETOPT_DEVICE_TYPE,
                            mock_get_device_type);
    netdev_test_set_get_cb(&mock_netdev, NETOPT_MAX_PDU_SIZE,
                           mock_get_max_packet_size);
    netdev_test_set_send_cb(&mock_netdev, mock_send);
    mock_netif = gnrc_netif_raw_create(mock_netif_stack,
                                       sizeof(mock_netif_stack),
                                       GNRC_NETIF_PRIO, "mock_netif",
                                       (netdev_t *)&mock_netdev);
    printf("Sending UDP test packets to port %u\n", TEST_PORT);
    for (unsigned i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        if (ipv6_addr_is_link_local(&eth_netif->ipv6.addrs[i])) {
            local_addr = &eth_netif->ipv6.addrs[i];
        }
    }
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
