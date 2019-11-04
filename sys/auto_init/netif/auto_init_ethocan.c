/*
 * Copyright (C) 2019 Juergen Fitschen <me@jue.yt>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for Ethernet-over-CAN module
 *
 * @author  Juergen Fitschen <me@jue.yt>
 */

#ifdef MODULE_ETHOCAN

#include "log.h"
#include "debug.h"
#include "ethocan.h"
#include "ethocan_params.h"
#include "net/gnrc/netif/ethernet.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ETHOCAN_MAC_STACKSIZE (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef ETHOCAN_MAC_PRIO
#define ETHOCAN_MAC_PRIO      (GNRC_NETIF_PRIO)
#endif

#define ETHOCAN_NUM (sizeof(ethocan_params) / sizeof(ethocan_params[0]))

static char _netdev_eth_stack[ETHOCAN_NUM][ETHOCAN_MAC_STACKSIZE];
static ethocan_t ethocan[ETHOCAN_NUM];

void auto_init_ethocan(void)
{
    /* setup netdev devices */
    for (unsigned i = 0; i < ETHOCAN_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing ethocan #%d.\n", i);

        ethocan_setup(&ethocan[i], &ethocan_params[i]);
        gnrc_netif_ethernet_create(_netdev_eth_stack[i], ETHOCAN_MAC_STACKSIZE,
                                   ETHOCAN_MAC_PRIO, "ethocan", (netdev_t *)&ethocan[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ETHOCAN */
/** @} */
