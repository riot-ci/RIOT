/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif2
 * @{
 *
 * @file
 * @brief   IPv6 defintions for @ref net_gnrc_netif2
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF2_IPV6_H
#define NET_GNRC_NETIF2_IPV6_H

#include "evtimer_msg.h"
#include "net/ipv6/addr.h"
#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib/conf.h"
#endif
#include "net/gnrc/netif2/conf.h"
#ifdef MODULE_NETSTATS_IPV6
#include "net/netstats.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    IPv6 unicast and anycast address flags
 * @anchor  net_gnrc_netif2_ipv6_addrs_flags
 * @{
 */
/**
 * @brief   Mask for the address' state
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_STATE_MASK         (0x03U)

/**
 * @brief   Tentative address state
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_STATE_TENTATIVE    (0x01U)

/**
 * @brief   Deprecated address state (still valid, but not preferred)
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_STATE_DEPRECATED   (0x02U)

/**
 * @brief   Valid address state
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_STATE_VALID        (0x03U)

/**
 * @brief   Address is an anycast address
 */
#define GNRC_NETIF2_IPV6_ADDRS_FLAGS_ANYCAST            (0x04U)
/** @} */

/**
 * @brief   IPv6 component for @ref gnrc_netif2_t
 *
 * @note only available with @ref net_gnrc_ipv6.
 */
typedef struct {
    /**
     * @brief   Flags for gnrc_netif2_t::ipv6_addrs
     *
     * @see net_gnrc_netif2_ipv6_addrs_flags
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    uint8_t addrs_flags[GNRC_NETIF2_IPV6_ADDRS_NUMOF];

    /**
     * @brief   IPv6 unicast and anycast addresses of the interface
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    ipv6_addr_t addrs[GNRC_NETIF2_IPV6_ADDRS_NUMOF];

    /**
     * @brief   IPv6 multicast groups of the interface
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    ipv6_addr_t groups[GNRC_NETIF2_IPV6_GROUPS_NUMOF];
#ifdef MODULE_NETSTATS_IPV6
    /**
     * @brief IPv6 packet statistics
     *
     * @note    Only available with module `netstats_ipv6`.
     */
    netstats_t stats;
#endif
#if defined(MODULE_GNRC_IPV6_NIB) || DOXYGEN
#if GNRC_IPV6_NIB_CONF_ROUTER || DOXYGEN
    /**
     * @brief   Route info callback
     *
     * This callback is called by the @ref net_gnrc_ipv6_nib "NIB" to inform
     * the routing protocol about state changes, route usages, missing routes
     * etc.
     *
     * @todo    Define types (RRQ, RRN, NSC) in NIB
     *
     * @param[in] type      Type of the route info.
     * @param[in] ctx_addr  Context address of the route info.
     * @param[in] ctx       Further context of the route info.
     */
    int (*route_info_cb)(unsigned type, const ipv6_addr_t *ctx_addr,
                         const void *ctx);
    /**
     * @brief   Event for @ref GNRC_IPV6_NIB_SND_MC_RA
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ROUTER != 0
     */
    evtimer_msg_event_t snd_mc_ra;
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
#if GNRC_IPV6_NIB_CONF_ARSM || DOXYGEN
    /**
     * @brief   Event for @ref GNRC_IPV6_NIB_RECALC_REACH_TIME
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ARSM != 0
     */
    evtimer_msg_event_t recalc_reach_time;
#endif /* GNRC_IPV6_NIB_CONF_ARSM */
    /**
     * @brief   Event for @ref GNRC_IPV6_NIB_SEARCH_RTR
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB"
     */
    evtimer_msg_event_t search_rtr;

#if GNRC_IPV6_NIB_CONF_ROUTER || DOXYGEN
    /**
     * @brief   Timestamp in milliseconds of last unsolicited router
     *          advertisement
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ROUTER != 0
     */
    uint32_t last_ra;
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
#if GNRC_IPV6_NIB_CONF_ARSM || defined(DOXYGEN)
    /**
     * @brief   Base for random reachable time calculation and advertised
     *          reachable time in ms (if @ref GNRC_NETIF2_FLAGS_IPV6_RTR_ADV is
     *          set)
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ARSM != 0
     */
    uint32_t reach_time_base;

    /**
     * @brief   Reachable time (in ms)
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ARSM != 0
     */
    uint32_t reach_time;
#endif /* GNRC_IPV6_NIB_CONF_ARSM */
    /**
     * @brief   Retransmission time and advertised retransmission time (in ms)
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6" and
     *          @ref net_gnrc_ipv6_nib "NIB"
     */
    uint32_t retrans_time;
#if GNRC_IPV6_NIB_CONF_ROUTER || DOXYGEN
    /**
     * @brief   (Advertised) Router lifetime (default 1800).
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ROUTER != 0
     */
    uint16_t rtr_ltime;
    /**
     * @brief   number of unsolicited router advertisements sent
     *
     * This only counts up to the first @ref NDP_MAX_INIT_RA_NUMOF on interface
     * initialization. The last @ref NDP_MAX_FIN_RA_NUMOF of an advertising
     * interface are counted from UINT8_MAX - @ref NDP_MAX_FIN_RA_NUMOF + 1.
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     *          and @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_ROUTER != 0
     */
    uint8_t ra_sent;
#endif
    /**
     * @brief   number of unsolicited router solicitations scheduled
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6" and
     *          @ref net_gnrc_ipv6_nib "NIB"
     */
    uint8_t rs_sent;
    /**
     * @brief   number of unsolicited neighbor advertisements scheduled
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6" and
     *          @ref net_gnrc_ipv6_nib "NIB"
     */
    uint8_t na_sent;
#endif

    /**
     * @brief   IPv6 auto-address configuration mode
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6"
     */
    uint8_t aac_mode;

    /**
     * @brief   Maximum transition unit (MTU) for IPv6 packets
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    uint16_t mtu;
} gnrc_netif2_ipv6_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF2_IPV6_H */
/** @} */
