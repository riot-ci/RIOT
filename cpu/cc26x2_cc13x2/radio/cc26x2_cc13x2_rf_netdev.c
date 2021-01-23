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

#include "cc26x2_cc13x2_rf.h"
#include "cc26x2_cc13x2_rfc.h"
#include "cc26xx_cc13xx_rfc_mailbox.h"
#include "cc26xx_cc13xx_rfc_prop_mailbox.h"
#include "cc26xx_cc13xx_rfc_queue.h"
#include "vendor/rf_patch_cpe_prop.h"
#include "rf_conf.h"

#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "cpu.h"

static cc26x2_cc13x2_rf_netdev_t *_netdev;  /**< Netdev used for IRQs */
static mutex_t _lock;                       /**< Lock used for IRQs */

void cc26x2_cc13x2_rfc_isr(void)
{
    /* Check if CPE_IRQ_RX_ENTRY_DONE is enabled and the flag is present */
    if ((RFC_DBELL->RFCPEIFG & CPE_IRQ_RX_ENTRY_DONE) &&
        (RFC_DBELL->RFCPEIEN & CPE_IRQ_RX_ENTRY_DONE)) {
        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_RX_ENTRY_DONE;
        _netdev->rx_events++;
        netdev_trigger_event_isr((netdev_t *)_netdev);
    }
}

static int _send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev;
    int ret;

    if (cc26x2_cc13x2_rf_rx_is_on()) {
        cc26x2_cc13x2_rf_rx_stop();
    }

    if ((ret = cc26x2_cc13x2_rf_write(iolist)) != 0) {
        DEBUG("[cc26x2_cc13x2]: couldn't write packet into TX buffer\n");
        return ret;
    }

    if ((ret = cc26x2_cc13x2_request_transmit()) != 0) {
        DEBUG("[cc26x2_cc13x2]: failed to transmit packet\n");
        return ret;
    }

    /* Wait for packet to be sent */
    while (cc26x2_cc13x2_rfc_confirm_execute() == -EAGAIN) {}

    /* Restart RX again */
    assert(!cc26x2_cc13x2_rx_is_on());
    cc26x2_cc13x2_rf_rx_start();

    return len;
}

static int _recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    rfc_data_entry_general_t *entry;
    uint8_t *psdu;
    uint16_t psdu_len;

    (void)dev;
    (void)len;
    (void)buf;
    (void)info;

    /* shouldn't be null as we informed about a packet */
    entry = cc26x2_cc13x2_rf_recv();
    assert(entry != NULL);

    /* first two bytes are the PSDU length */
    psdu = (&entry->data) + sizeof(uint16_t);
    psdu_len = *(uint16_t *)&entry->data;

    if (info) {
        netdev_ieee802154_rx_info_t *ieee_info = info;
        /* RSSI and LQI reside at the end of the PSDU */
        ieee_info->rssi = (int8_t)psdu[psdu_len];
        ieee_info->lqi = 0;
    }

    if (buf == NULL) {
        /* Without buf return only the length so it can be read later */
        if (len == 0) {
            /* Drop packet if we don't have a payload, this might be a frame
             * with Mode Switch (or maybe other frame type?) which doesn't
             * include a payload, RIOT nor the MCU can't process these types of
             * frames. The frame needs to be dropped of the queue because if
             * we return 0 the upper layers won't call the function again (to
             * read the payload this time) and it will not get marked as
             * pending */
            if (psdu_len == 0) {
                entry->status = RFC_DATA_ENTRY_PENDING;
            }
            irq_restore(key);
            return payload_len;
        }
        else {
            /* Discard entry */
            entry->status = RFC_DATA_ENTRY_PENDING;
            irq_restore(key);
            return 0;
        }
    }

    /* check if the buffer has enough space to hold the PSDU */
    if (psdu_len > len) {
        entry->status = RFC_DATA_ENTRY_PENDING;
        irq_restore(key);
        return -ENOBUFS;
    }
    memcpy(buf, psdu, psdu_len);

    entry->status = RFC_DATA_ENTRY_PENDING;

    return psdu_len;
}



