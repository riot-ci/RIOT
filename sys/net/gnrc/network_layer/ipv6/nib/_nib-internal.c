/*
 * Copyright (C) 2016 Freie Universität Berlin
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
#include <string.h>

#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif.h"
#include "random.h"

#include "_nib-internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* pointers for default router selection */
static _nib_dr_t *_prime_def_router = NULL;
static _nib_dr_t *_last_def_router = NULL;
static clist_node_t _next_removable = { NULL };

static _nib_t _nodes[GNRC_IPV6_NIB_NUMOF];
static _nib_dr_t _def_routers[GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF];
static _nib_iface_t _nis[GNRC_NETIF_NUMOF];

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

mutex_t _nib_mutex = MUTEX_INIT;
evtimer_msg_t _nib_evtimer;

static void _override_node(const ipv6_addr_t *addr, unsigned iface,
                           _nib_t *node);
static inline bool _node_unreachable(_nib_t *node);

void _nib_init(void)
{
    _prime_def_router = NULL;
    _last_def_router = NULL;
    _next_removable.next = NULL;
    memset(_nodes, 0, sizeof(_nodes));
    memset(_def_routers, 0, sizeof(_def_routers));
    memset(_nis, 0, sizeof(_nis));
    evtimer_init_msg(&_nib_evtimer);
    /* TODO: load ABR information from persistent memory */
}

