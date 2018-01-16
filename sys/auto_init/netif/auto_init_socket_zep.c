/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for @ref netdev_socket_zep devices
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#ifdef MODULE_SOCKET_ZEP

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "socket_zep.h"
#include "socket_zep_params.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/netdev/ieee802154.h"


/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define SOCKET_ZEP_MAC_STACKSIZE    (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef SOCKET_ZEP_MAC_PRIO
#define SOCKET_ZEP_MAC_PRIO         (GNRC_NETDEV_MAC_PRIO)
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev_ieee802154_stacks[SOCKET_ZEP_MAX][SOCKET_ZEP_MAC_STACKSIZE];
static socket_zep_t _socket_zeps[SOCKET_ZEP_MAX];
static gnrc_netdev_t _gnrc_socket_zeps[SOCKET_ZEP_MAX];

void auto_init_socket_zep(void)
{
    DEBUG("auto_init_socket_zep(): initializing device...\n");
    for (int i = 0; i < SOCKET_ZEP_MAX; i++) {
        /* setup netdev device */
        socket_zep_setup(&_socket_zeps[i], &socket_zep_params[i]);

        /* initialize netdev<->gnrc adapter state */
        gnrc_netdev_ieee802154_init(&_gnrc_socket_zeps[i],
                                     (netdev_ieee802154_t *)&_socket_zeps[i]);

        /* start gnrc netdev thread */
        gnrc_netdev_init(_netdev_ieee802154_stacks[i], SOCKET_ZEP_MAC_STACKSIZE,
                          SOCKET_ZEP_MAC_PRIO, "gnrc_socket_zep",
                          &_gnrc_socket_zeps[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SOCKET_ZEP */
/** @} */