static int _init(netdev_t *netdev)
{
    uint64_t hwaddr;
    uint16_t shortaddr;
    cc26x2_cc13x2_rf_netdev_t *dev = (cc26x2_cc13x2_rf_netdev_t *)netdev;

    if (netdev == NULL) {
        return -1;
    }

    /* Initialize netdev_ieee802154 defaults */
    netdev_ieee802154_reset(&dev->netdev);

    hwaddr = cc26x2_cc13x2_get_hwaddr();
    /* https://tools.ietf.org/html/rfc4944#section-12
     * Requires the first bit to 0 for unicast addresses */
    shortaddr = (hwaddr & 0xFFFF);
    shortaddr &= 0x7FFF;

    netdev_ieee802154_set(&dev->netdev, NETOPT_ADDRESS_LONG, hwaddr,
                          sizeof(hwaddr));
    netdev_ieee802154_set(&dev->netdev, NETOPT_ADDRESS, shortaddr,
                         sizeof(shortaddr));

    cc26x2_cc13x2_rf_request_on();
    while (cc26x2_cc13x2_rf_confirm_on() == -EAGAIN) {}

    int ret;
    if ((ret = cc26x2_cc13x2_rf_rx_start()) != 0) {
        return ret;
    }

    return 0;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    (void)opt;
    (void)val;
    (void)len;

    cc26x2_cc13x2_rf_netdev_t *dev = (cc26x2_cc13x2_rf_netdev_t *)netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_RX_END_IRQ:
            assert(len == sizeof(netopt_enable_t));
            if (*(const netopt_enable_t *)val == NETOPT_ENABLE) {
                RFC_DBELL_NONBUF->RFCPEIEN |= CPE_IRQ_RX_ENTRY_DONE;
            }
            else {
                RFC_DBELL_NONBUF->RFCPEIEN &= ~CPE_IRQ_RX_ENTRY_DONE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_TX_END_IRQ:
            assert(len == sizeof(netopt_enable_t));
            if (*(const netopt_enable_t *)val == NETOPT_ENABLE) {
                _tx_end_irq = true;
            }
            else {
                _tx_end_irq = false;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_TX_POWER:
            assert(len == sizeof(int16_t));
            cc26x2_cc13x2_rf_set_tx_pwr(*(const int16_t *)val);
            return sizeof(int16_t);

        default:
            res = -ENOTSUP;
            break;
    }

    if (res == -ENOTSUP) {
        res = netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt, val, len);
    }

    return res;
}


static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    (void)val;
    (void)opt;
    (void)max_len;

    cc26x2_cc13x2_rf_netdev_t *dev = (cc26x2_cc13x2_rf_netdev_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        /* Only MR-FSK is supported */
        case NETOPT_IEEE802154_PHY:
            assert(max_len >= sizeof(uint8_t));
            *(uint8_t *)val = IEEE802154_PHY_MR_FSK;
            return sizeof(uint8_t);

        case NETOPT_RX_END_IRQ:
            assert(max_len >= sizeof(netopt_enable_t));
            if (RFC_DBELL->RFCPEIEN & CPE_IRQ_RX_ENTRY_DONE) {
                *(netopt_enable_t *)val = NETOPT_ENABLE;
            }
            else {
                *(netopt_enable_t *)val = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            *(int16_t *)val = _tx_pwr;
            return sizeof(int16_t);

        default:
            break;
    }

    return netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt, val,
                                 max_len);
}

static void _isr(netdev_t *netdev)
{
    cc26x2_cc13x2_rf_netdev_t *dev = (cc26x2_cc13x2_rf_netdev_t *)netdev;

    while (dev->rx_events) {
        dev->rx_events--;
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
}

void cc26x2_cc13x2_rf_setup(cc26x2_cc13x2_rf_netdev_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;

    netdev->driver = &cc26x2_cc13x2_rf_driver;

    netdev_register(netdev, NETDEV_CC26XX_CC13XX, 0);

    netdev_ieee802154_setup(&dev->netdev);

    _netdev = dev;
    mutex_init(&_lock);
    mutex_lock(&_lock);

    dev->rx_events = 0;
    dev->tx_events = 0;

    cc26x2_cc13x2_rf_init();
}

netdev_driver_t cc26x2_cc13x2_rf_driver =
{
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .set = _set,
    .get = _get,
};
