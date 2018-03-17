/*
 * Copyright (C) 2018 Freie Universität Berlin
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

#include <stdbool.h>

#include "event.h"
#include "log.h"
#include "net/arp.h"
#include "net/dhcpv6/client.h"
#include "net/sock/udp.h"
#include "random.h"
#include "timex.h"
#include "xtimer.h"
#include "xtimer/implementation.h"

#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/rpl.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Representation of a generic lease
 */
typedef struct {
    clist_node_t list_node;
    union {
        uint32_t id;
        struct {
            uint16_t netif;
            uint16_t type;
        } info;
    } ia_id;
} lease_t;

/**
 * @brief   Representation of a DHCPv6 prefix deligation lease
 * @extends lease_t
 */
typedef struct {
    lease_t parent;
    ipv6_addr_t pfx;
    uint8_t pfx_len;
    uint8_t leased;
} pfx_lease_t;

/**
 * @brief   Client representation of a DHCPv6 server
 */
typedef struct {
    dhcpv6_duid_t duid;
    uint32_t t1;
    uint8_t pref;
    uint8_t duid_len;
} server_t;

static char addr_str[IPV6_ADDR_MAX_STR_LEN];
static uint8_t send_buf[DHCPV6_CLIENT_BUFLEN];
static uint8_t recv_buf[DHCPV6_CLIENT_BUFLEN];
static uint8_t best_adv[DHCPV6_CLIENT_BUFLEN];
static uint8_t duid[DHCPV6_CLIENT_DUID_LEN];
static pfx_lease_t pfx_leases[DHCPV6_CLIENT_PFX_LEASE_MAX];
static server_t server;
static xtimer_t timer, rebind_timer;
static event_queue_t *event_queue;
static sock_udp_t sock;
static sock_udp_ep_t local = { .family = AF_INET6, .port = DHCPV6_CLIENT_PORT };
static sock_udp_ep_t remote = { .family = AF_INET6, .port = DHCPV6_SERVER_PORT,
                                .addr = {
                                   .ipv6 = DHCPV6_ALL_RELAY_AGENTS_AND_SERVERS
                                } };
static uint32_t sol_max_rt = DHCPV6_SOL_MAX_RT;
static uint32_t t2, rebind_time;
static uint32_t transaction_start;
static uint32_t transaction_id;
static uint8_t duid_len = sizeof(dhcpv6_duid_l2_t);

static void _generate_duid(void);
static void _post_solicit_servers(void *args);
static void _solicit_servers(event_t *event);
static void _request(event_t *event);
static void _renew(event_t *event);
static void _rebind(event_t *event);

static event_t solicit_servers = { .handler = _solicit_servers };
static event_t request = { .handler = _request };
static event_t renew = { .handler = _renew };
static event_t rebind = { .handler = _rebind };

#ifdef MODULE_DHCPV6_CLIENT_AUTO_INIT
static char _thread_stack[DHCPV6_CLIENT_STACK_SIZE];
static void *_thread(void *args);
static kernel_pid_t _thread_pid;

void dhcpv6_client_auto_init(void)
{
    if (_thread_pid <= 0) {
        _thread_pid = thread_create(_thread_stack, DHCPV6_CLIENT_STACK_SIZE,
                                    DHCPV6_CLIENT_PRIORITY,
                                    THREAD_CREATE_STACKTEST,
                                    _thread, NULL, "dhcpv6-client");
    }
}

static void *_thread(void *args)
{
    (void)args;
    event_queue_t event_queue;
    event_queue_init(&event_queue);
    dhcpv6_client_init(&event_queue, SOCK_ADDR_ANY_NETIF);
    /* TODO: add configuration for IA_NA here, when implemented */
    dhcpv6_client_start();
    event_loop(&event_queue);   /* never returns */
    return NULL;
}
#endif /* MODULE_DHCPV6_CLIENT_AUTO_INIT */

void dhcpv6_client_init(event_queue_t *eq, uint16_t netif)
{
    assert(eq->waiter != NULL);
    event_queue = eq;
    local.netif = netif;
    remote.netif = netif;
}

void dhcpv6_client_start(void)
{
    uint32_t delay = random_uint32_range(0, DHCPV6_SOL_MAX_DELAY * US_PER_SEC);

    _generate_duid();
    sock_udp_create(&sock, &local, NULL, 0);
    timer.callback = _post_solicit_servers;
    xtimer_set(&timer, delay);
}

