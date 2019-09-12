/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_ext_frag Support for IPv6 fragmentation extension
 * @ingroup     net_gnrc_ipv6_ext
 * @brief       GNRC implementation of IPv6 fragmentation extension
 * @{
 *
 * @file
 * @brief   GNRC fragmentation extension definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_EXT_FRAG_H
#define NET_GNRC_IPV6_EXT_FRAG_H

#include <stdbool.h>
#include <stdint.h>

#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type to send further fragments of a IPv6 packet
 */
#define GNRC_IPV6_EXT_FRAG_SEND         (0xfe01U)   /**< continue fragmenting */
#define GNRC_IPV6_EXT_FRAG_SEND_FRAG    (0xfe02U)   /**< send fragment */

/**
 * @brief   Fragmentation send buffer type
 */
typedef struct {
    gnrc_pktsnip_t *pkt;            /**< the IPv6 packet to fragment */
    gnrc_pktsnip_t *per_frag;       /**< per fragment headers */
    uint32_t id;                    /**< the identification for the fragment header */
    uint16_t path_mtu;              /**< path MTU to destination of
                                     *   gnrc_ipv6_ext_frag_send_t::pkt */
    uint16_t offset;                /**< current fragmentation offset */
} gnrc_ipv6_ext_frag_send_t;

/**
 *  * @brief   Initializes IPv6 fragmentation and reassembly
 *   * @internal
 *    */
void gnrc_ipv6_ext_frag_init(void);

/**
 * @brief   Send an IPv6 packet fragmented
 *
 * @param[in] pkt       The IPv6 packet. The packet must have an already
 *                      prepared @ref GNRC_NETTYPE_NETIF snip as its first
 *                      snip. The packet must contain at least an IPv6 header
 *                      and any number of IPv6 extension headers after that.
 * @param[in] path_mtu  Path MTU to destination of IPv6 packet.
 */
void gnrc_ipv6_ext_frag_send_pkt(gnrc_pktsnip_t *pkt, unsigned path_mtu);

/**
 * @brief   Fragment packet already in fragmentation send buffer
 *
 * @pre `snd_buf != NULL`
 *
 * @param[in,out] snd_buf   A fragmentation send buffer entry. May not be NULL.
 */
void gnrc_ipv6_ext_frag_send(gnrc_ipv6_ext_frag_send_t *snd_buf);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_EXT_FRAG_H */
/** @} */
