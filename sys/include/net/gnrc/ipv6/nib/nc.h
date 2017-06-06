/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_nib_nc   Neighbor Cache
 * @ingroup     net_gnrc_ipv6_nib
 * @brief       Neighbor cache component of network information base
 * @{
 *
 * @file
 * @brief       Neighbor cache definitions
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef GNRC_IPV6_NIB_NC_H_
#define GNRC_IPV6_NIB_NC_H_

#include <stdbool.h>
#include <stdint.h>

#include "net/eui64.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Values for gnrc_ipv6_nib_nc_t::info
 * @anchor  net_gnrc_ipv6_nib_nc_info
 * @name    Info values
 * @{
 */
/**
 * @brief   Mask for neighbor unreachability detection (NUD) states
 *
 * @see [RFC 4861, section 7.3.2](https://tools.ietf.org/html/rfc4861#section-7.3.2)
 * @see [RFC 7048](https://tools.ietf.org/html/rfc7048)
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK            (0x0007)
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED       (0x0000)    /**< not managed by NUD */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE     (0x0001)    /**< entry is not reachable */

/**
 * @brief   address resolution is currently performed
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE      (0x0002)

/**
 * @brief   address might not be reachable
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE           (0x0003)

/**
 * @brief   NUD will be performed in a moment
 */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY           (0x0004)
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE           (0x0005)    /**< NUD is performed */
#define GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE       (0x0006)    /**< entry is reachable */

/**
 * @brief   gnrc_ipv6_nib_t::next_hop is router
 *
 * This flag indicates that does not indicate that gnrc_ipv6_nib_t::next_hop is
 * a router, but it does not necessarily indicate that it is in the default
 * router list! A neighbor that has this flag unset however **must not** appear
 * in the default router list.
 *
 * @see [RFC 4861, Appendix D](https://tools.ietf.org/html/rfc4861#page-91)
 */
#define GNRC_IPV6_NIB_NC_INFO_IS_ROUTER                 (0x0008)

/**
 * @brief   Mask for interface identifier
 */
#define GNRC_IPV6_NIB_NC_INFO_IFACE_MASK                (0x01f0)

/**
 * @brief   Shift position of interface identifier
 */
#define GNRC_IPV6_NIB_NC_INFO_IFACE_POS                 (4)

/**
 * @brief Mask for 6LoWPAN address registration (6Lo-AR) states
 *
 * @see [RFC 6775, section 3.5](https://tools.ietf.org/html/rfc6775#section-3.5)
 */
#define GNRC_IPV6_NIB_NC_INFO_AR_STATE_MASK             (0x0600)

/**
 * @brief   not managed by 6Lo-AR (address can be removed when memory is low
 */
#define GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC               (0x0000)

/**
 * @brief   address registration still pending at upstream router
 */
#define GNRC_IPV6_NIB_NC_INFO_AR_STATE_TENTATIVE        (0x0200)
#define GNRC_IPV6_NIB_NC_INFO_AR_STATE_REGISTERED       (0x0400)    /**< address is registered */
#define GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL           (0x0600)    /**< added manually */
/** @} */

/**
 * @brief   Neighbor cache entry view on NIB
 */
typedef struct {
    ipv6_addr_t ipv6;                               /**< neighbor's IPv6 address */
    uint8_t l2addr[GNRC_IPV6_NIB_L2ADDR_MAX_LEN];   /**< neighbor's link-local address */
    /**
     * @brief   neighbor information as defined in @ref net_gnrc_ipv6_nib_nc_info "info values"
     */
    uint16_t info;
    uint8_t l2addr_len;                             /**< length of gnrc_ipv6_nib_nc_t::l2addr */
} gnrc_ipv6_nib_nc_t;

/**
 * @brief   Get state of neighbor unreachability state from entry
 *
 * @param[in] entry     A neighbor cache entry.
 *
 * @return  The neighbor unreachability state of @p entry.
 */
static inline unsigned gnrc_ipv6_nib_nc_get_nud_state(const gnrc_ipv6_nib_nc_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK);
}

/**
 * @brief   Get state of neighbor unreachability state of an entry
 *
 * @param[in] entry     A neighbor cache entry.
 *
 * @return  true, if @p entry is a router.
 * @return  false, if @p entry is not a router.
 */
static inline bool gnrc_ipv6_nib_nc_is_router(const gnrc_ipv6_nib_nc_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_NC_INFO_IS_ROUTER);
}