void dhcpv6_client_req_ia_pd(uint16_t netif, uint8_t pfx_len)
{
    pfx_lease_t *lease = NULL;

    for (unsigned i = 0; i < DHCPV6_CLIENT_PFX_LEASE_MAX; i++) {
        if (pfx_leases[i].parent.ia_id.id == 0) {
            lease = &pfx_leases[i];
            lease->parent.ia_id.info.netif = netif;
            lease->parent.ia_id.info.type = DHCPV6_OPT_IA_PD;
            lease->pfx_len = pfx_len;
        }
    }
}

/* TODO make GNRC-independent */
static void _configure_prefix(pfx_lease_t *lease, uint32_t valid,
                              uint32_t pref)
{
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(lease->parent.ia_id.info.netif);
    eui64_t iid;
    ipv6_addr_t addr;

    assert(netif != NULL);
    DEBUG("DHCPv6 client: (re-)configure prefix %s/%d\n",
          ipv6_addr_to_str(addr_str, &lease->pfx, sizeof(addr_str)),
          lease->pfx_len);
    if (gnrc_netapi_get(netif->pid, NETOPT_IPV6_IID, 0, &iid,
                        sizeof(eui64_t)) >= 0) {
        ipv6_addr_set_aiid(&addr, iid.uint8);
    }
    else {
        LOG_WARNING("DHCPv6 client: cannot get IID of netif %u\n", netif->pid);
        return;
    }
    ipv6_addr_init_prefix(&addr, &lease->pfx, lease->pfx_len);
    if (gnrc_netif_ipv6_addr_add(netif, &addr, lease->pfx_len, 0) > 0) {
        /* update lifetime */
        if (valid < UINT32_MAX) { /* UINT32_MAX means infinite lifetime */
            /* the valid lifetime is given in seconds, but the NIB's timers work
             * in microseconds, so we have to scale down to the smallest
             * possible value (UINT32_MAX - 1). */
            valid = (valid > (UINT32_MAX / MS_PER_SEC)) ?
                          (UINT32_MAX - 1) : valid * MS_PER_SEC;
        }
        if (pref < UINT32_MAX) { /* UINT32_MAX means infinite lifetime */
            /* same treatment for pref */
            pref = (pref > (UINT32_MAX / MS_PER_SEC)) ?
                         (UINT32_MAX - 1) : pref * MS_PER_SEC;
        }
        gnrc_ipv6_nib_pl_set(netif->pid, &lease->pfx, lease->pfx_len,
                             valid, pref);
#if defined(MODULE_GNRC_IPV6_NIB) && GNRC_IPV6_NIB_CONF_6LBR && \
        GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
        gnrc_ipv6_nib_abr_add(&addr);
#endif
#ifdef MODULE_GNRC_RPL
        gnrc_rpl_init(netif->pid);
        gnrc_rpl_instance_t *inst = gnrc_rpl_instance_get(GNRC_RPL_DEFAULT_INSTANCE);
        if (inst) {
            gnrc_rpl_instance_remove(inst);
        }
        gnrc_rpl_root_init(GNRC_RPL_DEFAULT_INSTANCE, &addr, false, false);
#endif
    }
}

static void _post_solicit_servers(void *args)
{
    (void)args;
    event_post(event_queue, &solicit_servers);
}

static void _post_renew(void *args)
{
    (void)args;
    event_post(event_queue, &renew);
}

static void _post_rebind(void *args)
{
    (void)args;
    event_post(event_queue, &rebind);
}

static void _generate_duid(void)
{
    gnrc_netif_t *netif;
    dhcpv6_duid_l2_t *d = (dhcpv6_duid_l2_t *)duid;
    uint8_t *l2addr = ((uint8_t *)(duid)) + sizeof(dhcpv6_duid_l2_t);
    int res;

    d->type = byteorder_htons(DHCPV6_DUID_TYPE_L2);
    /* TODO make GNRC-independent */
    if (local.netif == SOCK_ADDR_ANY_NETIF) {
        netif = gnrc_netif_iter(NULL);
    }
    else {
        netif = gnrc_netif_get_by_pid(local.netif);
    }
    assert(netif != NULL);
    if ((res = gnrc_netapi_get(netif->pid, NETOPT_ADDRESS_LONG, 0,
                               l2addr, GNRC_NETIF_L2ADDR_MAXLEN)) > 0) {
        d->l2type = byteorder_htons(ARP_HWTYPE_EUI64);
    }
    else if ((netif->device_type == NETDEV_TYPE_ETHERNET) &&
             ((res = gnrc_netapi_get(netif->pid, NETOPT_ADDRESS, 0,
                                     l2addr, GNRC_NETIF_L2ADDR_MAXLEN)) > 0)) {
        d->l2type = byteorder_htons(ARP_HWTYPE_ETHERNET);
    }
    else {
        LOG_ERROR("DHCPv6 client: Link-layer type of interface %u not supported "
                  "for DUID creation", netif->pid);
        return;
    }
    duid_len = (uint8_t)res + sizeof(dhcpv6_duid_l2_t);
}

