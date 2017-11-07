/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief       Tests default configuration of GNRC's Network Information Base
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "cib.h"
#include "common.h"
#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "net/ethernet.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/ndp.h"
#include "sched.h"

#define _BUFFER_SIZE    (128)

static const uint8_t _loc_l2[] = { _LL0, _LL1, _LL2, _LL3, _LL4, _LL5 };
static const ipv6_addr_t _loc_ll = { {
                0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            _LL0 ^ 2, _LL1, _LL2, 0xff, 0xfe, _LL3, _LL4, _LL5
        } };
static const ipv6_addr_t _loc_sol_nodes = { {
            0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0xff, _LL3, _LL4, _LL5
        } };
#define _loc_iid    _loc_ll.u64[1].u8
static const uint8_t _rem_l2[] = { _LL0, _LL1, _LL2, _LL3, _LL4, _LL5 + 1 };
static const ipv6_addr_t _rem_ll = { {
                0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            _LL0 ^ 2, _LL1, _LL2, 0xff, 0xfe, _LL3, _LL4, _LL5 + 1
        } };
#define _rem_iid    _rem_ll.u64[1].u8
static uint8_t _buffer[_BUFFER_SIZE];
static ipv6_hdr_t *ipv6 = (ipv6_hdr_t *)&_buffer[0];
static icmpv6_hdr_t *icmpv6 = (icmpv6_hdr_t *)&_buffer[sizeof(ipv6_hdr_t)];

static inline size_t ceil8(size_t size);

static void _set_up(void)
{
    _common_set_up();
    memset(_buffer, 0, sizeof(_buffer));
    gnrc_pktbuf_init();
    /* remove messages */
    while (msg_avail()) {
        msg_t msg;
        msg_receive(&msg);
    }
}

static void test_get_next_hop_l2addr__link_local_EHOSTUNREACH(gnrc_netif2_t *netif)
{
    msg_t msg;
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    gnrc_pktsnip_t *pkt;

    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH,
                          gnrc_ipv6_nib_get_next_hop_l2addr(&_rem_ll, netif,
                                                            NULL, &nce));
    if (netif != NULL) {
        ndp_nbr_sol_t *nbr_sol;
        bool contains_sl2ao = false;

        TEST_ASSERT_MESSAGE(gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                            "Expected neighbor cache entry");
        TEST_ASSERT_MESSAGE(ipv6_addr_equal(&_rem_ll, &nce.ipv6),
                            "_rem_ll != nce->ipv6");
        TEST_ASSERT_EQUAL_INT(0, nce.l2addr_len);
        TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE,
                              gnrc_ipv6_nib_nc_get_nud_state(&nce));
        TEST_ASSERT(!gnrc_ipv6_nib_nc_is_router(&nce));
        TEST_ASSERT_EQUAL_INT(netif->pid, gnrc_ipv6_nib_nc_get_iface(&nce));
        TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC,
                              gnrc_ipv6_nib_nc_get_ar_state(&nce));
        TEST_ASSERT_EQUAL_INT(1, msg_avail());
        msg_receive(&msg);
        TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
        pkt = msg.content.ptr;
        TEST_ASSERT_NOT_NULL(pkt->next);
        TEST_ASSERT_NOT_NULL(pkt->next->next);
        TEST_ASSERT_EQUAL_INT(sizeof(ndp_nbr_sol_t), pkt->next->next->size);
        nbr_sol = pkt->next->next->data;
        TEST_ASSERT_EQUAL_INT(ICMPV6_NBR_SOL, nbr_sol->type);
        TEST_ASSERT_NOT_NULL(pkt->next->next->next);
        for (gnrc_pktsnip_t *opt_snip = pkt->next->next->next; opt_snip != NULL;
             opt_snip = opt_snip->next) {
            ndp_opt_t *opt = opt_snip->data;
            if (opt->type == NDP_OPT_SL2A) {
                contains_sl2ao = true;
                TEST_ASSERT_EQUAL_INT(1U, opt->len);
                TEST_ASSERT_MESSAGE(memcmp(&_loc_l2, opt + 1,
                                           sizeof(_loc_l2)) == 0,
                                    "src_l2 != pkt->l2");
            }
        }
        TEST_ASSERT_MESSAGE(contains_sl2ao, "Sent NS does not contain SL2AO");
        gnrc_pktbuf_release(pkt);
        TEST_ASSERT(gnrc_pktbuf_is_empty());
    }
}

