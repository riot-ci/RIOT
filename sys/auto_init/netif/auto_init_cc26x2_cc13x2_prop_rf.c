/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for the cc13x2 Sub-GHz network interface
 *
 * @author  Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifdef MODULE_CC26X2_CC13X2_PROP_RF

#include "log.h"
#include "net/gnrc/netif/ieee802154.h"

#include "cc13x2_prop_rf_netdev.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC26X2_CC13X2_MAC_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#ifndef CC26X2_CC13X2_MAC_PRIO
#define CC26X2_CC13X2_MAC_PRIO            (GNRC_NETIF_PRIO)
#endif

static cc13x2_prop_rf_netdev_t cc13x2_prop_rf_dev;
static char _cc13x2_prop_rf_stack[CC26X2_CC13X2_MAC_STACKSIZE];

void auto_init_cc13x2_prop_rf(void)
{
    LOG_DEBUG("[auto_init_netif] initializing cc2538 radio\n");

    cc13x2_prop_rf_setup(&cc13x2_prop_rf_dev);
    gnrc_netif_ieee802154_create(_cc13x2_prop_rf_stack,
                                 CC26X2_CC13X2_MAC_STACKSIZE,
                                 CC26X2_CC13X2_MAC_PRIO, "cc13x2_prop_rf",
                                 (netdev_t *)&cc13x2_prop_rf_dev);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC26X2_CC13X2_PROP_RF */
/** @} */