static void _generate_tid(void)
{
    transaction_id = random_uint32() & 0xffffff;
}

static void _set_tid(uint8_t *tgt)
{
    tgt[0] = (transaction_id & 0xff0000) >> 16;
    tgt[1] = (transaction_id & 0xff00) >> 8;
    tgt[2] = transaction_id & 0xff;
}

static inline bool _is_tid(dhcpv6_msg_t *msg)
{
    uint32_t tid = (msg->tid[0] << 16) | (msg->tid[1] << 8) | (msg->tid[2]);

    return (transaction_id == (tid));
}

static inline uint32_t _now_cs(void)
{
    return (uint32_t)(xtimer_now_usec64() / US_PER_CS);
}

static inline uint16_t _compose_cid_opt(dhcpv6_opt_duid_t *cid)
{
    uint16_t len = duid_len;

    cid->type = byteorder_htons(DHCPV6_OPT_CID);
    cid->len = byteorder_htons(len);
    memcpy(cid->duid, duid, duid_len);
    return len + sizeof(dhcpv6_opt_t);
}

static inline uint16_t _compose_sid_opt(dhcpv6_opt_duid_t *sid)
{
    uint16_t len = server.duid_len;

    sid->type = byteorder_htons(DHCPV6_OPT_SID);
    sid->len = byteorder_htons(len);
    memcpy(sid->duid, server.duid.u8, server.duid_len);
    return len + sizeof(dhcpv6_opt_t);
}

static inline uint16_t _get_elapsed_time(void)
{
    uint32_t now = _now_cs();
    uint32_t elapsed_time = transaction_start - now;

    if (elapsed_time > UINT16_MAX) {
        /* xtimer_now_usec64() overflowed since transaction_start */
        elapsed_time = (UINT32_MAX - transaction_start) + now + 1;
    }
    return elapsed_time;
}

static inline size_t _compose_elapsed_time_opt(dhcpv6_opt_elapsed_time_t *time)
{
    uint16_t len = 2U;

    time->type = byteorder_htons(DHCPV6_OPT_ELAPSED_TIME);
    time->len = byteorder_htons(len);
    time->elapsed_time = byteorder_htons(_get_elapsed_time());
    return len + sizeof(dhcpv6_opt_t);
}

static inline size_t _compose_oro_opt(dhcpv6_opt_oro_t *oro, uint16_t *opts,
                                        unsigned opts_num)
{
    uint16_t len = 2U * opts_num;

    oro->type = byteorder_htons(DHCPV6_OPT_ORO);
    oro->len = byteorder_htons(len);
    for (unsigned i = 0; i < opts_num; i++) {
        oro->opt_codes[i] = byteorder_htons(opts[i]);
    }
    return len + sizeof(dhcpv6_opt_t);
}

static inline size_t _compose_ia_pd_opt(dhcpv6_opt_ia_pd_t *ia_pd,
                                        uint32_t ia_id, uint16_t opts_len)
{
    uint16_t len = 12U + opts_len;

    ia_pd->type = byteorder_htons(DHCPV6_OPT_IA_PD);
    ia_pd->len = byteorder_htons(len);
    ia_pd->ia_id = byteorder_htonl(ia_id);
    ia_pd->t1.u32 = 0;
    ia_pd->t2.u32 = 0;
    return len + sizeof(dhcpv6_opt_t);
}

static inline size_t _add_ia_pd_from_config(uint8_t *buf)
{
    size_t msg_len = 0;

    for (unsigned i = 0; i < DHCPV6_CLIENT_PFX_LEASE_MAX; i++) {
        uint32_t ia_id = pfx_leases[i].parent.ia_id.id;
        if (ia_id != 0) {
            dhcpv6_opt_ia_pd_t *ia_pd = (dhcpv6_opt_ia_pd_t *)(&buf[msg_len]);

            msg_len += _compose_ia_pd_opt(ia_pd, ia_id, 0U);
        }
    }
    return msg_len;
}

static inline int32_t get_rand_us(void)
{
    int32_t rand = ((int32_t)random_uint32_range(0, 200 * US_PER_MS));
    rand -= 100 * US_PER_SEC;
    return rand;
}