static void test_get_next_hop_l2addr__link_local_EHOSTUNREACH_no_iface(void)
{
    test_get_next_hop_l2addr__link_local_EHOSTUNREACH(NULL);
}

static void test_get_next_hop_l2addr__link_local_EHOSTUNREACH_iface(void)
{
    test_get_next_hop_l2addr__link_local_EHOSTUNREACH(_mock_netif);
}

static void test_get_next_hop_l2addr__link_local_static_conf(void)
{
    gnrc_ipv6_nib_nc_t nce;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&_rem_ll, _mock_netif->pid,
                                                  _rem_l2, sizeof(_rem_l2)));
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_get_next_hop_l2addr(&_rem_ll,
                                                               _mock_netif,
                                                               NULL, &nce));
    TEST_ASSERT_MESSAGE((memcmp(&_rem_ll, &nce.ipv6, sizeof(_rem_ll)) == 0),
                        "_rem_ll != nce.ipv6");
    TEST_ASSERT_EQUAL_INT(sizeof(_rem_l2), nce.l2addr_len);
    TEST_ASSERT_MESSAGE((memcmp(&_rem_l2, &nce.l2addr, nce.l2addr_len) == 0),
                        "_rem_l2 != nce.l2addr");
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
    TEST_ASSERT_EQUAL_INT(_mock_netif->pid, gnrc_ipv6_nib_nc_get_iface(&nce));
    TEST_ASSERT(!gnrc_ipv6_nib_nc_is_router(&nce));
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL,
                          gnrc_ipv6_nib_nc_get_ar_state(&nce));
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

void _simulate_ndp_handshake(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                             uint8_t adv_flags)
{
    msg_t msg;
    gnrc_ipv6_nib_nc_t nce;
    ndp_nbr_adv_t *nbr_adv = (ndp_nbr_adv_t *)icmpv6;
    ndp_opt_t *tl2ao = (ndp_opt_t *)&_buffer[sizeof(ipv6_hdr_t) +
                                             sizeof(ndp_nbr_adv_t)];

    /* trigger sending of neighbor discovery */
    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH,
                          gnrc_ipv6_nib_get_next_hop_l2addr(dst,
                                                            _mock_netif,
                                                            NULL, &nce));
    TEST_ASSERT_EQUAL_INT(1, msg_avail());
    /* clear message queue */
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    gnrc_pktbuf_release(msg.content.ptr);
    /* generate neighbor advertisement */
    ipv6_hdr_set_version(ipv6);
    ipv6->hl = 255U;
    /* this simulates a reply, so dst and src need to be switched */
    memcpy(&ipv6->src, dst, sizeof(ipv6->src));
    memcpy(&ipv6->dst, src, sizeof(ipv6->dst));
    nbr_adv->type = ICMPV6_NBR_ADV;
    /* checksum isn't checked by gnrc_ipv6_nib_handle_pkt() */
    nbr_adv->flags = adv_flags;
    memcpy(&nbr_adv->tgt, dst, sizeof(nbr_adv->tgt));
    tl2ao->type = NDP_OPT_TL2A;
    tl2ao->len = 1;
    memcpy(tl2ao + 1, _rem_l2, sizeof(_rem_l2));
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, (icmpv6_hdr_t *)nbr_adv,
                             sizeof(ndp_nbr_adv_t) + 8U);
}

static void test_get_next_hop_l2addr__link_local_after_handshake(uint8_t adv_flags)
{
    gnrc_ipv6_nib_nc_t nce;

    _simulate_ndp_handshake(&_loc_ll, &_rem_ll, adv_flags);
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_get_next_hop_l2addr(&_rem_ll,
                                                               _mock_netif,
                                                               NULL, &nce));
    TEST_ASSERT_MESSAGE((memcmp(&_rem_ll, &nce.ipv6, sizeof(_rem_ll)) == 0),
                        "_rem_ll != nce.ipv6");
    TEST_ASSERT_EQUAL_INT(sizeof(_rem_l2), nce.l2addr_len);
    TEST_ASSERT_MESSAGE((memcmp(&_rem_l2, &nce.l2addr, nce.l2addr_len) == 0),
                        "_rem_l2 != nce.l2addr");
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
    TEST_ASSERT_EQUAL_INT(_mock_netif->pid, gnrc_ipv6_nib_nc_get_iface(&nce));
    if (adv_flags & NDP_NBR_ADV_FLAGS_R) {
        TEST_ASSERT(gnrc_ipv6_nib_nc_is_router(&nce));
    }
    else {
        TEST_ASSERT(!gnrc_ipv6_nib_nc_is_router(&nce));
    }
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC,
                          gnrc_ipv6_nib_nc_get_ar_state(&nce));
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_get_next_hop_l2addr__link_local_after_handshake_iface(void)
{
    test_get_next_hop_l2addr__link_local_after_handshake(NDP_NBR_ADV_FLAGS_S);
}

