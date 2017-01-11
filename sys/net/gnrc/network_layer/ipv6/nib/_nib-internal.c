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

static _nib_t _nodes[GNRC_IPV6_NIB_NUMOF];
static _nib_dr_t _def_routers[GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF];
static _nib_iface_t _nis[GNRC_NETIF_NUMOF];

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

mutex_t _nib_mutex = MUTEX_INIT;

static void _override_node(const ipv6_addr_t *addr, unsigned iface,
                           _nib_t *node);
static inline bool _node_unreachable(_nib_t *node);

void _nib_init(void)
{
    _prime_def_router = NULL;
    _last_def_router = NULL;
    memset(_nodes, 0, sizeof(_nodes));
    memset(_def_routers, 0, sizeof(_def_routers));
    memset(_nis, 0, sizeof(_nis));
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
            DEBUG("  Found %p\n", node);
            return node;
        }
    }
    DEBUG("  No suitable entry found\n");
    return NULL;
}

void _nib_nc_set_reachable(_nib_t *nib)
{
    DEBUG("nib: set %s%%%u reachable (reachable time = %u)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface,
          nib->reach_time);
    nib->info &= ~GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK;
    nib->info |= GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE;
    /* TODO add event for state change to STALE to event timer*/
}

void _nib_nc_remove(_nib_t *nib)
{
    nib->mode &= ~(_NC);
    /* TODO: remove NC related timers */
    _nib_clear(nib);
}

_nib_dr_t *_nib_drl_add(const ipv6_addr_t *router_addr, unsigned iface)
{
    _nib_dr_t *def_router = NULL;

    DEBUG("nib: Allocating default router list entry entry "
          "(router_addr = %s, iface = %u)\n",
          ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)), iface);
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

/** @} */