static inline uint32_t _irt_us(uint16_t irt, bool greater_irt)
{
    uint32_t irt_us = (irt * US_PER_SEC);
    int32_t rand = get_rand_us();

    if (greater_irt && (rand < 0)) {
        rand = -rand;
    }
    irt_us += (rand * irt_us) / US_PER_SEC;
    return irt_us;
}

static inline uint32_t _sub_rt_us(uint32_t rt_prev_us, uint16_t mrt)
{
    uint32_t sub_rt_us = (2 * rt_prev_us) +
                         ((get_rand_us() * rt_prev_us) / US_PER_SEC);

    if (sub_rt_us > (mrt * US_PER_SEC)) {
        uint32_t mrt_us = mrt * US_PER_SEC;

        sub_rt_us = mrt_us + ((get_rand_us() * mrt_us) / US_PER_SEC);
    }
    return sub_rt_us;
}

#define OPT_LEN(opt)    (sizeof(dhcpv6_opt_t) + byteorder_ntohs((opt)->len))
#define FOREACH_OPT(first, opt, len) \
    for (opt = (dhcpv6_opt_t *)(first); \
         len > 0; \
         len -= OPT_LEN(opt), \
         opt = (dhcpv6_opt_t *)(((uint8_t *)(opt)) + OPT_LEN(opt)))

static bool _check_status_opt(dhcpv6_opt_status_t *status)
{
    /* DHCPV6_STATUS_SUCCESS is 0, so we don't need to fix byte order */
#if ENABLE_DEBUG
    if ((status != NULL) && (status->code.u16 != DHCPV6_STATUS_SUCCESS)) {
        size_t msg_len = byteorder_ntohs(status->len);
        char msg[msg_len - 1];

        strncpy(msg, status->msg, msg_len - 2);
        DEBUG("DHCPv6 client: server returned error (%u) \"%s\"\n",
              byteorder_ntohs(status->code), msg);
    }
#endif
    return (status == NULL) || (status->code.u16 == DHCPV6_STATUS_SUCCESS);
}

static bool _check_cid_opt(dhcpv6_opt_duid_t *cid)
{
#if ENABLE_DEBUG
    if ((byteorder_ntohs(cid->len) != duid_len) ||
        (memcmp(cid->duid, duid, duid_len) != 0)) {
        DEBUG("DHCPv6 client: message is not for me\n");
    }
#endif
    return ((byteorder_ntohs(cid->len) == duid_len) &&
            (memcmp(cid->duid, duid, duid_len) == 0));
}

static bool _check_sid_opt(dhcpv6_opt_duid_t *sid)
{
#if ENABLE_DEBUG
    if ((byteorder_ntohs(sid->len) != server.duid_len) ||
        (memcmp(sid->duid, server.duid.u8, server.duid_len) != 0)) {
        DEBUG("DHCPv6 client: message is not from my server\n");
    }
#endif
    return ((byteorder_ntohs(sid->len) == server.duid_len) &&
            (memcmp(sid->duid, server.duid.u8, server.duid_len) == 0));
}

static int _preparse_advertise(uint8_t *adv, size_t len, uint8_t **buf)
{
    dhcpv6_opt_duid_t *cid = NULL, *sid = NULL;
    dhcpv6_opt_pref_t *pref = NULL;
    dhcpv6_opt_status_t *status = NULL;
    dhcpv6_opt_ia_pd_t *ia_pd = NULL;
    dhcpv6_opt_t *opt = NULL;
    size_t orig_len = len;
    uint8_t pref_val = 0;

    DEBUG("DHCPv6 client: received ADVERTISE\n");
    if ((len < sizeof(dhcpv6_msg_t)) || !_is_tid((dhcpv6_msg_t *)adv)) {
        DEBUG("DHCPv6 client: packet too small or transaction ID wrong\n");
        return -1;
    }
    FOREACH_OPT(&adv[sizeof(dhcpv6_msg_t)], opt, len) {
        if (len > orig_len) {
            DEBUG("DHCPv6 client: ADVERTISE options overflow packet boundaries\n");
            return -1;
        }
        switch (byteorder_ntohs(opt->type)) {
            case DHCPV6_OPT_CID:
                cid = (dhcpv6_opt_duid_t *)opt;
                break;
            case DHCPV6_OPT_SID:
                sid = (dhcpv6_opt_duid_t *)opt;
                break;
            case DHCPV6_OPT_STATUS:
                status = (dhcpv6_opt_status_t *)opt;
                break;
            case DHCPV6_OPT_IA_PD:
                ia_pd = (dhcpv6_opt_ia_pd_t *)opt;
                break;
            case DHCPV6_OPT_PREF:
                pref = (dhcpv6_opt_pref_t *)opt;
                break;
            default:
                break;
        }
    }
    if ((cid == NULL) || (sid == NULL) || (ia_pd == NULL)) {
        DEBUG("DHCPv6 client: ADVERTISE does not contain either server ID, "
              "client ID or IA_PD option\n");
        return false;
    }
    if (!_check_status_opt(status) || !_check_cid_opt(cid)) {
        return -1;
    }
    if (pref != NULL) {
        pref_val = pref->value;
    }
    if ((server.duid_len == 0) || (pref_val > server.pref)) {
        memcpy(best_adv, recv_buf, orig_len);
        if (buf != NULL) {
            *buf = best_adv;
        }
        server.duid_len = byteorder_ntohs(sid->len);
        memcpy(server.duid.u8, sid->duid, server.duid_len);
        server.pref = pref_val;
    }
    return pref_val;
}