static void test_get_next_hop_l2addr__link_local_after_handshake_iface_router(void)
{
    test_get_next_hop_l2addr__link_local_after_handshake(NDP_NBR_ADV_FLAGS_S |
                                                         NDP_NBR_ADV_FLAGS_R);
}

static void test_get_next_hop_l2addr__link_local_after_handshake_no_iface(void)
{
    gnrc_ipv6_nib_nc_t nce;

    _simulate_ndp_handshake(&_loc_ll, &_rem_ll, NDP_NBR_ADV_FLAGS_S);
    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH,
                          gnrc_ipv6_nib_get_next_hop_l2addr(&_rem_ll,
                                                            NULL,
                                                            NULL, &nce));
}

static void test_handle_pkt__unknown_type(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;

    ipv6_hdr_set_version(ipv6);
    ipv6->hl = 255U;
    memcpy(&ipv6->src, &_loc_ll, sizeof(ipv6->src));
    memcpy(&ipv6->dst, &_rem_ll, sizeof(ipv6->dst));
    icmpv6->type = ICMPV6_ECHO_REQ;
    icmpv6->code = 0;
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6,
                             sizeof(icmpv6_hdr_t));
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
}

static size_t _set_nbr_sol(const ipv6_addr_t *ipv6_src,
                           const ipv6_addr_t *ipv6_dst,
                           uint8_t ipv6_hl, uint8_t nbr_sol_code,
                           const ipv6_addr_t *nbr_sol_tgt,
                           const uint8_t *sl2ao_addr, size_t sl2ao_addr_len)
{
    size_t icmpv6_len = sizeof(ndp_nbr_sol_t);
    ndp_nbr_sol_t *nbr_sol = (ndp_nbr_sol_t *)icmpv6;

    ipv6_hdr_set_version(ipv6);
    ipv6->hl = ipv6_hl;
    memcpy(&ipv6->src, ipv6_src, sizeof(ipv6->src));
    memcpy(&ipv6->dst, ipv6_dst, sizeof(ipv6->dst));
    nbr_sol->type = ICMPV6_NBR_SOL;
    nbr_sol->code = nbr_sol_code;
    memcpy(&nbr_sol->tgt, nbr_sol_tgt, sizeof(nbr_sol->tgt));

    if ((sl2ao_addr != NULL) && (sl2ao_addr_len > 0)) {
        ndp_opt_t *sl2ao = (ndp_opt_t *)&_buffer[sizeof(ipv6_hdr_t) +
                                                 sizeof(ndp_nbr_sol_t)];

        sl2ao->type = NDP_OPT_SL2A;
        sl2ao->len = ceil8(sizeof(ndp_opt_t) + sl2ao_addr_len) / 8;
        memcpy(sl2ao + 1, sl2ao_addr, sl2ao_addr_len);
        icmpv6_len += ceil8(sizeof(ndp_opt_t) + sl2ao_addr_len);
    }

    return icmpv6_len;
}