/**
 * @brief   Get interface from entry
 *
 * @param[in] entry     A neighbor cache entry
 *
 * @return  The interface identifier of @p entry.
 * @return  0 if no interface is identify for @p entry.
 */
static inline unsigned gnrc_ipv6_nib_nc_get_iface(const gnrc_ipv6_nib_nc_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_NC_INFO_IFACE_MASK) >>
           GNRC_IPV6_NIB_NC_INFO_IFACE_POS;
}

/**
 * @brief   Get address registration state of an entry
 *
 * @param[in] entry     A neighbor cache entry
 *
 * @return  The address registration state of @p entry.
 */
static inline unsigned gnrc_ipv6_nib_nc_get_ar_state(const gnrc_ipv6_nib_nc_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_NC_INFO_AR_STATE_MASK);
}

/**
 * @brief   Adds an unmanaged neighbor entry to NIB
 *
 * @pre `(ipv6 != NULL) && (l2addr != NULL)`
 * @pre `l2addr_len <= GNRC_IPV6_NIB_L2ADDR_MAX_LEN`
 * @pre `iface > KERNEL_PID_LAST`
 *
 * @param[in] ipv6          The neighbor's IPv6 address.
 * @param[in] iface         The interface to the neighbor.
 * @param[in] l2addr        The neighbor's L2 address.
 * @param[in] l2addr_len    Length of @p l2addr.
 *
 * A neighbor cache entry created this way is marked as persistent.
 * Also, a non-persistent neighbor or destination cache entry already in the
 * NIB might be removed to make room for the new entry.
 * If an entry pointing to the same IPv6 address as @p ipv6 exists already it
 * will be overwritten and marked as unmanaged.
 *
 * If @ref GNRC_IPV6_NIB_CONF_ARSM != 0 @p l2addr and @p l2addr_len won't be set.
 *
 * @return  0 on success.
 * @return  -ENOMEM, if no space is left in neighbor cache.
 */
int gnrc_ipv6_nib_nc_set(const ipv6_addr_t *ipv6, unsigned iface,
                         const uint8_t *l2addr, size_t l2addr_len);

/**
 * @brief   Deletes neighbor with address @p ipv6 from NIB
 *
 * @pre `ipv6 != NULL`
 *
 * @param[in] ipv6 The neighbor's IPv6 address.
 *
 * If the @p ipv6 can't be found for a neighbor in the NIB nothing happens.
 */
void gnrc_ipv6_nib_nc_del(const ipv6_addr_t *ipv6);

/**
 * @brief   Mark neighbor with address @p ipv6 as reachable
 *
 * @pre `ipv6 != NULL`
 *
 * @param[in] ipv6 A neighbor's IPv6 address. May not be NULL.
 *
 * This function shall be called if an upper layer gets reachability confirmation
 * via its own means (e.g. a TCP connection build-up or confirmation). Unmanaged
 * neighbor cache entries (i.e. entries created using
 * @ref gnrc_ipv6_nib_nc_set()) or entries which next-hop are not in the
 * neighbor cache yet are ignored.
 *
 * Entries in state @ref GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED are not
 * affected by this, since they are assumed to always be reachable and kept out
 * of the NUD state-machine
 */
void gnrc_ipv6_nib_nc_mark_reachable(const ipv6_addr_t *ipv6);

/**
 * @brief   Iterates over all neighbor cache entries in the NIB
 *
 * @pre (state != NULL) && (entry != NULL)
 *
 * @param[in] iface     Restrict iteration to entries on this interface.
 *                      0 for any interface.
 * @param[in,out] state Iteration state of the neighbor cache. Must point to
 *                      a NULL pointer to start iteration.
 * @param[out] entry    The next neighbor cache entry
 *
 * Usage example:
 *
 * ```C
 * #include "net/gnrc/ipv6/nib/nc.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     gnrc_ipv6_nib_nc_t entry;
 *
 *     puts("My neighbors:");
 *     while (gnrc_ipv6_nib_nc_iter(0, &state, &entry)) {
 *         gnrc_ipv6_nib_nc_print(&entry);
 *     }
 *     return 0;
 * }
 * ```
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p entry is the last neighbor cache entry in the NIB.
 */
bool gnrc_ipv6_nib_nc_iter(unsigned iface, void **state,
                           gnrc_ipv6_nib_nc_t *entry);

/**
 * @brief   Prints a neighbor cache entry
 *
 * @param[in] entry A neighbor cache entry
 */
void gnrc_ipv6_nib_nc_print(gnrc_ipv6_nib_nc_t *entry);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_IPV6_NIB_NC_H_ */
/** @} */
