/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  net_gnrc_ipv6_nib
 * @internal
 * @{
 *
 * @file
 * @brief       Internal definitions
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef GNRC_IPV6_NIB_INTERNAL_H_
#define GNRC_IPV6_NIB_INTERNAL_H_

#include <stdbool.h>
#include <stdint.h>

#include "evtimer/msg.h"
#include "kernel_types.h"
#include "mutex.h"
#include "net/eui64.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/nib/nc.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/pktqueue.h"
#include "net/ndp.h"
#include "random.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @{
 * @brief   Mode flags for entries
 */
#define _EMPTY  (0x00)      /**< empty */
#define _NC     (0x01)      /**< neighbor cache */
#define _DC     (0x02)      /**< destination cache */
#define _PL     (0x04)      /**< prefix list */
#define _DRL    (0x08)      /**< default router list */
#define _FT     (0x10)      /**< forwarding table */
#define _DAD    (0x20)      /**< 6LoWPAN duplicate address detection table */
/** @} */

/**
 * @brief   Shorter name for convenience ;-)
 */
#define _NIB_IF_MASK        (GNRC_IPV6_NIB_NC_INFO_IFACE_MASK)

/**
 * @brief   Shorter name for convenience ;-)
 */
#define _NIB_IF_POS         (GNRC_IPV6_NIB_NC_INFO_IFACE_POS)

/**
 * @brief   On-link NIB entry
 */
typedef struct _nib {
    struct _nib *next;                  /**< next removable entry */
#if GNRC_IPV6_NIB_CONF_QUEUE_PKT || defined(DOXYGEN)
    /**
     * @brief   queue for packets currently in address resolution
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_QUEUE_PKT != 0
     */
    gnrc_pktqueue_t *pktqueue;
#endif
    /**
     * @brief Neigbours IPv6 address
     */
    ipv6_addr_t ipv6;
#if GNRC_IPV6_NIB_CONF_6LN || defined(DOXYGEN)
    /**
     * @brief   The neighbors EUI-64 (used for DAD)
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_6LN != 0
     */
    eui64_t eui64;
#endif
#if GNRC_IPV6_NIB_CONF_ARSM || defined(DOXYGEN)
    /**
     * @brief   Link-layer address of _nib_t::next_hop
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ARSM != 0
     */
    uint8_t l2addr[GNRC_IPV6_NIB_L2ADDR_MAX_LEN];
    evtimer_msg_event_t reach_timeout;  /**< Event for @ref GNRC_IPV6_NIB_REACH_TIMEOUT */
    evtimer_msg_event_t delay_timeout;  /**< Event for @ref GNRC_IPV6_NIB_DELAY_TIMEOUT */
#endif

    /**
     * @brief   Information flags (see @ref net_gnrc_ipv6_nib_nc_info "NC info values")
     */
    uint16_t info;
    uint8_t mode;                       /**< NIB mode */
#if GNRC_IPV6_NIB_CONF_ARSM || defined(DOXYGEN)
    /**
     * @brief   Neighbor solicitations sent for probing
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ARSM != 0
     */
    uint8_t ns_sent;

    /**
     * @brief   length of _nib_t::l2addr
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ARSM != 0
     */
    uint8_t l2addr_len;
#endif
} _nib_t;

/**
 * @brief   Default route NIB entry
 */
typedef struct {
    _nib_t *next_hop;   /**< next hop to destination */
    uint16_t ltime;     /**< lifetime in seconds */
} _nib_dr_t;

/**
 * @brief   Off-link NIB entry
 */
typedef struct {
    _nib_t *next_hop;           /**< next hop to destination */
    ipv6_addr_t pfx;            /**< prefix to the destination */
    unsigned pfx_len;           /**< prefix-length of _nib_t::dst */
} _nib_dst_t;

/**
 * @brief   Interface specific information for Neighbor Discovery
 */
typedef struct {
#if GNRC_IPV6_NIB_CONF_ARSM
    uint32_t reach_time_base;           /**< base for random reachable time calculation */
    uint32_t reach_time;                /**< reachable time (in ms) */
    uint32_t retrans_time;              /**< retransmission time (in ms) */
#endif
#if GNRC_IPV6_NIB_CONF_ROUTER || defined(DOXYGEN)
    /**
     * @brief   timestamp in milliseconds of last unsolicited router advertisement
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ROUTER
     */
    uint32_t last_ra;
#endif
    kernel_pid_t pid;                   /**< identifier of the interface */
#if GNRC_IPV6_NIB_CONF_ROUTER || defined(DOXYGEN)
    /**
     * @brief   number of unsolicited router advertisements sent
     *
     * This only counts up to the first @ref NDP_MAX_INIT_RA_NUMOF on interface
     * initialization. The last @ref NDP_MAX_FIN_RA_NUMOF of an advertising
     * interface are counted from UINT8_MAX - @ref NDP_MAX_FIN_RA_NUMOF + 1.
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_CONF_ROUTER
     */
    uint8_t ra_sent;
#endif
    uint8_t rs_sent;                    /**< number of unsolicited router solicitations scheduled */
    /**
     * @brief   number of unsolicited neighbor advertisements scheduled
     */
    uint8_t na_sent;
} _nib_iface_t;

/**
 * @brief   Mutex for locking the NIB.
 */
extern mutex_t _nib_mutex;

/**
 * @brief   Event timer for the NIB.
 */
extern evtimer_msg_t _nib_evtimer;

/**
 * @brief   Initializes NIB internally
 */
void _nib_init(void);

/**
 * @brief   Get interface identifier from a NIB entry
 *
 * @param[in] nib   A NIB entry
 *
 * @return  The NIB entry's interface identifier
 */
static inline unsigned _nib_get_if(const _nib_t *nib)
{
    return (nib->info & _NIB_IF_MASK) >> _NIB_IF_POS;
}

/**
 * @brief   Set interface for a NIB entry
 *
 * @param[in,out] nib   A NIB entry
 * @param[in] iface     An interface identifier
 */
static inline void _nib_set_if(_nib_t *nib, unsigned iface)
{
    assert(iface <= 0x1f);
    nib->info &= ~(_NIB_IF_MASK);
    nib->info |= ((iface << _NIB_IF_POS) & _NIB_IF_MASK);
}

/**
 * @brief   Creates or gets an existing on-link entry by address
 *
 * @pre     `(addr != NULL)`
 *
 * @param[in] addr  An IPv6 address. May not be NULL.
 *                  *May also be a global address!*
 * @param[in] iface The interface to the node.
 *
 * @return  A new or existing on-link entry with _nib_t::ipv6 set to @p addr.
 * @return  NULL, if no space is left.
 */
_nib_t *_nib_alloc(const ipv6_addr_t *addr, unsigned iface);

/**
 * @brief   Clears out a NIB entry (on-link version)
 *
 * @param[in] nib  An entry
 *
 * @return  true, if entry was cleared
 * @return  false, if entry was not cleared
 */
static inline bool _nib_clear(_nib_t *nib)
{
    if (nib->mode == _EMPTY) {
        memset(nib, 0, sizeof(_nib_t));
        return true;
    }
    return false;
}

/**
 * @brief   Iterate over on-link entries
 *
 * @param[in] last  Last entry (NULL to start)
 *
 * @return  entry after @p last
 */
_nib_t *_nib_iter(const _nib_t *last);

/**
 * @brief   Gets a node by IPv6 address and interface
 *
 * @pre     `(addr != NULL)`
 *
 * @param[in] addr  The address of a node. May not be NULL.
 * @param[in] iface The interface to the node. May be 0 for any interface
 *
 * @return  The NIB entry for node with @p addr and @p iface on success.
 * @return  NULL, if there is no such entry.
 */
_nib_t *_nib_get(const ipv6_addr_t *addr, unsigned iface);

/**
 * @brief   Adds a node to the neighbor cache
 *
 * @pre     `(addr != NULL)`
 * @pre     `(cstate != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY)`
 * @pre     `(cstate != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE)`
 * @pre     `(cstate != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE)`
 *
 * @param[in] addr      The address of a node. May not be NULL.
 * @param[in] iface     The interface to the node.
 * @param[in] cstate    Creation state. State of the entry *if* the entry is
 *                      newly created.
 * @return  The NIB entry for the new neighbor cache on success.
 * @return  NULL, if there is no space left.
 */
_nib_t *_nib_nc_add(const ipv6_addr_t *addr, unsigned iface, uint16_t cstate);

/**
 * @brief   Removes a node from the neighbor cache
 *
 * @param[in] nib   A node.
 */
void _nib_nc_remove(_nib_t *nib);

/**
 * @brief   Sets a NUD-managed neighbor cache entry to reachable and sets the
 *          respective event in @ref _nib_evtimer "event timer"
 *          (@ref GNRC_IPV6_NIB_MSG_NUD_SET_STALE)
 *
 * @param[in] nib   A node.
 */
void _nib_nc_set_reachable(_nib_t *nib);

/**
 * @brief   Get external neighbor cache entry representation from on-link entry
 *
 * @param[in] nib   On-link entry.
 * @param[out] nce  External representation of the neighbor cache
 *                  representation.
 */
void _nib_nc_get(const _nib_t *nib, gnrc_ipv6_nib_nc_t *nce);

/**
 * @brief   Creates or gets an existing default router list entry by address
 *
 * @pre     `(router_addr != NULL)`
 *
 * @param[in] addr  An IPv6 address. May not be NULL.
 *                  *May also be a global address!*
 * @param[in] iface The interface to the router.
 *
 * This creates or gets an existing default router entry by address. Older
 * on-link entries might be cycled out by this for _nib_dr_t::next_hop in a
 * CLOCK-wise manner, if they are non-persistent cache entries.
 *
 * @return  A new or existing default router entry with _nib_t::ipv6 of
 *          _nib_dr_t::next_hop set to @p router_addr.
 * @return  NULL, if no space is left.
 */
_nib_dr_t *_nib_drl_add(const ipv6_addr_t *router_addr, unsigned iface);

/**
 * @brief   Removes a default router list entry.
 *
 * @param[in] nib_dr    An entry
 *
 * Does not clear, if @p entry is persistent. Corresponding non-link entry is
 * removed, too.
 */
void _nib_drl_remove(_nib_dr_t *nib_dr);

/**
 * @brief   Iterate over default router list
 *
 * @param[in] last  Last entry (NULL to start)
 *
 * @return  entry after @p last
 */
_nib_dr_t *_nib_drl_iter(const _nib_dr_t *last);

/**
 * @brief   Gets a default router by IPv6 address and interface
 *
 * @pre     `(router_addr != NULL)`
 *
 * @param[in] router_addr   The address of a default router. May not be NULL.
 * @param[in] iface         The interface to the node. May be 0 for any interface.
 *
 * @return  The NIB entry for node with @p router_addr and @p iface onsuccess.
 * @return  NULL, if there is no such entry.
 */
_nib_dr_t *_nib_drl_get(const ipv6_addr_t *router_addr, unsigned iface);

/**
 * @brief   Gets *the* default router.
 *
 * @see [RFC 4861, section 6.3.6](https://tools.ietf.org/html/rfc4861#section-6.3.6)
 *
 * @return  The current default router, on success.
 * @return  NULL, if there is no default router in the list.
 */
_nib_dr_t *_nib_drl_get_dr(void);

/**
 * @brief   Creates or gets an existing off-link entry by next hop and prefix
 *
 * @pre `(next_hop != NULL)`
 * @pre `(pfx != NULL) && (pfx != "::") && (pfx_len != 0) && (pfx_len <= 128)`
 *
 * @param[in] next_hop  An IPv6 address to next hop. May not be NULL.
 *                      *May also be a global address!*
 * @param[in] iface     The interface to @p next_hop.
 * @param[in] pfx       The IPv6 prefix or address of the destinaion.
 *                      May not be NULL or unspecified address. Use
 *                      @ref _nib_drl_add() for default route destinations.
 * @param[in] pfx_len   The length in bits of @p pfx in bits.
 *
 * @return  A new or existing off-link entry with _nib_dr_t::pfx set to @p pfx.
 * @return  NULL, if no space is left.
 */
_nib_dst_t *_nib_dst_alloc(const ipv6_addr_t *next_hop, unsigned iface,
                           const ipv6_addr_t *pfx, unsigned pfx_len);

/**
 * @brief   Clears out a NIB entry (off-link version)
 *
 * @param[in] dst  An entry
 */
void _nib_dst_clear(_nib_dst_t *dst);

/**
 * @brief   Iterate over off-link entries
 *
 * @param[in] last  Last entry (NULL to start)
 *
 * @return  entry after @p last
 */
_nib_dst_t *_nib_dst_iter(const _nib_dst_t *last);

_nib_dst_t *_nib_dc_add(const ipv6_addr_t *next_hop, unsigned iface,
                        const ipv6_addr_t *dst);

static inline void _nib_dc_remove(_nib_dst_t *nib_dst)
{
    _nib_t *nib = nib_dst->next_hop;

    if (nib != NULL) {
        nib->mode &= ~(_DC);
    }
    _nib_dst_clear(nib_dst);
}

static inline _nib_dst_t *_nib_pl_add(unsigned iface, const ipv6_addr_t *pfx,
                                      unsigned pfx_len)
{
    _nib_dst_t *nib_dst = _nib_dst_alloc(NULL, iface, pfx, pfx_len);
    if (nib_dst != NULL) {
        nib_dst->next_hop->mode |= (_PL);
    }
    return nib_dst;
}

static inline void _nib_pl_remove(_nib_dst_t *nib_dst)
{
    _nib_t *nib = nib_dst->next_hop;

    if (nib != NULL) {
        nib->mode &= ~(_PL);
    }
    _nib_dst_clear(nib_dst);
}

static inline _nib_dst_t *_nib_ft_add(const ipv6_addr_t *next_hop, unsigned iface,
                                      const ipv6_addr_t *pfx, unsigned pfx_len)
{
    _nib_dst_t *nib_dst = _nib_dst_alloc(next_hop, iface, pfx, pfx_len);
    if (nib_dst != NULL) {
        nib_dst->next_hop->mode |= (_FT);
    }
    return nib_dst;
}

static inline void _nib_ft_remove(_nib_dst_t *nib_dst)
{
    _nib_t *nib = nib_dst->next_hop;

    if (nib != NULL) {
        nib->mode &= ~(_FT);
    }
    _nib_dst_clear(nib_dst);
}

static inline _nib_t *_nib_dad_add(const ipv6_addr_t *addr)
{
    _nib_t *nib = _nib_alloc(addr, 0);
    if (nib != NULL) {
        nib->mode |= (_DAD);
    }
    return nib;
}

static inline void _nib_dad_remove(_nib_t *nib)
{
    nib->mode &= ~(_DAD);
    _nib_clear(nib);
}

/**
 * @brief   Get (or creates if it not exists) interface information for neighbor
 *          discovery
 *
 * @pre `(iface > KERNEL_PID_UNDEF) && (iface <= KERNEL_PID_LAST)`
 *
 * @param[in] iface Interface identifier to get information for
 *
 * @return  Interface information on @p iface
 * @return  NULL, if no space left for interface
 */
_nib_iface_t *_nib_iface_get(unsigned iface);

/**
 * @brief   Looks up if an event is queued in the event timer
 *
 * @param[in] ctx   Context of the event. May be NULL for any event context.
 * @param[in] type  [Type of the event](@ref net_gnrc_ipv6_nib_msg).
 *
 * @return  Milliseconds to the event, if event in queue.
 * @return  UINT32_MAX, event is not in queue.
 */
uint32_t _evtimer_lookup(void *ctx, uint16_t type);

/**
 * @brief   Adds an event to the event timer
 *
 * @param[in] ctx       The context of the event
 * @param[in] type      [Type of the event](@ref net_gnrc_ipv6_nib_msg).
 * @param[in] event     Representation of the event.
 * @param[in] offset    Offset in milliseconds to the event.
 */
static inline void _evtimer_add(void *ctx, int16_t type,
                                evtimer_msg_event_t *event, uint32_t offset)
{
#ifdef MODULE_GNRC_IPV6
    static const kernel_pid_t target_pid = gnrc_ipv6_pid;
#else
    static const kernel_pid_t target_pid = KERNEL_PID_LAST;  /* just for testing */
#endif
    evtimer_del((evtimer_t *)(&_nib_evtimer), (evtimer_event_t *)event);
    assert(event->event.next == NULL);
    event->event.offset = offset;
    event->msg.type = type;
    event->msg.content.ptr = ctx;
    evtimer_add_msg(&_nib_evtimer, event, target_pid);
}

#ifdef __cplusplus
}
#endif

#endif /* GNRC_IPV6_NIB_INTERNAL_H_ */
/** @} */