static void test_handle_pkt__nbr_sol__invalid_hl(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes, 194U, 0U,
                                     &_loc_ll, _rem_l2, sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_code(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes, 255U, 201U,
                                     &_loc_ll, _rem_l2, sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_icmpv6_len(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;

    _set_nbr_sol(&_rem_ll, &_loc_sol_nodes, 255U, 0U, &_loc_ll, _rem_l2,
                 sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6,
                             sizeof(ndp_nbr_sol_t) - 1);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_tgt(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes, 255U, 0U,
                                     &ipv6_addr_all_routers_site_local, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_opt_len(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes, 255U, 0U,
                                     &_loc_ll, _rem_l2, sizeof(_rem_l2));
    ndp_opt_t *opt = (ndp_opt_t *)&_buffer[icmpv6_len];

    opt->type = NDP_OPT_SL2A;
    opt->len = 0U;
    icmpv6_len += sizeof(ndp_opt_t);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_dst(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&ipv6_addr_unspecified, &_loc_ll, 255U, 0U,
                                     &_loc_ll, NULL, 0);

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__invalid_sl2ao(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&ipv6_addr_unspecified, &_loc_sol_nodes,
                                     255U, 0U, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_sol__tgt_not_assigned(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes,
                                     255U, 0U, &_rem_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_pkt_is_nbr_adv(gnrc_pktsnip_t *pkt, const ipv6_addr_t *dst,
                                const ipv6_addr_t *tgt,
                                const uint8_t *tgt_l2addr,
                                size_t tgt_l2addr_len)
{
    gnrc_netif_hdr_t *netif_hdr;
    ipv6_hdr_t *ipv6_hdr;
    ndp_nbr_adv_t *nbr_adv;
    ndp_opt_t *tl2ao;

    /* first snip is a netif header to _mock_netif */
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_NETIF, pkt->type);
    TEST_ASSERT(sizeof(gnrc_netif_hdr_t) <= pkt->size);
    netif_hdr = pkt->data;
    TEST_ASSERT_EQUAL_INT(_mock_netif->pid, netif_hdr->if_pid);
    /* second snip is an IPv6 header to dst */
    TEST_ASSERT_NOT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_IPV6, pkt->next->type);
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_hdr_t), pkt->next->size);
    ipv6_hdr = pkt->next->data;
    TEST_ASSERT(!ipv6_addr_is_multicast(&ipv6_hdr->dst));
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(dst, &ipv6_hdr->dst),
                        "dst != ipv6_hdr->dst");
    TEST_ASSERT_EQUAL_INT(255, ipv6_hdr->hl);
    /* third snip is a valid solicited neighbor advertisement to tgt */
    TEST_ASSERT_NOT_NULL(pkt->next->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_ICMPV6, pkt->next->next->type);
    TEST_ASSERT_EQUAL_INT(sizeof(ndp_nbr_adv_t), pkt->next->next->size);
    nbr_adv = pkt->next->next->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_NBR_ADV, nbr_adv->type);
    TEST_ASSERT_EQUAL_INT(0, nbr_adv->code);
    TEST_ASSERT(!ipv6_addr_is_multicast(&nbr_adv->tgt));
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(tgt, &nbr_adv->tgt),
                        "tgt != nbr_adv->tgt");
    TEST_ASSERT(nbr_adv->flags & NDP_NBR_ADV_FLAGS_S);
    /* fourth snip is a TL2AO for tgt_l2addr */
    TEST_ASSERT_NOT_NULL(pkt->next->next->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, pkt->next->next->next->type);
    TEST_ASSERT_EQUAL_INT(ceil8(sizeof(ndp_opt_t) + tgt_l2addr_len),
                          pkt->next->next->next->size);
    tl2ao = pkt->next->next->next->data;
    TEST_ASSERT_EQUAL_INT(NDP_OPT_TL2A, tl2ao->type);
    TEST_ASSERT_EQUAL_INT(1, tl2ao->len);
    TEST_ASSERT_MESSAGE(memcmp(tl2ao + 1, tgt_l2addr, tgt_l2addr_len) == 0,
                        "tl2ao.l2addr != tgt_l2addr");
    /* no further options */
    TEST_ASSERT_NULL(pkt->next->next->next->next);
}

static void test_handle_pkt__nbr_sol__ll_src(unsigned exp_nud_state,
                                             unsigned exp_ar_state)
{
    msg_t msg;
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes,
                                     255U, 0U, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "Expected neighbor cache entry");
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(&_rem_ll, &nce.ipv6),
                        "_rem_ll != nce->ipv6");
    TEST_ASSERT_EQUAL_INT(sizeof(_rem_l2), nce.l2addr_len);
    TEST_ASSERT_MESSAGE(memcmp(_rem_l2, nce.l2addr, nce.l2addr_len) == 0,
                        "_rem_l2 != nce.l2addr");
    TEST_ASSERT_EQUAL_INT(exp_nud_state, gnrc_ipv6_nib_nc_get_nud_state(&nce));
    TEST_ASSERT(!gnrc_ipv6_nib_nc_is_router(&nce));
    TEST_ASSERT_EQUAL_INT(_mock_netif->pid, gnrc_ipv6_nib_nc_get_iface(&nce));
    TEST_ASSERT_EQUAL_INT(exp_ar_state, gnrc_ipv6_nib_nc_get_ar_state(&nce));
    TEST_ASSERT_EQUAL_INT(1, msg_avail());
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    test_pkt_is_nbr_adv(msg.content.ptr, &_rem_ll, &_loc_ll, _loc_l2,
                        sizeof(_loc_l2));
    gnrc_pktbuf_release(msg.content.ptr);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_handle_pkt__nbr_sol__ll_src_empty_nc(void)
{
    test_handle_pkt__nbr_sol__ll_src(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE,
                                     GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC);
}

