/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_ipv6_nib
 * @internal
 * @{
 *
 * @file
 * @brief   Definitions related to router functionality of NIB
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef PRIV_NIB_ROUTER_H
#define PRIV_NIB_ROUTER_H

#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/netif2/internal.h"
#include "net/gnrc/netif2/ipv6.h"
#include "net/ipv6/addr.h"
#include "net/ndp.h"

#include "_nib-internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#if GNRC_IPV6_NIB_CONF_ROUTER
static inline void _init_iface_router(gnrc_netif2_t *netif)
{
    netif->ipv6.rtr_ltime = NDP_RTR_LTIME_SEC;
    netif->ipv6.last_ra = UINT32_MAX;
    netif->ipv6.ra_sent = 0;
    netif->flags |= GNRC_NETIF2_FLAGS_IPV6_FORWARDING;
#if !GNRC_IPV6_NIB_CONF_6LR || GNRC_IPV6_NIB_CONF_6LBR
    netif->flags |= GNRC_NETIF2_FLAGS_IPV6_RTR_ADV;
#endif
#if GNRC_IPV6_NIB_CONF_6LBR
    netif->flags |= GNRC_NETIF2_FLAGS_6LO_ABR;
#endif
    gnrc_netif2_ipv6_group_join(netif, &ipv6_addr_all_routers_link_local);
}

static inline void _call_route_info_cb(gnrc_netif2_t *netif, unsigned type,
                                       const ipv6_addr_t *ctx_addr,
                                       const void *ctx)
{
    if (netif->ipv6.route_info_cb != NULL) {
        netif->ipv6.route_info_cb(type, ctx_addr, ctx);
    }
}

void _handle_reply_rs(_nib_onl_entry_t *host);
void _handle_snd_mc_ra(gnrc_netif2_t *netif);
void _set_rtr_adv(gnrc_netif2_t *netif);
void _snd_rtr_advs(gnrc_netif2_t *netif, const ipv6_addr_t *dst,
                  bool final);
#else  /* GNRC_IPV6_NIB_CONF_ROUTER */
#define _init_iface_router(netif)                       (void)netif
#define _call_route_info_cb(netif, type, ctx_addr, ctx) (void)netif; \
                                                        (void)type; \
                                                        (void)ctx_addr; \
                                                        (void)ctx
#define _handle_reply_rs(host)                          (void)host
#define _handle_snd_mc_ra(netif)                        (void)netif
#define _set_rtr_adv(netif)                             (void)netif
#define _snd_rtr_advs(netif, dst, final)                (void)netif; \
                                                        (void)dst; \
                                                        (void)final
#endif /* GNRC_IPV6_NIB_CONF_ROUTER */

#ifdef __cplusplus
}
#endif

#endif /* PRIV_NIB_ROUTER_H */
/** @} */