static void _schedule_t2(void)
{
    if (t2 < UINT32_MAX) {
        uint64_t t2_usec = t2 * US_PER_SEC;

        rebind_time = (xtimer_now_usec64() / US_PER_SEC) + t2;
        xtimer_remove(&rebind_timer);
        rebind_timer.callback = _post_rebind;
        DEBUG("DHCPv6 client: scheduling REBIND in %lu sec\n",
              (unsigned long)t2);
        _xtimer_set64(&rebind_timer, t2_usec, t2_usec >> 32);
    }
}

static void _schedule_t1_t2(void)
{
    if (server.t1 < UINT32_MAX) {
        uint64_t t1_usec = server.t1 * US_PER_SEC;

        xtimer_remove(&timer);
        timer.callback = _post_renew;
        DEBUG("DHCPv6 client: scheduling RENEW in %lu sec\n",
              (unsigned long)server.t1);
        _xtimer_set64(&timer, t1_usec, t1_usec >> 32);
    }
    _schedule_t2();
}

static void _parse_advertise(uint8_t *adv, size_t len)
{
    dhcpv6_opt_smr_t *smr = NULL;
    dhcpv6_opt_t *opt = NULL;

    /* might not have been executed when not received in first retransmission
     * window => redo even if already done */
    if (_preparse_advertise(adv, len, NULL) < 0) {
        return;
    }
    DEBUG("DHCPv6 client: scheduling REQUEST\n");
    event_post(event_queue, &request);
    FOREACH_OPT(&adv[sizeof(dhcpv6_msg_t)], opt, len) {
        switch (byteorder_ntohs(opt->type)) {
            case DHCPV6_OPT_IA_PD:
                for (unsigned i = 0; i < DHCPV6_CLIENT_PFX_LEASE_MAX; i++) {
                    dhcpv6_opt_ia_pd_t *ia_pd = (dhcpv6_opt_ia_pd_t *)opt;
                    dhcpv6_opt_t *ia_pd_opt = NULL;
                    unsigned pd_t1, pd_t2;
                    uint32_t ia_id = byteorder_ntohl(ia_pd->ia_id);
                    size_t ia_pd_len = byteorder_ntohs(ia_pd->len);
                    size_t ia_pd_orig_len = ia_pd_len;

                    if (pfx_leases[i].parent.ia_id.id != ia_id) {
                        continue;
                    }
                    /* check for status */
                    FOREACH_OPT(ia_pd + 1, ia_pd_opt, ia_pd_len) {
                        if (ia_pd_len > ia_pd_orig_len) {
                            DEBUG("DHCPv6 client: IA_PD options overflow option "
                                  "boundaries\n");
                            return;
                        }
                        switch (byteorder_ntohs(ia_pd_opt->type)) {
                            case DHCPV6_OPT_STATUS: {
                                if (!_check_status_opt((dhcpv6_opt_status_t *)ia_pd_opt)) {
                                    continue;
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    pd_t1 = byteorder_ntohl(ia_pd->t1);
                    pd_t2 = byteorder_ntohl(ia_pd->t2);
                    if ((pd_t1 != 0) && (pd_t2 != 0) &&
                        (server.t1 > pd_t1) && (t2 > pd_t2)) {
                        server.t1 = pd_t1;
                        t2 = pd_t2;
                        _schedule_t2();
                    }
                }
                break;
            case DHCPV6_OPT_SMR:
                smr = (dhcpv6_opt_smr_t *)opt;
                break;
            default:
                break;
        }
    }
    if (smr != NULL) {
        sol_max_rt = byteorder_ntohl(smr->value);
    }
    return;
}

static bool _parse_reply(uint8_t *rep, size_t len)
{
    dhcpv6_opt_duid_t *cid = NULL, *sid = NULL;
    dhcpv6_opt_ia_pd_t *ia_pd = NULL;
    dhcpv6_opt_status_t *status = NULL;
    dhcpv6_opt_smr_t *smr = NULL;
    dhcpv6_opt_t *opt = NULL;
    size_t orig_len = len;

    DEBUG("DHCPv6 client: received REPLY\n");
    if ((len < sizeof(dhcpv6_msg_t)) || !_is_tid((dhcpv6_msg_t *)rep)) {
        DEBUG("DHCPv6 client: packet too small or transaction ID wrong\n");
        return false;
    }
    FOREACH_OPT(&rep[sizeof(dhcpv6_msg_t)], opt, len) {
        if (len > orig_len) {
            DEBUG("DHCPv6 client: ADVERTISE options overflow packet boundaries\n");
            return false;
        }
        switch (byteorder_ntohs(opt->type)) {
            case DHCPV6_OPT_CID:
                cid = (dhcpv6_opt_duid_t *)opt;
                break;
            case DHCPV6_OPT_SID:
                sid = (dhcpv6_opt_duid_t *)opt;
                break;
            case DHCPV6_OPT_STATUS:
                status = (dhcpv6_opt_status_t *)opt;
                break;
            case DHCPV6_OPT_IA_PD:
                ia_pd = (dhcpv6_opt_ia_pd_t *)opt;
                break;
            case DHCPV6_OPT_SMR:
                smr = (dhcpv6_opt_smr_t *)opt;
                break;
            default:
                break;
        }
    }
    if ((cid == NULL) || (sid == NULL) || (ia_pd == NULL)) {
        DEBUG("DHCPv6 client: ADVERTISE does not contain either server ID, "
              "client ID or IA_PD option\n");
        return false;
    }
    if (!_check_cid_opt(cid) || !_check_sid_opt(sid)) {
        return false;
    }
    if (smr != NULL) {
        sol_max_rt = byteorder_ntohl(smr->value);
    }
    if (!_check_status_opt(status)) {
        return false;
    }
    len = orig_len;
    FOREACH_OPT(&rep[sizeof(dhcpv6_msg_t)], opt, len) {
        switch (byteorder_ntohs(opt->type)) {
            case DHCPV6_OPT_IA_PD:
                for (unsigned i = 0; i < DHCPV6_CLIENT_PFX_LEASE_MAX; i++) {
                    dhcpv6_opt_iapfx_t *iapfx = NULL;
                    pfx_lease_t *lease = &pfx_leases[i];
                    ia_pd = (dhcpv6_opt_ia_pd_t *)opt;
                    dhcpv6_opt_t *ia_pd_opt = NULL;
                    unsigned pd_t1, pd_t2;
                    uint32_t ia_id = byteorder_ntohl(ia_pd->ia_id);
                    size_t ia_pd_len = byteorder_ntohs(ia_pd->len);
                    size_t ia_pd_orig_len = ia_pd_len;

                    if (lease->parent.ia_id.id != ia_id) {
                        continue;
                    }
                    /* check for status */
                    FOREACH_OPT(ia_pd + 1, ia_pd_opt, ia_pd_len) {
                        if (ia_pd_len > ia_pd_orig_len) {
                            DEBUG("DHCPv6 client: IA_PD options overflow option "
                                  "boundaries\n");
                            return false;
                        }
                        switch (byteorder_ntohs(ia_pd_opt->type)) {
                            case DHCPV6_OPT_STATUS: {
                                if (!_check_status_opt((dhcpv6_opt_status_t *)ia_pd_opt)) {
                                    continue;
                                }
                                break;
                            }
                            case DHCPV6_OPT_IAPFX: {
                                dhcpv6_opt_iapfx_t *this_iapfx = (dhcpv6_opt_iapfx_t *)ia_pd_opt;
                                if ((!lease->leased) ||
                                    (iapfx == NULL) ||
                                    ((this_iapfx->pfx_len == lease->pfx_len) &&
                                     ipv6_addr_match_prefix(&this_iapfx->pfx,
                                                            &lease->pfx) >= lease->pfx_len)) {
                                    /* only take first prefix for now */
                                    iapfx = this_iapfx;
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    pd_t1 = byteorder_ntohl(ia_pd->t1);
                    pd_t2 = byteorder_ntohl(ia_pd->t2);
                    if ((pd_t1 != 0) && (pd_t2 != 0) &&
                        ((server.t1 == 0) || (server.t1 >= pd_t1)) &&
                        ((t2 == 0) || (t2 >= pd_t2))) {
                        server.t1 = pd_t1;
                        t2 = pd_t2;
                        _schedule_t1_t2();
                    }
                    if ((iapfx != NULL)) {
                        uint32_t valid = byteorder_ntohl(iapfx->valid);
                        uint32_t pref = byteorder_ntohl(iapfx->pref);

                        lease->pfx_len = iapfx->pfx_len;
                        lease->leased = 1U;
                        ipv6_addr_init_prefix(&lease->pfx,
                                              &iapfx->pfx,
                                              iapfx->pfx_len);
                        if (iapfx->pfx_len > 0) {
                            _configure_prefix(lease, valid, pref);
                        }
                        return true;
                    }
                }
                break;
            default:
                break;
        }
    }
    return true;
}

static void _solicit_servers(event_t *event)
{
    dhcpv6_msg_t *msg = (dhcpv6_msg_t *)&send_buf[0];
    dhcpv6_opt_elapsed_time_t *time;
    uint8_t *buf = NULL;
    uint32_t retrans_timeout = _irt_us(DHCPV6_SOL_TIMEOUT, true);
    size_t msg_len = sizeof(dhcpv6_msg_t);
    int res, best_res = 0;
    bool first_rt = true;
    uint16_t oro_opts[] = { DHCPV6_OPT_SMR };

    (void)event;
    _generate_tid();
    msg->type = DHCPV6_SOLICIT;
    _set_tid(msg->tid);
    msg_len += _compose_cid_opt((dhcpv6_opt_duid_t *)&send_buf[msg_len]);
    transaction_start = _now_cs();
    time = (dhcpv6_opt_elapsed_time_t *)&send_buf[msg_len];
    msg_len += _compose_elapsed_time_opt(time);
    msg_len += _compose_oro_opt((dhcpv6_opt_oro_t *)&send_buf[msg_len], oro_opts,
                                sizeof(oro_opts) / sizeof(oro_opts[0]));
    msg_len += _add_ia_pd_from_config(&send_buf[msg_len]);
    DEBUG("DHCPv6 client: send SOLICIT\n");
    res = sock_udp_send(&sock, send_buf, msg_len, &remote);
    assert(res > 0);    /* something went terribly wrong */
    while (((res = sock_udp_recv(&sock, recv_buf, sizeof(recv_buf),
                                 retrans_timeout, NULL)) <= 0) ||
           (first_rt && (res > 0)) ||
           ((res > 0) && (recv_buf[0] != DHCPV6_ADVERTISE))) {
        if (first_rt && (res > 0) && (recv_buf[0] == DHCPV6_ADVERTISE)) {
            int parse_res;

            DEBUG("DHCPv6 client: initial transmission, collect best advertise\n");
            retrans_timeout -= (_get_elapsed_time() * US_PER_CS);
            parse_res = _preparse_advertise(recv_buf, res, &buf);
            if (buf != NULL) {
                best_res = res;
            }
            if ((parse_res == UINT8_MAX) ||
                (retrans_timeout > (DHCPV6_SOL_MAX_RT * US_PER_SEC))) {
                /* retrans_timeout underflowed => don't retry to receive */
                break;
            }
        }
        else if (buf == NULL) {
            DEBUG("DHCPv6 client: resend SOLICIT\n");
            first_rt = false;
            retrans_timeout = _sub_rt_us(retrans_timeout, DHCPV6_SOL_MAX_RT);
            _compose_elapsed_time_opt(time);
            res = sock_udp_send(&sock, send_buf, msg_len, &remote);
            assert(res > 0);    /* something went terribly wrong */
        }
        else {
            break;
        }
    }
    if (buf == NULL) {
        buf = recv_buf;
        best_res = res;
    }
    if (best_res > 0) {
        _parse_advertise(buf, best_res);
    }
}

static void _request_renew_rebind(uint8_t type)
{
    dhcpv6_msg_t *msg = (dhcpv6_msg_t *)&send_buf[0];
    dhcpv6_opt_elapsed_time_t *time;
    uint32_t retrans_timeout;
    size_t msg_len = sizeof(dhcpv6_msg_t);
    int res;
    uint16_t oro_opts[] = { DHCPV6_OPT_SMR };
    uint8_t retrans = 0;
    uint16_t irt;
    uint16_t mrt;
    uint16_t mrc = 0;
    uint16_t mrd = 0;

    switch (type) {
        case DHCPV6_REQUEST:
            irt = DHCPV6_REQ_TIMEOUT;
            mrt = DHCPV6_REQ_MAX_RT;
            mrc = DHCPV6_REQ_MAX_RC;
            break;
        case DHCPV6_RENEW:
            irt = DHCPV6_REN_TIMEOUT;
            mrt = DHCPV6_REN_MAX_RT;
            mrd = rebind_time - t2;
            break;
        case DHCPV6_REBIND: {
            irt = DHCPV6_REB_TIMEOUT;
            mrt = DHCPV6_REB_MAX_RT;
            /* calculate MRD from prefix leases */
            for (unsigned i = 0; i < DHCPV6_CLIENT_PFX_LEASE_MAX; i++) {
                /* TODO make GNRC independent */
                gnrc_ipv6_nib_pl_t ple;
                void *state = NULL;
                pfx_lease_t *lease = &pfx_leases[i];
                bool leases_expired = true;

                while (gnrc_ipv6_nib_pl_iter(lease->parent.ia_id.info.netif,
                                             &state, &ple)) {
                    if ((ple.pfx_len == lease->pfx_len) &&
                        ((ple.valid_until / MS_PER_SEC) > mrd) &&
                        (ipv6_addr_match_prefix(&ple.pfx,
                                                &lease->pfx) >= ple.pfx_len)) {
                        mrd = ple.valid_until / MS_PER_SEC;
                        if (mrd > 0) {
                            leases_expired = false;
                        }
                    }
                }
                if (leases_expired) {
                    /* all leases already expired, don't try to rebind and
                     * solicit immediately */
                    _post_solicit_servers(NULL);
                    return;
                }
            }
            break;
        }
        default:
            return;
    }
    retrans_timeout = _irt_us(irt, false);
    _generate_tid();
    msg->type = type;
    _set_tid(msg->tid);
    msg_len += _compose_cid_opt((dhcpv6_opt_duid_t *)&send_buf[msg_len]);
    if (type != DHCPV6_REBIND) {
        msg_len += _compose_sid_opt((dhcpv6_opt_duid_t *)&send_buf[msg_len]);
    }
    transaction_start = _now_cs();
    time = (dhcpv6_opt_elapsed_time_t *)&send_buf[msg_len];
    msg_len += _compose_elapsed_time_opt(time);
    msg_len += _compose_oro_opt((dhcpv6_opt_oro_t *)&send_buf[msg_len], oro_opts,
                                sizeof(oro_opts) / sizeof(oro_opts[0]));
    msg_len += _add_ia_pd_from_config(&send_buf[msg_len]);
    while (sock_udp_send(&sock, send_buf, msg_len, &remote) <= 0) {}
    while (((res = sock_udp_recv(&sock, recv_buf, sizeof(recv_buf),
                                 retrans_timeout, NULL)) <= 0) ||
           ((res > 0) && (recv_buf[0] != DHCPV6_REPLY))) {
        if ((mrd > 0) && (_get_elapsed_time() > (mrd * CS_PER_SEC))) {
            break;
        }
        retrans_timeout = _sub_rt_us(retrans_timeout, mrt);
        if ((mrc > 0) && (++retrans) >= mrc) {
            break;
        }
        _compose_elapsed_time_opt(time);
        DEBUG("DHCPv6 client: resend %s\n",
              (type == DHCPV6_REQUEST) ? "REQUEST" :
              (type == DHCPV6_RENEW) ? "RENEW": "REBIND");
        sock_udp_send(&sock, send_buf, msg_len, &remote);
    }
    if ((res > 0) && (recv_buf[0] == DHCPV6_REPLY)) {
        if (!_parse_reply(recv_buf, res)) {
            /* try again */
            event_post(event_queue, &request);
        }
    }
    else if (type == DHCPV6_REBIND) {
        _post_solicit_servers(NULL);
    }
}

static void _request(event_t *event)
{
    (void)event;
    DEBUG("DHCPv6 client: send REQUEST\n");
    _request_renew_rebind(DHCPV6_REQUEST);
}

static void _renew(event_t *event)
{
    (void)event;
    DEBUG("DHCPv6 client: send RENEW\n");
    _request_renew_rebind(DHCPV6_RENEW);
}

static void _rebind(event_t *event)
{
    (void)event;
    DEBUG("DHCPv6 client: send REBIND\n");
    _request_renew_rebind(DHCPV6_REBIND);
}

/** @} */