static void test_handle_pkt__nbr_sol__ll_src_unmanaged_nce(void)
{
    test_get_next_hop_l2addr__link_local_static_conf();
    /* unmanaged entry stays unmanaged */
    test_handle_pkt__nbr_sol__ll_src(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED,
                                     GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL);
}

static void test_handle_pkt__nbr_sol__ll_src_no_sl2ao(void)
{
    msg_t msg;
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_sol(&_rem_ll, &_loc_sol_nodes,
                                     255U, 0U, &_loc_ll, NULL, 0);

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    TEST_ASSERT_EQUAL_INT(1, msg_avail());
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    test_pkt_is_nbr_adv(msg.content.ptr, &_rem_ll, &_loc_ll, _loc_l2,
                        sizeof(_loc_l2));
    gnrc_pktbuf_release(msg.content.ptr);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static size_t _set_nbr_adv(const ipv6_addr_t *ipv6_src,
                           const ipv6_addr_t *ipv6_dst,
                           uint8_t ipv6_hl, uint8_t nbr_adv_code,
                           uint8_t nbr_adv_flags,
                           const ipv6_addr_t *nbr_adv_tgt,
                           const uint8_t *tl2ao_addr, size_t tl2ao_addr_len)
{
    size_t icmpv6_len = sizeof(ndp_nbr_adv_t);
    ndp_nbr_adv_t *nbr_adv = (ndp_nbr_adv_t *)icmpv6;

    ipv6_hdr_set_version(ipv6);
    ipv6->hl = ipv6_hl;
    memcpy(&ipv6->src, ipv6_src, sizeof(ipv6->src));
    memcpy(&ipv6->dst, ipv6_dst, sizeof(ipv6->dst));
    nbr_adv->type = ICMPV6_NBR_ADV;
    nbr_adv->code = nbr_adv_code;
    nbr_adv->flags = nbr_adv_flags;
    memcpy(&nbr_adv->tgt, nbr_adv_tgt, sizeof(nbr_adv->tgt));

    if ((tl2ao_addr != NULL) || (tl2ao_addr_len > 0)) {
        ndp_opt_t *tl2ao = (ndp_opt_t *)&_buffer[sizeof(ipv6_hdr_t) +
                                                 sizeof(ndp_nbr_adv_t)];

        tl2ao->type = NDP_OPT_TL2A;
        tl2ao->len = ceil8(sizeof(ndp_opt_t) + tl2ao_addr_len) / 8;
        memcpy(tl2ao + 1, tl2ao_addr, tl2ao_addr_len);
        icmpv6_len += ceil8(sizeof(ndp_opt_t) + tl2ao_addr_len);
    }

    return icmpv6_len;
}

static void test_handle_pkt__nbr_adv__invalid_hl(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &_loc_ll, 194U, 0U,
                                     NDP_NBR_ADV_FLAGS_S, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__invalid_code(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &_loc_ll, 255U, 201U,
                                     NDP_NBR_ADV_FLAGS_S, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__invalid_icmpv6_len(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;

    _set_nbr_adv(&_rem_ll, &_loc_ll, 255U, 0U, NDP_NBR_ADV_FLAGS_S,
                 &_loc_ll, _rem_l2,
                 sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6,
                             sizeof(ndp_nbr_adv_t) - 1);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__invalid_tgt(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &_loc_ll, 255U, 0U,
                                     NDP_NBR_ADV_FLAGS_S,
                                     &ipv6_addr_all_routers_site_local, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__invalid_flags(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &ipv6_addr_all_nodes_link_local,
                                     255U, 0U, NDP_NBR_ADV_FLAGS_S, &_loc_ll,
                                     NULL, 0);

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__invalid_opt_len(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &_loc_ll, 255U, 0U,
                                     NDP_NBR_ADV_FLAGS_S, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));
    ndp_opt_t *opt = (ndp_opt_t *)&_buffer[icmpv6_len];

    opt->type = NDP_OPT_SL2A;
    opt->len = 0U;
    icmpv6_len += sizeof(ndp_opt_t);
    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__unspecified_src(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&ipv6_addr_unspecified, &_loc_ll, 255U, 0U,
                                     NDP_NBR_ADV_FLAGS_S, &_loc_ll, _rem_l2,
                                     sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    /* TODO: check other views as well */
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static void test_handle_pkt__nbr_adv__unsolicited(void)
{
    gnrc_ipv6_nib_nc_t nce;
    void *state = NULL;
    size_t icmpv6_len = _set_nbr_adv(&_rem_ll, &_loc_sol_nodes, 255U, 0U,
                                     NDP_NBR_ADV_FLAGS_S, &_loc_ll,
                                     _rem_l2, sizeof(_rem_l2));

    gnrc_ipv6_nib_handle_pkt(_mock_netif, ipv6, icmpv6, icmpv6_len);
    TEST_ASSERT_MESSAGE(!gnrc_ipv6_nib_nc_iter(0, &state, &nce),
                        "There is an unexpected neighbor cache entry");
    TEST_ASSERT_EQUAL_INT(0, msg_avail());
}

static Test *tests_gnrc_ipv6_nib(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /* gnrc_ipv6_nib_init() and gnrc_ipv6_nib_init_iface() "tested" in
         * set-up (otherwise the following tests wouldn't work) */
        /* TODO: ENETUNREACH when non-link-local communication is implemented */
        new_TestFixture(test_get_next_hop_l2addr__link_local_EHOSTUNREACH_no_iface),
        new_TestFixture(test_get_next_hop_l2addr__link_local_EHOSTUNREACH_iface),
        new_TestFixture(test_get_next_hop_l2addr__link_local_static_conf),
        new_TestFixture(test_get_next_hop_l2addr__link_local_after_handshake_iface),
        new_TestFixture(test_get_next_hop_l2addr__link_local_after_handshake_iface_router),
        new_TestFixture(test_get_next_hop_l2addr__link_local_after_handshake_no_iface),
        new_TestFixture(test_handle_pkt__unknown_type),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_hl),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_code),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_icmpv6_len),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_tgt),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_opt_len),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_dst),
        new_TestFixture(test_handle_pkt__nbr_sol__invalid_sl2ao),
        new_TestFixture(test_handle_pkt__nbr_sol__tgt_not_assigned),
        /* TODO add tests for unspecified source (involves SLAAC) */
        new_TestFixture(test_handle_pkt__nbr_sol__ll_src_empty_nc),
        new_TestFixture(test_handle_pkt__nbr_sol__ll_src_unmanaged_nce),
        new_TestFixture(test_handle_pkt__nbr_sol__ll_src_no_sl2ao),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_hl),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_code),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_icmpv6_len),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_tgt),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_flags),
        new_TestFixture(test_handle_pkt__nbr_adv__invalid_opt_len),
        new_TestFixture(test_handle_pkt__nbr_adv__unspecified_src),
        new_TestFixture(test_handle_pkt__nbr_adv__unsolicited),
        /* solicited tested in get_next_hop_l2addr */
        /* gnrc_ipv6_nib_handle_timer_event not testable in this context since
         * we do not have access to the (internally defined) contexts required
         * for it */
    };

    EMB_UNIT_TESTCALLER(tests, _set_up, NULL, fixtures);

    return (Test *)&tests;
}

int main(void)
{
    _tests_init();

    TESTS_START();
    TESTS_RUN(tests_gnrc_ipv6_nib());
    TESTS_END();

    return 0;
}

int _mock_netif_get(gnrc_netapi_opt_t *opt)
{
    switch (opt->opt) {
        case NETOPT_ADDRESS:
            if (opt->data_len < sizeof(_loc_l2)) {
                return -EOVERFLOW;
            }
            memcpy(opt->data, _loc_l2, sizeof(_loc_l2));
            return sizeof(_loc_l2);
        case NETOPT_IS_WIRED:
            return 1;
        case NETOPT_MAX_PACKET_SIZE: {
                uint16_t *val = opt->data;
                if (opt->data_len != sizeof(uint16_t)) {
                    return -EOVERFLOW;
                }
                *val = ETHERNET_DATA_LEN;
                return sizeof(uint16_t);
            }
        default:
            return -ENOTSUP;
    }
}

static inline size_t ceil8(size_t size)
{
    if (size % 8) {
        return ((size / 8) + 1) * 8;
    }
    else {
        return size;
    }
}
