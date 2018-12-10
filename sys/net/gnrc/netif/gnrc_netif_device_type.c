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
#include "net/ethernet.h"
#include "net/ieee802154.h"
#include "net/l2util.h"

netopt_t gnrc_netif_get_l2addr_opt(const gnrc_netif_t *netif)
{
    netopt_t res = NETOPT_ADDRESS;

    switch (netif->device_type) {
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) || \
    defined(MODULE_NORDIC_SOFTDEVICE_BLE)
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_BLE: {
                netdev_t *dev = netif->dev;
                int r;
                uint16_t tmp;

                r = dev->driver->get(dev, NETOPT_SRC_LEN, &tmp, sizeof(tmp));
                assert(r == sizeof(tmp));
                assert(r <= ((int)UINT8_MAX));
                (void)r;
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

#if defined(MODULE_CC110X) || defined(MODULE_NRFMIN)
static void _create_eui64_from_short(const uint8_t *addr, size_t addr_len,
                                     eui64_t *eui64)
{
    const unsigned offset = sizeof(eui64_t) - addr_len;

    memset(eui64->uint8, 0, sizeof(eui64->uint8));
    eui64->uint8[3] = 0xff;
    eui64->uint8[4] = 0xfe;
    memcpy(&eui64->uint8[offset], addr, addr_len);
}
#endif /* defined(MODULE_CC110X) || defined(MODULE_NRFMIN) */

int gnrc_netif_eui64_from_addr(const gnrc_netif_t *netif,
                               const uint8_t *addr, size_t addr_len,
                               eui64_t *eui64)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    if (netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR) {
        switch (netif->device_type) {
#if defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) || \
    defined(MODULE_NORDIC_SOFTDEVICE_BLE)
            case NETDEV_TYPE_ETHERNET:
            case NETDEV_TYPE_ESP_NOW:
            case NETDEV_TYPE_BLE:
                if (addr_len == sizeof(eui48_t)) {
                    eui48_to_eui64(eui64, (const eui48_t *)addr);
                    return sizeof(eui64_t);
                }
                else {
                    return -EINVAL;
                }
#endif  /* defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) */
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
            case NETDEV_TYPE_IEEE802154:
                switch (addr_len) {
                    case IEEE802154_SHORT_ADDRESS_LEN: {
                        netdev_t *dev = netif->dev;
                        return dev->driver->get(dev, NETOPT_ADDRESS_LONG, eui64,
                                                sizeof(eui64_t));
                    }
                    case IEEE802154_LONG_ADDRESS_LEN:
                        memcpy(eui64, addr, addr_len);
                        return sizeof(eui64_t);
                    default:
                        return -EINVAL;
                }
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
#if defined(MODULE_CC110X) || defined(MODULE_NRFMIN)
            case NETDEV_TYPE_CC110X:
            case NETDEV_TYPE_NRFMIN:
                if (addr_len <= 3) {
                    _create_eui64_from_short(addr, addr_len, eui64);
                    return sizeof(eui64_t);
                }
                else {
                    return -EINVAL;
                }
#endif  /* defined(MODULE_CC110X) || defined(MODULE_NRFMIN) */
            default:
                (void)addr;
                (void)addr_len;
                (void)eui64;
#ifdef DEVELHELP
                LOG_ERROR("gnrc_netif: can't convert hardware address to EUI-64"
                          " on interface %u\n", netif->pid);
#endif  /* DEVELHELP */
                assert(false);
                break;
        }
    }
#endif /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    return -ENOTSUP;
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
    defined(MODULE_XBEE) || defined(MODULE_ESP_NOW) || \
    defined(MODULE_GNRC_SIXLOENC) || defined(MODULE_CC110X)
        case NETDEV_TYPE_IEEE802154:
        case NETDEV_TYPE_NRFMIN:
        case NETDEV_TYPE_CC110X:
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
            netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
#endif
            /* intentionally falls through */
        case NETDEV_TYPE_ESP_NOW:
            res = dev->driver->get(dev, NETOPT_MAX_PDU_SIZE,
                                   &tmp, sizeof(tmp));
            assert(res == sizeof(tmp));
#ifdef MODULE_GNRC_SIXLOWPAN
            netif->ipv6.mtu = IPV6_MIN_MTU;
            netif->sixlo.max_frag_size = tmp;
#else
            netif->ipv6.mtu = tmp;
#endif
            break;
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_NRFMIN) || \
         * defined(MODULE_XBEE) || defined(MODULE_ESP_NOW) */
#ifdef MODULE_NETDEV_ETH
        case NETDEV_TYPE_ETHERNET:
#ifdef MODULE_GNRC_IPV6
            netif->ipv6.mtu = ETHERNET_DATA_LEN;
#endif
#if defined(MODULE_GNRC_SIXLOWPAN_IPHC) && defined(MODULE_GNRC_SIXLOENC)
            netif->flags |= GNRC_NETIF_FLAGS_6LO_HC;
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
            res = dev->driver->get(dev, NETOPT_MAX_PDU_SIZE,
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
