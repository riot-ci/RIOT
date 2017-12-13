/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   IPv6 defintions for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_IPV6_H
#define NET_GNRC_NETIF_IPV6_H

#include <assert.h>

#include "evtimer_msg.h"
#include "net/ipv6/addr.h"
#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib/conf.h"
#endif
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif/conf.h"
#ifdef MODULE_NETSTATS_IPV6
#include "net/netstats.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    IPv6 unicast and anycast address flags
 * @anchor  net_gnrc_netif_ipv6_addrs_flags
 * @{
 */
/**
 * @brief   Mask for the address' state
 */
#define GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK             (0x1fU)

/**
 * @brief   Tentative states (with encoded DAD retransmissions)
 *
 * The retransmissions of DAD transmits can be decoded from this state by
 * applying it as a mask to the [flags](gnrc_netif_ipv6_t::addrs_flags) of the
 * address.
 */
#define GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE        (0x07U)

/**
 * @brief   Deprecated address state (still valid, but not preferred)
 */
#define GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_DEPRECATED       (0x08U)

/**
 * @brief   Valid address state
 */
#define GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID            (0x10U)

/**
 * @brief   Address is an anycast address
 */
#define GNRC_NETIF_IPV6_ADDRS_FLAGS_ANYCAST                (0x20U)
/** @} */

/**
 * @brief   IPv6 component for @ref gnrc_netif_t
 *
 * @note only available with @ref net_gnrc_ipv6.
 */
typedef struct {
    /**
     * @brief   Flags for gnrc_netif_t::ipv6_addrs
     *
     * @see net_gnrc_netif_ipv6_addrs_flags
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    uint8_t addrs_flags[GNRC_NETIF_IPV6_ADDRS_NUMOF];

    /**
     * @brief   IPv6 unicast and anycast addresses of the interface
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    ipv6_addr_t addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];

    /**
     * @brief   IPv6 multicast groups of the interface
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    ipv6_addr_t groups[GNRC_NETIF_IPV6_GROUPS_NUMOF];
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
     * The callback may be `NULL` if no such behavior is required by the routing
     * protocol (or no routing protocol is present).
     *
     * @param[in] type      [Type](@ref net_gnrc_ipv6_nib_route_info_type) of
     *                      the route info.
     * @param[in] ctx_addr  Context address of the route info.
     * @param[in] ctx       Further context of the route info.
     */
    void (*route_info_cb)(unsigned type, const ipv6_addr_t *ctx_addr,
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
#if GNRC_IPV6_NIB_CONF_6LN || DOXYGEN
    /**
     * @brief   Timers for address re-registration
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6" and
     *          @ref net_gnrc_ipv6_nib "NIB" and if
     *          @ref GNRC_IPV6_NIB_CONF_6LN != 0
     * @note    Might also be usable in the later default SLAAC implementation
     *          for NS retransmission timers.
     */
    evtimer_msg_event_t addrs_timers[GNRC_NETIF_IPV6_ADDRS_NUMOF];
#endif

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
     *          reachable time in ms (if @ref GNRC_NETIF_FLAGS_IPV6_RTR_ADV is
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
#endif /* MODULE_GNRC_IPV6_NIB */

    /**
     * @brief   IPv6 auto-address configuration mode
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6"
     */
    uint8_t aac_mode;

    /**
     * @brief   Maximum transmission unit (MTU) for IPv6 packets
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv6".
     */
    uint16_t mtu;
} gnrc_netif_ipv6_t;

/**
 * @brief   Gets the (unicast on anycast) IPv6 addresss of an interface (if IPv6
 *          is supported)
 *
 * @pre `netif != NULL`
 * @pre `addrs != NULL`
 * @pre `max_len >= sizeof(ipv6_addr_t)`
 *
 * @param[in] netif     The interface. May not be `NULL`.
 * @param[out] addrs    Up to the first `max_len / sizeof(ipv6_addr_t)`
 *                      addresses assigned to @p netif. Must not be `NULL`
 * @param[in] max_len   Number of *bytes* available in @p addrs. Must be at
 *                      least `sizeof(ipv6_addr_t)`. It is recommended to use
 *                      @p GNRC_NETIF_IPV6_ADDRS_NUMOF `* sizeof(ipv6_addr_t)
 *                      here (and have @p addrs of the according length).
 *
 * @return  Number of addresses in @p addrs times `sizeof(ipv6_addr_t)` on
 *          success (including 0).
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_addrs_get(const gnrc_netif_t *netif,
                                            ipv6_addr_t *addrs,
                                            size_t max_len)
{
    assert(netif != NULL);
    assert(addrs != NULL);
    assert(max_len >= sizeof(ipv6_addr_t));
    return gnrc_netapi_get(netif->pid, NETOPT_IPV6_ADDR, 0, addrs, max_len);
}

/**
 * @brief   Adds an (unicast or anycast) IPv6 address to an interface (if IPv6
 *          is supported)
 *
 * @pre `netif != NULL`
 * @pre `addr != NULL`
 * @pre `(pfx_len > 0) && (pfx_len <= 128)`
 *
 * @param[in] netif     The interface. May not be `NULL`.
 * @param[in] addr      The address to add to @p netif. May not be `NULL`.
 * @param[in] pfx_len   The prefix length of @p addr. Must be greater than 0 and
 *                      lesser than or equal to 128.
 * @param[in] flags     [Flags](@ref net_gnrc_netif_ipv6_addrs_flags) for
 *                      @p addr. Set @ref GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID
 *                      to skip duplicate address detection (when activated).
 *
 * @return  sizeof(ipv6_addr_t) on success.
 * @return  -ENOMEM, if no space is left on @p netif to add @p addr or its
 *          corresponding solicited-nodes multicast address.
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_addr_add(const gnrc_netif_t *netif,
                                           const ipv6_addr_t *addr,
                                           unsigned pfx_len, uint8_t flags)
{
    assert(netif != NULL);
    assert(addr != NULL);
    assert((pfx_len > 0) && (pfx_len <= 128));
    return gnrc_netapi_set(netif->pid, NETOPT_IPV6_ADDR,
                           ((pfx_len << 8U) | flags), addr,
                           sizeof(ipv6_addr_t));
}

/**
 * @brief   Removes a (unicast or anycast) IPv6 address from an interface (if
 *          IPv6 is supported)
 *
 * @pre `netif != NULL`
 * @pre `addr != NULL`
 *
 * @param[in] netif     The interface. May not be `NULL`.
 * @param[in] addr      The address to remove from @p netif. May not be `NULL`.
 *
 * @return  sizeof(ipv6_addr_t) on success.
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_addr_remove(const gnrc_netif_t *netif,
                                              const ipv6_addr_t *addr)
{
    assert(netif != NULL);
    assert(addr != NULL);
    return gnrc_netapi_set(netif->pid, NETOPT_IPV6_ADDR_REMOVE,
                           0, addr, sizeof(ipv6_addr_t));
}

/**
 * @brief   Gets the IPv6 multicast groups an interface is joined to (if IPv6 is
 *          supported)
 *
 * @pre `netif != NULL`
 * @pre `groups != NULL`
 * @pre `max_len >= sizeof(ipv6_addr_t)`
 *
 * @param[in] netif     The interface. May not be `NULL`.
 * @param[out] groups   Up to the first `max_len / 8` multicast groups @p netif
 *                      is joined to. Must not be `NULL`
 * @param[in] max_len   Number of *bytes* available in @p groups. Must be at
 *                      least `sizeof(ipv6_addr_t)`. It is recommended to use
 *                      @p GNRC_NETIF_IPV6_GROUPS_NUMOF `* sizeof(ipv6_addr_t)
 *                      here (and have @p groups of the according length).
 *
 * @return  Number of addresses in @p groups times `sizeof(ipv6_addr_t)` on
 *          success (including 0).
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_groups_get(const gnrc_netif_t *netif,
                                             ipv6_addr_t *groups,
                                             size_t max_len)
{
    assert(netif != NULL);
    assert(groups != NULL);
    assert(max_len >= sizeof(ipv6_addr_t));
    return gnrc_netapi_get(netif->pid, NETOPT_IPV6_GROUP, 0, addrs, max_len);
}

/**
 * @brief   Joins an IPv6 multicast group on an interface (if IPv6 is supported)
 *
 * @pre `netif != NULL`
 * @pre `group != NULL`
 *
 * @param[in] netif     The interface.
 * @param[in] group     The address of the multicast group to join on @p netif.
 *
 * @return  sizeof(ipv6_addr_t) on success.
 * @return  -ENOMEM, if no space is left on @p netif to add @p group.
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_group_join(const gnrc_netif_t *netif,
                                             const ipv6_addr_t *group)
{
    assert(netif != NULL);
    assert(group != NULL);
    return gnrc_netapi_set(netif->pid, NETOPT_IPV6_GROUP, 0, group,
                           sizeof(ipv6_addr_t));
}

/**
 * @brief   Leaves an IPv6 multicast group on an interface (if IPv6 is supported)
 *
 * @pre `netif != NULL`
 * @pre `group != NULL`
 *
 * @param[in] netif     The interface.
 * @param[in] group     The address of the multicast group to leave on @p netif.
 *
 * @return  sizeof(ipv6_addr_t) on success.
 * @return  -ENOTSUP, if @p netif doesn't support IPv6.
 */
static inline int gnrc_netif_ipv6_group_leave(const gnrc_netif_t *netif,
                                              const ipv6_addr_t *group)
{
    assert(netif != NULL);
    assert(group != NULL);
    return gnrc_netapi_set(netif->pid, NETOPT_IPV6_GROUP_LEAVE, 0, group,
                           sizeof(ipv6_addr_t));
}

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_IPV6_H */
/** @} */
