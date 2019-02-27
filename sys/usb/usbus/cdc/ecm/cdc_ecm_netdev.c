/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usbus_cdc_ecm
 * @{
 * @file Netdev implementation for ethernet control model
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <string.h>
#include "kernel_defines.h"
#include "iolist.h"
#include "luid.h"
#include "msg.h"
#include "mutex.h"
#include "net/ethernet.h"
#include "net/netdev.h"
#include "usb/usbus/cdc/ecm.h"
#include "net/netdev/eth.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static const netdev_driver_t netdev_driver_cdcecm;

static void _signal_rx_flush(usbus_cdcecm_device_t *cdcecm)
{
    msg_t msg = { .type = USBUS_MSG_CDCECM_RX_FLUSH,
                 .content = { .ptr = cdcecm } };
    if (msg_send(&msg, cdcecm->usbus->pid) <= 0) {
        puts("CDC ECM netdev: possibly lost message when rx flushing.");
    }
}

static void _signal_tx_xmit(usbus_cdcecm_device_t *cdcecm)
{
    msg_t msg = { .type = USBUS_MSG_CDCECM_TX_XMIT,
                 .content = { .ptr = cdcecm } };
    if (msg_send(&msg, cdcecm->usbus->pid) <= 0) {
        puts("CDC ECM netdev: possibly lost message when tx flushing.");
    }
}

static usbus_cdcecm_device_t *_netdev_to_cdcecm(netdev_t *netdev)
{
    return container_of(netdev, usbus_cdcecm_device_t, netdev);
}

void cdcecm_netdev_setup(usbus_cdcecm_device_t *cdcecm)
{
    cdcecm->netdev.driver = &netdev_driver_cdcecm;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    assert(iolist);
    usbus_cdcecm_device_t *cdcecm = _netdev_to_cdcecm(netdev);
    uint8_t *buf = cdcecm->ep_in.ep->buf;
    const iolist_t *iolist_start = iolist;
    size_t len = iolist_size(iolist);
    DEBUG("CDC_ECM: sending %u bytes\n", len);
    /* load packet data into FIFO */
    size_t iol_offset = 0;
    size_t usb_offset = 0;
    size_t usb_remain = cdcecm->ep_in.ep->len;
    DEBUG("CDC_ECM: cur iol: %d\n", iolist->iol_len);
    while (len) {
        mutex_lock(&cdcecm->out_lock);
        if (iolist->iol_len - iol_offset > usb_remain) {
            /* Only part of the iolist can be copied, usb_remain bytes */
            memcpy(buf + usb_offset, iolist->iol_base + iol_offset, usb_remain);

            usb_offset =  cdcecm->ep_in.ep->len;
            len -= usb_remain;
            iol_offset += usb_remain;
            usb_remain = 0;
        }
        else {
            size_t bytes_copied = iolist->iol_len - iol_offset;
            /* Full iolist can be copied */
            memcpy(buf + usb_offset, iolist->iol_base + iol_offset, bytes_copied);
            len -= bytes_copied;
            usb_offset += bytes_copied;
            usb_remain -= bytes_copied;
            iol_offset = iolist->iol_len;
        }
        if (iol_offset == iolist->iol_len) {
        /* Current iolist exhausted */
            iolist = iolist->iol_next;
            if (iolist) {
                DEBUG("CDC_ECM: cur iol: %d\n", iolist->iol_len);
            }
            else {
                DEBUG("No more iol\n");
            }
            iol_offset = 0;
        }
        if (usb_remain == 0 || !len) {
            cdcecm->tx_len = usb_offset;
            /* USB frame full or last frame, flush! */

            DEBUG("CDC_ECM_NETDEV: triggering xmit with len %d\n", cdcecm->tx_len);
            _signal_tx_xmit(cdcecm);
            usb_remain = cdcecm->ep_in.ep->len;
            usb_offset = 0;
        }
        else {
            mutex_unlock(&cdcecm->out_lock);
        }
    }
    /* Zero length USB packet required */
    if ((iolist_size(iolist_start) % cdcecm->ep_in.ep->len) == 0) {
        mutex_lock(&cdcecm->out_lock);
        DEBUG("CDC ECM netdev: Zero length USB packet required\n");
        cdcecm->tx_len = 0;
        _signal_tx_xmit(cdcecm);
    }
    return len;
}

static int _recv(netdev_t *netdev, void *buf, size_t max_len, void *info)
{
    usbus_cdcecm_device_t *cdcecm = _netdev_to_cdcecm(netdev);
    (void)info;
    if (max_len == 0 && buf == NULL) {
        return cdcecm->len;
    }
    if (max_len && buf == NULL) {
        _signal_rx_flush(cdcecm);
        return cdcecm->len;
    }
    memcpy(buf, cdcecm->in_buf, max_len);
    _signal_rx_flush(cdcecm);
    return max_len;
}

static int _init(netdev_t *netdev)
{
    usbus_cdcecm_device_t *cdcecm = _netdev_to_cdcecm(netdev);
    luid_get(cdcecm->mac_netdev, ETHERNET_ADDR_LEN);
    cdcecm->mac_netdev[0] |= 0x02;      /* locally administered address */
    cdcecm->mac_netdev[0] &= ~0x01;     /* unicast address */
    return 0;
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    usbus_cdcecm_device_t *cdcecm = _netdev_to_cdcecm(netdev);
    (void)max_len;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            memcpy(value, cdcecm->mac_netdev, ETHERNET_ADDR_LEN);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_get(netdev, opt, value, max_len);
    }
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value, size_t value_len)
{
    usbus_cdcecm_device_t *cdcecm = _netdev_to_cdcecm(netdev);
    (void)cdcecm;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(value_len == ETHERNET_ADDR_LEN);
            memcpy(cdcecm->mac_netdev, value, ETHERNET_ADDR_LEN);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(netdev, opt, value, value_len);
    }
}

static void _isr(netdev_t *dev)
{
    usbus_cdcecm_device_t *cdcecm = _netdev_to_cdcecm(dev);
    if (cdcecm->len) {
        cdcecm->netdev.event_callback(&cdcecm->netdev, NETDEV_EVENT_RX_COMPLETE);
    }
}

static const netdev_driver_t netdev_driver_cdcecm = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};
