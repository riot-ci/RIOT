/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup net_dhcpv6_client  DHCPv6 client
 * @ingroup  net_dhcpv6
 * @brief   DHCPv6 client implementation
 * @todo    Make GNRC-independent
 * @{
 *
 * @file
 * @brief   DHCPv6 client defintions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_DHCPV6_CLIENT_H
#define NET_DHCPV6_CLIENT_H

#include "event.h"
#include "net/gnrc/netif/conf.h"
#include "net/dhcpv6.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Auto-initialization parameters
 */
#ifndef DHCPV6_CLIENT_STACK_SIZE
#define DHCPV6_CLIENT_STACK_SIZE    (THREAD_STACKSIZE_DEFAULT)  /**< stack size */
#endif

#ifndef DHCPV6_CLIENT_PRIORITY
#define DHCPV6_CLIENT_PRIORITY      (THREAD_PRIORITY_MAIN - 2)  /**< priority */
#endif
/** @} */

/**
 * @brief   Static length of the DUID
 */
#define DHCPV6_CLIENT_DUID_LEN      (sizeof(dhcpv6_duid_l2_t) + GNRC_NETIF_L2ADDR_MAXLEN)
#define DHCPV6_CLIENT_BUFLEN        (92)    /**< length for send and receive buffer */
#ifndef DHCPV6_CLIENT_SERVER_MAX
#define DHCPV6_CLIENT_SERVER_MAX    (1U)    /**< maximum number of servers to store */
#endif
#ifndef DHCPV6_CLIENT_PFX_LEASE_MAX
#define DHCPV6_CLIENT_PFX_LEASE_MAX (1U)    /**< maximum number of prefix leases to store */
#endif

#if defined(MODULE_AUTO_INIT_DHCPV6_CLIENT) || defined(DOXYGEN)
/**
 * @brief   Auto-initializes the client in its own thread
 *
 * @note    Only available with (and called by) the `auto_init_dhcpv6_client`
 *          module.
 */
void dhcpv6_client_auto_init(void);
#endif /* MODULE_AUTO_INIT_DHCPV6_CLIENT */

/**
 * @brief   Initializes the client
 *
 * @pre `event_queue->waiter != NULL`
 *
 * @param[in] event_queue   Event queue to use with the client. Needs to be
 *                          initialized in the handler thread.
 * @param[in] netif         The network interface the client should listen on.
 *                          SOCK_ADDR_ANY_NETIF for any interface
 */
void dhcpv6_client_init(event_queue_t *event_queue, uint16_t netif);

/**
 * @brief   Let the server start listening
 *
 * This needs to be called *after* all desired [configuration functions]
 * (@ref net_dhcpv6_client_conf) where called.
 */
void dhcpv6_client_start(void);

/**
 * @name    Configuration functions
 * @anchor  net_dhcpv6_client_conf
 * @{
 */
/**
 * @brief   Configures the client to request prefix delegation for a network
 *          interface from a server
 *
 * @param[in] netif     The interface to request the prefix delegation for.
 * @param[in] pfx_len   The desired length of the prefix (note that the server
 *                      might not consider this request).
 */
void dhcpv6_client_req_ia_pd(uint16_t netif, uint8_t pfx_len);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_DHCPV6_CLIENT_H */
/** @} */
