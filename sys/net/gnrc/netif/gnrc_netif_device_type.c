/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Functions that are based around a network interface's device type.
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <errno.h>

#include "log.h"
#ifdef MODULE_GNRC_IPV6
#include "net/ipv6.h"
#endif
#include "net/gnrc/netif.h"
#include "net/eui48.h"
#include "net/ieee802154.h"
#include "net/l2util.h"

netopt_t gnrc_netif_get_l2addr_opt(gnrc_netif_t *netif)
{
    netopt_t res = NETOPT_ADDRESS;

    switch (netif->device_type) {
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) || \
    defined(MODULE_NORDIC_SOFTDEVICE_BLE)
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_BLE: {
                netdev_t *dev = netif->dev;
                uint16_t tmp;

                res = dev->driver->get(dev, NETOPT_SRC_LEN, &tmp, sizeof(tmp));
                assert(res == sizeof(tmp));
                netif->l2addr_len = (uint8_t)tmp;
                if (tmp == IEEE802154_LONG_ADDRESS_LEN) {
                    res = NETOPT_ADDRESS_LONG;
                }
            }
            break;
#endif
        default:
            break;
    }
    return res;
}

#ifdef MODULE_GNRC_IPV6
void gnrc_netif_ipv6_init_mtu(gnrc_netif_t *netif)
{
#ifdef MODULE_GNRC_IPV6
    netdev_t *dev = netif->dev;
    int res;
    uint16_t tmp;

    switch (netif->device_type) {
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_NRFMIN) || \
    defined(MODULE_XBEE)
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_NRFMIN:
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
            netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
#endif
            res = dev->driver->get(dev, NETOPT_MAX_PACKET_SIZE,
                                   &tmp, sizeof(tmp));
            assert(res == sizeof(tmp));
#ifdef MODULE_GNRC_SIXLOWPAN
            netif->ipv6.mtu = IPV6_MIN_MTU;
            netif->sixlo.max_frag_size = tmp;
#else
            netif->ipv6.mtu = tmp;
#endif
            break;
#endif  /* MODULE_NETDEV_IEEE802154 */
#ifdef MODULE_NETDEV_ETH
        case NETDEV_TYPE_ETHERNET:
#ifdef MODULE_GNRC_IPV6
            netif->ipv6.mtu = ETHERNET_DATA_LEN;
#endif
            break;
#endif
#ifdef MODULE_NORDIC_SOFTDEVICE_BLE
        case NETDEV_TYPE_BLE:
            netif->ipv6.mtu = IPV6_MIN_MTU;
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
            netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
#endif
            break;
#endif
        default:
#ifdef DEVELHELP
            LOG_DEBUG("gnrc_netif: getting MTU from device for interface %i\n",
                      netif->pid);
#endif
            res = dev->driver->get(dev, NETOPT_MAX_PACKET_SIZE,
                                   &tmp, sizeof(tmp));
            if (res < 0) {
                /* assume maximum possible transition unit */
                netif->ipv6.mtu = UINT16_MAX;
            }
            else {
                netif->ipv6.mtu = tmp;
            }
            break;
    }
#endif
}

#if defined(MODULE_CC110X) || defined(MODULE_NRFMIN)
static void _create_iid_from_short(const uint8_t *addr, size_t addr_len,
                                   eui64_t *iid)
{
    const unsigned offset = sizeof(eui64_t) - addr_len;

    memset(iid->uint8, 0, sizeof(iid->uint8));
    iid->uint8[3] = 0xff;
    iid->uint8[4] = 0xfe;
    memcpy(&iid->uint8[offset], addr, addr_len);
}
#endif /* defined(MODULE_CC110X) || defined(MODULE_NRFMIN) */

int gnrc_netif_ipv6_iid_from_addr(const gnrc_netif_t *netif,
                                  const uint8_t *addr, size_t addr_len,
                                  eui64_t *iid)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    if (netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR) {
        return l2util_ipv6_iid_from_addr(netif->device_type,
                                         addr, addr_len, iid);
    }
#endif /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    return -ENOTSUP;
}

#endif /* MODULE_GNRC_IPV6 */

/** @} */
