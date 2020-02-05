/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC13x2 IEEE 802.15.4 netdev driver
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include <string.h>

#include "cc13x2_prop_rf_netdev.h"
#include "cc13x2_prop_rf_internal.h"
#include "cc26x2_cc13x2_rfc.h"
#include <driverlib/rf_mailbox.h>

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "cpu.h"

static void _irq_handler(void *arg)
{
    netdev_t *dev = (netdev_t *)arg;

    if (dev->event_callback) {
        dev->event_callback(dev, NETDEV_EVENT_ISR);
    }
}

static int _send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev;

    return cc13x2_prop_rf_send(iolist);
}

static int _recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    (void)dev;

    return cc13x2_prop_rf_recv(buf, len, info);
}

static int _init(netdev_t *dev)
{
    cc26x2_cc13x2_enable_vims();

    cc13x2_prop_rf_netdev_t *netdev = (cc13x2_prop_rf_netdev_t *)dev;

    if (dev == NULL) {
        return -1;
    }

    cc13x2_prop_rf_irq_set_handler(_irq_handler, dev);
    cc13x2_prop_rf_get_ieee_eui64(netdev->netdev.long_addr);
    memcpy(netdev->netdev.short_addr, netdev->netdev.long_addr + 6, 2);
    /* https://tools.ietf.org/html/rfc4944#section-12
     * Requires the first bit to 0 for unicast addresses */
    netdev->netdev.short_addr[1] &= 0x7F;

    netdev_ieee802154_reset(&netdev->netdev);

    if (cc13x2_prop_rf_enable() == -1) {
        return -1;
    }

    if (cc13x2_prop_rf_rx_start() == -1) {
        return -1;
    }

    cc13x2_prop_rf_set_chan(CC13X2_CHANNEL_MIN_SUB_GHZ, true);

    return 0;
}

static int _set_state(netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_OFF:
            if (_cc13x2_prop_rf_state != cc13x2_stateDisabled) {
                if (cc13x2_prop_rf_enable() == -1) {
                    DEBUG("[cc13x2_prop_rf]: couldn't enable RF Core.\n");
                    return -EIO;
                }
                if (cc13x2_prop_rf_rx_start() == -1) {
                    DEBUG("[cc13x2_prop_rf]: couldn't start RX.\n");
                    return -EIO;
                }
            }
            break;

        case NETOPT_STATE_IDLE:
            /* If the state is on Transmit or Sleep, it will be switched to
             * Receive state */
            if (_cc13x2_prop_rf_state != cc13x2_stateDisabled) {
                if (cc13x2_prop_rf_rx_start() == -1) {
                    DEBUG("[cc13x2_prop_rf]: couldn't start RX.\n");
                    return -EIO;
                }
            }
            break;

        case NETOPT_STATE_RESET:
            if (cc13x2_prop_rf_reset() == -1) {
                DEBUG("[cc13x2_prop_rf]: couldn't reset state.\n");
                return -EIO;
            }
            break;

        default:
            return -ENOTSUP;
    }

    return sizeof(netopt_state_t);
}

static netopt_state_t _get_state(void)
{
    switch (_cc13x2_prop_rf_state) {
        case cc13x2_stateDisabled:
            return NETOPT_STATE_OFF;
        case cc13x2_stateSleep:
            return NETOPT_STATE_IDLE;
        case cc13x2_stateReceive:
            return NETOPT_STATE_RX;
        case cc13x2_stateTransmit:
            return NETOPT_STATE_TX;
        default:
            return NETOPT_STATE_IDLE;
    }
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    cc13x2_prop_rf_netdev_t *dev = (cc13x2_prop_rf_netdev_t *) netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_CHANNEL:
            assert(len == sizeof(uint16_t));
            uint8_t chan = (((const uint16_t *)val)[0]) & UINT8_MAX;
            cc13x2_prop_rf_set_chan(chan, false);
            /* don't set res to set netdev_ieee802154_t::chan */
            break;

        case NETOPT_CHANNEL_PAGE:
            assert(len == sizeof(uint16_t));
            uint8_t page = (((const uint16_t *)val)[0]) & UINT8_MAX;
            if (page != 0) {
                res = -EINVAL;
            }
            else {
                res = sizeof(uint16_t);
            }
            break;

        case NETOPT_TX_POWER:
            assert(len <= sizeof(int16_t));
            cc13x2_prop_rf_set_txpower(*((const int16_t *)val));
            res = sizeof(uint16_t);
            break;

        case NETOPT_RX_END_IRQ:
            if (((const bool *)val)[0]) {
                cc13x2_prop_rf_irq_enable(IRQ_RX_OK);
            }
            else {
                cc13x2_prop_rf_irq_disable(IRQ_RX_OK);
            }
            return sizeof(netopt_enable_t);

        case NETOPT_TX_END_IRQ:
            if (((const bool *)val)[0]) {
                cc13x2_prop_rf_irq_enable(IRQ_TX_DONE);
            }
            else {
                cc13x2_prop_rf_irq_disable(IRQ_TX_DONE);
            }
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_STATE:
            res = _set_state(*((netopt_state_t *)val));
            break;

        default:
            break;
    }

    if (res == -ENOTSUP) {
        res = netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt, val, len);
    }

    return res;
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    cc13x2_prop_rf_netdev_t *dev = (cc13x2_prop_rf_netdev_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            *((netopt_state_t *)val) = _get_state();
            return sizeof(netopt_state_t);

        case NETOPT_RX_END_IRQ:
            *((netopt_enable_t *)val) = cc13x2_prop_rf_irq_is_enabled(IRQ_RX_OK);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_END_IRQ:
            *((netopt_enable_t *)val) = cc13x2_prop_rf_irq_is_enabled(IRQ_TX_DONE);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            *((uint16_t *)val) = cc13x2_prop_rf_get_txpower();
            return sizeof(uint16_t);

        default:
            break;
    }

    return netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt, val, max_len);
}

static void _isr(netdev_t *netdev)
{
    unsigned state = irq_disable();
    unsigned flags = cc13x2_prop_rf_get_flags();
    irq_restore(state);

    if (flags & 1) {
        while(cc13x2_prop_rf_recv_avail()) {
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
    }

    if (flags & 2) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
}

void cc13x2_prop_rf_setup(cc13x2_prop_rf_netdev_t *dev)
{
    memset(dev, 0, sizeof(*dev));
    cc13x2_prop_rf_init();

    dev->netdev.netdev.driver = &cc13x2_prop_rf_driver;
}

netdev_driver_t cc13x2_prop_rf_driver =
{
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr  = _isr,
    .set  = _set,
    .get  = _get,
};
