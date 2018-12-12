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
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>

#include "log.h"
#include "net/eui48.h"
#include "net/ieee802154.h"
#include "net/ipv6.h"
#include "net/netdev.h"

#include "net/l2util.h"

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

int l2util_ipv6_iid_from_addr(int dev_type,
                              const uint8_t *addr, size_t addr_len,
                              eui64_t *iid)
{
    switch (dev_type) {
#if defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW)
        case NETDEV_TYPE_ETHERNET:
        case NETDEV_TYPE_ESP_NOW:
            if (addr_len == sizeof(eui48_t)) {
                eui48_to_ipv6_iid(iid, (const eui48_t *)addr);
                return sizeof(eui64_t);
            }
            else {
                return -EINVAL;
            }
#endif  /* defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) */
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
        case NETDEV_TYPE_IEEE802154:
            if (ieee802154_get_iid(iid, addr, addr_len) != NULL) {
                return sizeof(eui64_t);
            }
            else {
                return -EINVAL;
            }
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
#ifdef MODULE_NORDIC_SOFTDEVICE_BLE
        case NETDEV_TYPE_BLE:
            if (addr_len == sizeof(eui64_t)) {
                memcpy(iid, addr, sizeof(eui64_t));
                iid->uint8[0] ^= 0x02;
                return sizeof(eui64_t);
            }
            else {
                return -EINVAL;
            }
#endif  /* MODULE_NORDIC_SOFTDEVICE_BLE */
#if defined(MODULE_CC110X) || defined(MODULE_NRFMIN)
        case NETDEV_TYPE_CC110X:
        case NETDEV_TYPE_NRFMIN:
            if (addr_len <= 3) {
                _create_iid_from_short(addr, addr_len, iid);
                return sizeof(eui64_t);
            }
            else {
                return -EINVAL;
            }
#endif  /* defined(MODULE_CC110X) || defined(MODULE_NRFMIN) */
        default:
            (void)addr;
            (void)addr_len;
            (void)iid;
#ifdef DEVELHELP
            LOG_ERROR("gnrc_netif: can't convert hardware address to IID "
                      "for device type %d\n", dev_type);
#endif  /* DEVELHELP */
            assert(false);
            break;
    }
    return -ENOTSUP;
}

int l2util_ipv6_iid_to_addr(int dev_type, const eui64_t *iid, uint8_t *addr)
{
    switch (dev_type) {
#if defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW)
        case NETDEV_TYPE_ETHERNET:
        case NETDEV_TYPE_ESP_NOW:
            eui48_from_ipv6_iid((eui48_t *)addr, iid);
            return sizeof(eui48_t);
#endif  /* defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) */
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
        case NETDEV_TYPE_IEEE802154:
            /* assume address was based on EUI-64
             * (see https://tools.ietf.org/html/rfc6775#section-5.2) */
            memcpy(addr, iid, sizeof(eui64_t));
            addr[0] ^= 0x02;
            return sizeof(eui64_t);
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
#ifdef MODULE_NRFMIN
        case NETDEV_TYPE_NRFMIN:
            addr[0] = iid->uint8[6];
            addr[1] = iid->uint8[7];
            return sizeof(uint16_t);
#endif  /* MODULE_NETDEV_IEEE802154 */
#ifdef MODULE_NORDIC_SOFTDEVICE_BLE
        case NETDEV_TYPE_BLE:
            memcpy(addr, iid, sizeof(eui64_t));
            addr[0] ^= 0x02;
            return sizeof(eui64_t);
#endif  /* MODULE_NORDIC_SOFTDEVICE_BLE */
#ifdef MODULE_CC110X
        case NETDEV_TYPE_CC110X:
            addr[0] = iid->uint8[7];
            return sizeof(uint8_t);
#endif  /* MODULE_CC110X */
        default:
            (void)iid;
            (void)addr;
#ifdef DEVELHELP
            LOG_ERROR("gnrc_netif: can't convert IID to hardware address "
                      "for device type %d\n", dev_type);
#endif  /* DEVELHELP */
            assert(false);
            break;
    }
    return -ENOTSUP;
}

int l2util_ndp_addr_len_from_l2ao(int dev_type,
                                  const ndp_opt_t *opt)
{
    switch (dev_type) {
#ifdef MODULE_CC110X
        case NETDEV_TYPE_CC110X:
            (void)opt;
            return sizeof(uint8_t);
#endif  /* MODULE_CC110X */
#if defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW)
        case NETDEV_TYPE_ETHERNET:
        case NETDEV_TYPE_ESP_NOW:
            /* see https://tools.ietf.org/html/rfc2464#section-6*/
            if (opt->len == 1U) {
                return sizeof(eui48_t);
            }
            else {
                return -EINVAL;
            }
#endif  /* defined(MODULE_NETDEV_ETH) || defined(MODULE_ESP_NOW) */
#ifdef MODULE_NRFMIN
        case NETDEV_TYPE_NRFMIN:
            (void)opt;
            return sizeof(uint16_t);
#endif  /* MODULE_NRFMIN */
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
        case NETDEV_TYPE_IEEE802154:
            /* see https://tools.ietf.org/html/rfc4944#section-8 */
            switch (opt->len) {
                case 1U:
                    return IEEE802154_SHORT_ADDRESS_LEN;
                case 2U:
                    return IEEE802154_LONG_ADDRESS_LEN;
                default:
                    return -EINVAL;
            }
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
        default:
            (void)opt;
#ifdef DEVELHELP
            LOG_ERROR("gnrc_netif: can't get address length from NDP link-layer "
                      "address option for device type %d\n", dev_type);
#endif
            assert(false);
            break;
    }
    return -ENOTSUP;
}


/** @} */
