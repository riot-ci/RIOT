/*
 * Copyright (C) 2018-20 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>

#include "event.h"
#include "log.h"
#include "net/dhcpv6/client.h"
#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib/ft.h"
#include "net/gnrc/netif/internal.h"

#include "net/gnrc/dhcpv6/client/6lbr.h"

static char _stack[DHCPV6_CLIENT_STACK_SIZE];

static gnrc_netif_t *_find_upstream_netif(void)
{
    gnrc_netif_t *netif = NULL;

    if (CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM) {
        return gnrc_netif_get_by_pid(CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM);
    }
    while ((netif = gnrc_netif_iter(netif))) {
        if (!gnrc_netif_is_6lo(netif)) {
            LOG_WARNING("DHCPv6: Selecting interface %d as upstream\n",
                        netif->pid);
            return netif;
        }
    }
    return NULL;
}

static void _configure_upstream_netif(gnrc_netif_t *upstream_netif)
{
    ipv6_addr_t addr = {
            .u8 = { 0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
        };

    /* set default route to host machine (as set-up in setup_network.sh) */
    gnrc_ipv6_nib_ft_add(NULL, 0, &addr, upstream_netif->pid, 0);
    /* set additional link-local address to allow for easy static route
     * configuration */
    addr.u8[15] = 2;
    gnrc_netif_ipv6_addr_add(upstream_netif, &addr, 64, 0);
}

static void _configure_dhcpv6_client(void)
{
    gnrc_netif_t *netif = NULL;
    while ((netif = gnrc_netif_iter(netif))) {
        if (gnrc_netif_is_6lo(netif)) {
            dhcpv6_client_req_ia_pd(netif->pid, 64U);
        }
    }
}

static void *_dhcpv6_cl_6lbr_thread(void *args)
{
    event_queue_t event_queue;
    gnrc_netif_t *upstream_netif = _find_upstream_netif();

    (void)args;
    if (upstream_netif == NULL) {
        LOG_ERROR("DHCPv6: No upstream interface found!\n");
        return NULL;
    }
    _configure_upstream_netif(upstream_netif);
    /* initialize client event queue */
    event_queue_init(&event_queue);
    /* initialize DHCPv6 client on border interface */
    dhcpv6_client_init(&event_queue, upstream_netif->pid);
    /* configure client to request prefix delegation for WPAN interfaces */
    _configure_dhcpv6_client();
    /* start DHCPv6 client */
    dhcpv6_client_start();
    /* start event loop of DHCPv6 client */
    event_loop(&event_queue);   /* never returns */
    return NULL;
}

void gnrc_dhcpv6_client_6lbr_init(void)
{
    /* start DHCPv6 client thread to request prefix for WPAN */
    thread_create(_stack, DHCPV6_CLIENT_STACK_SIZE,
                  DHCPV6_CLIENT_PRIORITY,
                  THREAD_CREATE_STACKTEST,
                  _dhcpv6_cl_6lbr_thread, NULL, "dhcpv6-client");
}

/** @} */
