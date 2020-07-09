/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "board.h"
#include "luid.h"
#include "net/eui_provider.h"

#ifndef EUI48_PROVIDER_NUMOF
#define EUI48_PROVIDER_NUMOF 0U
static const eui48_conf_t eui48_conf[1];
#endif

#ifndef EUI64_PROVIDER_NUMOF
#define EUI64_PROVIDER_NUMOF 0U
static const eui64_conf_t eui64_conf[1];
#endif

void netdev_eui48_get(netdev_t *netdev, eui48_t *addr)
{
    unsigned i = EUI48_PROVIDER_NUMOF;
    while (i--) {
#ifdef GNRC_NETIF_SINGLE
        (void) netdev;
#else
        if (eui48_conf[i].type != netdev->type &&
            eui48_conf[i].type != NETDEV_ANY) {
            continue;
        }

        if (eui48_conf[i].index != netdev->index &&
            eui48_conf[i].index != NETDEV_INDEX_ANY) {
            continue;
        }
#endif
        if (eui48_conf[i].provider(eui48_conf[i].arg, addr) == 0) {
            return;
        }
    }

    luid_get_eui48(addr);
}

void netdev_eui64_get(netdev_t *netdev, eui64_t *addr)
{
    unsigned i = EUI64_PROVIDER_NUMOF;
    while (i--) {
#ifdef GNRC_NETIF_SINGLE
        (void) netdev;
#else
        if (eui64_conf[i].type != netdev->type &&
            eui64_conf[i].type != NETDEV_ANY) {
            continue;
        }

        if (eui64_conf[i].index != netdev->index &&
            eui64_conf[i].index != NETDEV_INDEX_ANY) {
            continue;
        }
#endif
        if (eui64_conf[i].provider(eui64_conf[i].arg, addr) == 0) {
            return;
        }
    }

    luid_get_eui64(addr);
}

/** @} */