_nib_t *_nib_alloc(const ipv6_addr_t *addr, unsigned iface)
{
    _nib_t *node = NULL;

    assert(addr != NULL);
    DEBUG("nib: Allocating on-link node entry (addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
    for (unsigned i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_t *tmp = &_nodes[i];

        if ((ipv6_addr_equal(addr, &tmp->ipv6)) &&
            (_nib_get_if(tmp) == iface)) {
            /* exact match */
            DEBUG("  %p is an exact match\n", (void *)tmp);
            return tmp;
        }
        if ((node == NULL) && (tmp->mode == _EMPTY)) {
            node = tmp;
        }
    }
    if (node != NULL) {
        DEBUG("  using %p\n", (void *)node);
        _override_node(addr, iface, node);
    }
#if ENABLE_DEBUG
    else {
        DEBUG("  NIB full\n");
    }
#endif
    return node;
}

static inline bool _is_gc(_nib_t *nib)
{
    return ((nib->mode & ~(_NC)) == 0) &&
            ((nib->info & GNRC_IPV6_NIB_NC_INFO_AR_STATE_MASK) ==
              GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC);
}

_nib_t *_nib_nc_add(const ipv6_addr_t *addr, unsigned iface, uint16_t cstate)
{
    assert(cstate != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY);
    assert(cstate != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE);
    assert(cstate != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE);
    _nib_t *nib = _nib_alloc(addr, iface);
    if (nib != NULL) {
        DEBUG("nib: Adding to neighbor cache (addr = %s, iface = %u)\n",
              ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
        if (!(nib->mode & _NC)) {
            nib->info &= ~GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK;
            nib->info |= (cstate & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK);
            nib->mode |= _NC;
        }
        if (nib->next == NULL) {
            DEBUG("nib: queueing (addr = %s, iface = %u) for potential removal\n",
                  ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
            /* add to next removable list, if not already in it */
            clist_rpush(&_next_removable, (clist_node_t *)nib);
        }
    }
    else {
        _nib_t *first = (_nib_t *)clist_lpop(&_next_removable);
        _nib_t *tmp = first;

        DEBUG("nib: Searching for replaceable entries (addr = %s, iface = %u)\n",
              ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
        if (tmp != NULL) {
            do {
                if (_is_gc(tmp)) {
                    DEBUG("nib: Removing neighbor cache entry (addr = %s, "
                          "iface = %u) ",
                          ipv6_addr_to_str(addr_str, &tmp->ipv6,
                                           sizeof(addr_str)),
                          _nib_get_if(tmp));
                    DEBUG("for (addr = %s, iface = %u)\n",
                          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)),
                          iface);
                    nib = tmp;
                    nib->info = 0;
                    nib->mode = _EMPTY;
                    _override_node(addr, iface, nib);
                    nib->info |= (cstate & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK);
                    nib->mode |= _NC;
                    break;
                }
                else {
                    /* requeue if not garbage collectible at the moment */
                    DEBUG("nib: Requeing (addr = %s, iface = %u)\n",
                          ipv6_addr_to_str(addr_str, &tmp->ipv6,
                                           sizeof(addr_str)),
                          _nib_get_if(tmp));
                    clist_rpush(&_next_removable, (clist_node_t *)tmp);
                }
                tmp = (_nib_t *)clist_lpop(&_next_removable);
            } while (tmp != first);
        }
    }
    return nib;
}

_nib_t *_nib_iter(const _nib_t *last)
{
    bool found = false;

    for (unsigned i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_t *node = &_nodes[i];

        if (node->mode != _EMPTY) {
            if ((last == NULL)) {
                return node;
            }
            else if (!found && (last == node)) {
                found = true;
            }
            else if (found) {
                return node;
            }
        }
    }
    return NULL;
}

_nib_t *_nib_get(const ipv6_addr_t *addr, unsigned iface)
{
    assert(addr != NULL);
    DEBUG("nib: Getting on-link node entry (addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
    for (unsigned i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_t *node = &_nodes[i];

        if ((node->mode != _EMPTY) &&
            /* either requested or current interface undefined or interfaces equal */
            ((_nib_get_if(node) == 0) || (iface == 0) ||
             (_nib_get_if(node) == iface)) &&
            ipv6_addr_equal(&node->ipv6, addr)) {
            DEBUG("  Found %p\n", (void *)node);
            return node;
        }
    }
    DEBUG("  No suitable entry found\n");
    return NULL;
}

void _nib_nc_set_reachable(_nib_t *nib)
{
#if GNRC_IPV6_NIB_CONF_ARSM
    _nib_iface_t *iface = _nib_iface_get(_nib_get_if(nib));
    DEBUG("nib: set %s%%%u reachable (reachable time = %u)\n",
          ipv6_addr_to_str(addr_str, &nib->ipv6, sizeof(addr_str)),
          _nib_get_if(nib), iface->reach_time);
    nib->info &= ~GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK;
    nib->info |= GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE;
    _evtimer_add(nib, GNRC_IPV6_NIB_REACH_TIMEOUT, &nib->reach_timeout,
                 iface->reach_time);
#else
    (void)nib;
#endif
}

void _nib_nc_remove(_nib_t *nib)
{
    DEBUG("nib: remove from neighbor cache (addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, &nib->ipv6, sizeof(addr_str)),
          _nib_get_if(nib));
    nib->mode &= ~(_NC);
    /* TODO: remove NC related timers */
    _nib_clear(nib);
}

static inline void _get_l2addr_from_ipv6(uint8_t *l2addr,
                                         const ipv6_addr_t *ipv6)
{
    memcpy(l2addr, &ipv6->u64[1], sizeof(uint64_t));
    l2addr[0] ^= 0x02;
}

void _nib_nc_get(const _nib_t *nib, gnrc_ipv6_nib_nc_t *nce)
{
    assert((nib != NULL) && (nce != NULL));
    memcpy(&nce->ipv6, &nib->ipv6, sizeof(nce->ipv6));
    nce->info = nib->info;
#if GNRC_IPV6_NIB_CONF_ARSM
    if (ipv6_addr_is_link_local(&nce->ipv6)) {
        gnrc_ipv6_netif_t *netif = gnrc_ipv6_netif_get(_nib_get_if(nib));
        assert(netif != NULL);
        if ((netif->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) &&
            !(netif->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER)) {
            _get_l2addr_from_ipv6(nce->l2addr, &nib->ipv6);
            nce->l2addr_len = sizeof(uint64_t);
            return;
        }
    }
    nce->l2addr_len = nib->l2addr_len;
    memcpy(&nce->l2addr, &nib->l2addr, nib->l2addr_len);
#else
    assert(ipv6_addr_is_link_local(nce->ipv6));
    _get_l2addr_from_ipv6(nce->l2addr, &nib->ipv6);
    nce->l2addr_len = sizeof(uint64_t);
#endif
}

_nib_dr_t *_nib_drl_add(const ipv6_addr_t *router_addr, unsigned iface)
{
    _nib_dr_t *def_router = NULL;

    DEBUG("nib: Allocating default router list entry entry "
          "(router_addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, router_addr, sizeof(addr_str)), iface);
    for (unsigned i = 0; i < GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        _nib_dr_t *tmp = &_def_routers[i];
        _nib_t *tmp_node = tmp->next_hop;

        if ((tmp_node != NULL) &&
            (ipv6_addr_equal(router_addr, &tmp_node->ipv6)) &&
            (_nib_get_if(tmp_node) == iface)) {
            /* exact match */
            DEBUG("  %p is an exact match\n", (void *)tmp);
            tmp_node->mode |= _DRL;
            return tmp;
        }
        if ((def_router == NULL) && (tmp_node == NULL)) {
            def_router = tmp;
        }
    }
    if (def_router != NULL) {
        DEBUG("  using %p\n", (void *)def_router);
        def_router->next_hop = _nib_alloc(router_addr, iface);

        if (def_router->next_hop == NULL) {
            return NULL;
        }
        _override_node(router_addr, iface, def_router->next_hop);
        def_router->next_hop->mode |= _DRL;
    }
    return def_router;
}

void _nib_drl_remove(_nib_dr_t *nib_dr)
{
    if (nib_dr->next_hop != NULL) {
        nib_dr->next_hop->mode &= ~(_DRL);
        _nib_clear(nib_dr->next_hop);
        memset(nib_dr, 0, sizeof(_nib_dr_t));
    }
    if (nib_dr == _prime_def_router) {
        _prime_def_router = NULL;
    }
}

_nib_dr_t *_nib_drl_iter(const _nib_dr_t *last)
{
    bool found = false;

    for (unsigned i = 0; i < GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        _nib_dr_t *def_router = &_def_routers[i];
        _nib_t *node = def_router->next_hop;

        if (node != NULL) {
            /* It is linked to the default router list so it *should* be set */
            assert(node->mode & _DRL);
            if ((last == NULL)) {
                return def_router;
            }
            else if (!found && (last == def_router)) {
                found = true;
            }
            else if (found) {
                return def_router;
            }
        }
    }
    return NULL;
}

_nib_dr_t *_nib_drl_get(const ipv6_addr_t *router_addr, unsigned iface)
{
    for (unsigned i = 0; i < GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        _nib_dr_t *def_router = &_def_routers[i];
        _nib_t *node = def_router->next_hop;

        if ((node != NULL) &&
            (ipv6_addr_equal(router_addr, &node->ipv6)) &&
            (_nib_get_if(node) == iface)) {
            /* It is linked to the default router list so it *should* be set */
            assert(node->mode & _DRL);
            return def_router;
        }
    }
    return NULL;
}

_nib_dr_t *_nib_drl_get_dr(void)
{
    if ((_prime_def_router == NULL) ||
        (_node_unreachable(_prime_def_router->next_hop))) {
        _prime_def_router = NULL;   /* in case the prime router was unreachable */
        _nib_dr_t *last = _last_def_router;
        _nib_dr_t *res = _nib_drl_iter(last);

        if (res == NULL) {
            res = _nib_drl_iter(res);   /* try to wrap around */
            if (res == NULL) {          /* we really don't have a default router */
                return NULL;
            }
        }
        /* search a reachable router */
        while (_node_unreachable(res->next_hop)) {
            last = res;
            res = _nib_drl_iter(res);       /* go further */
            if (res == NULL) {
                res = _nib_drl_iter(res);   /* wrap around */
                if (_last_def_router == NULL) {
                    /* we iterated over the complete list, since we started at
                     * the first element */
                    break;
                }
            }
            if (last == _last_def_router) {
                break;
            }
        }
        if (_node_unreachable(res->next_hop)) {
            _last_def_router = res;
        }
        else {
            _last_def_router = NULL;
            _prime_def_router = res;
        }
        return res;
    }
    return _prime_def_router;
}

_nib_dst_t *_nib_dc_add(const ipv6_addr_t *next_hop, unsigned iface,
                        const ipv6_addr_t *dst)
{
    _nib_dst_t *nib_dst = _nib_dst_alloc(next_hop, iface, dst, IPV6_ADDR_BIT_LEN);
    if (nib_dst != NULL) {
        nib_dst->next_hop->mode |= (_DC);
    }
    return nib_dst;
}

_nib_iface_t *_nib_iface_get(unsigned iface)
{
    _nib_iface_t *ni = NULL;

    assert((iface > KERNEL_PID_UNDEF) && (iface <= KERNEL_PID_LAST));
    for (unsigned i = 0; i < GNRC_NETIF_NUMOF; i++) {
        _nib_iface_t *tmp = &_nis[i];
        if (((unsigned)tmp->pid) == iface) {
            return tmp;
        }
        if ((ni == NULL) && (tmp->pid == KERNEL_PID_UNDEF)) {
            ni = tmp;
        }
    }
    if (ni != NULL) {
        memset(ni, 0, sizeof(_nib_iface_t));
        /* TODO: set random reachable time using constants from #6220 */
        ni->pid = (kernel_pid_t)iface;
    }
    return ni;
}

static void _override_node(const ipv6_addr_t *addr, unsigned iface,
                           _nib_t *node)
{
    _nib_clear(node);
    memcpy(&node->ipv6, addr, sizeof(node->ipv6));
    _nib_set_if(node, iface);
}

static inline bool _node_unreachable(_nib_t *node)
{

    switch (node->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK) {
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE:
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE:
            return true;
        default:
            return false;
    }
}

uint32_t _evtimer_lookup(void *ctx, uint16_t type)
{
    evtimer_msg_event_t *event = (evtimer_msg_event_t *)_nib_evtimer.events;
    uint32_t offset = 0;

    mutex_lock(&_nib_mutex);
    DEBUG("nib: lookup ctx = %p, type = %u\n", (void *)ctx, type);
    while (event != NULL) {
        offset += event->event.offset;
        if ((event->msg.type == type) &&
            ((ctx == NULL) || (event->msg.content.ptr == ctx))) {
            mutex_unlock(&_nib_mutex);
            return offset;
        }
        event = (evtimer_msg_event_t *)event->event.next;
    }
    mutex_unlock(&_nib_mutex);
    return UINT32_MAX;
}

/** @} */
