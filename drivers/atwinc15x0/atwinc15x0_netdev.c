/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_atwinc15x0
 * @{
 *
 * @file
 * @brief       Netdev driver for the ATWINC15x0 WiFi module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <string.h>

#define ETH_MODE            (1)

#include "atwinc15x0.h"
#include "atwinc15x0_params.h"

#include "bus_wrapper/include/nm_bus_wrapper.h"
#include "driver/source/m2m_hif.h"
#include "driver/include/m2m_wifi.h"

#include "assert.h"
#include "log.h"
#include "net/netdev/eth.h"
#include "od.h"
#include "xtimer.h"

#define ENABLE_DEBUG        (0)
#define ENABLE_DEBUG_DUMP   (0)
#include "debug.h"

#define ATWINC15X0_EVENT_HANDLER_PERIOD     (100 * US_PER_MS)

#define ATWINC15X0_MAC_STR                  "%02x:%02x:%02x:%02x:%02x:%02x"
#define ATWINC15X0_MAC_STR_ARG(m)           m[0], m[1], m[2], m[3], m[4], m[5]

static uint8_t atwinc15x0_eth_buf[ETHERNET_MAX_LEN];

static void atwinc15x0_wifi_cb(uint8_t event, void *msg);
static void atwinc15x0_eth_cb(uint8_t type, void *msg, void *ctrl);

static tstrWifiInitParam atwinc15x0_wifi_params = {
    .pfAppWifiCb = atwinc15x0_wifi_cb,
    .strEthInitParam = {
        .pfAppWifiCb = atwinc15x0_wifi_cb,
        .pfAppEthCb = atwinc15x0_eth_cb,
        .au8ethRcvBuf = atwinc15x0_eth_buf,
        .u16ethRcvBufSize = ARRAY_SIZE(atwinc15x0_eth_buf),
        .u8EthernetEnable = M2M_WIFI_MODE_ETHERNET,
    },
};

atwinc15x0_t *atwinc15x0 = NULL;

static void atwinc15x0_eth_cb(uint8_t type, void *msg, void *ctrl_buf)
{
    assert(atwinc15x0);
    assert(msg != NULL);
    assert(ctrl_buf != NULL);

    tstrM2mIpCtrlBuf *ctrl = (tstrM2mIpCtrlBuf *)ctrl_buf;

    DEBUG("%s type=%u msg=%p len=%d remaining=%d\n", __func__,
          type, msg, ctrl->u16DataSize, ctrl->u16RemainigDataSize);
#if MODULE_OD && ENABLE_DEBUG_DUMP
    od_hex_dump(msg, ctrl->u16DataSize, 16);
#endif

    uint32_t state = irq_disable();
    uint16_t len = ctrl->u16DataSize;

    /*
     * The ring buffer uses two bytes for the pkt length, followed by the
     * actual packet data.
     */
    if (ringbuffer_get_free(&atwinc15x0->rx_buf) < (len + sizeof(uint16_t))) {
        irq_restore(state);
        DEBUG("%s buffer full, dropping packet of %d bytes", __func__, len);
        return;
    }

    /* store length information as first two bytes and the packet afterwards */
    ringbuffer_add(&atwinc15x0->rx_buf, (char *)&len, sizeof(uint16_t));
    ringbuffer_add(&atwinc15x0->rx_buf, (char *)msg, len);

    irq_restore(state);

    /**
     * This function is executed in thread context. Therefore,
     * netdev.event_callback can be called directly.
     */
    atwinc15x0->netdev.event_callback(&atwinc15x0->netdev,
                                      NETDEV_EVENT_RX_COMPLETE);
}

typedef union {
    tstrM2mWifiStateChanged state_changed;
    tstrM2MConnInfo conn_info;
} atwinc15x0_event_t;

static void atwinc15x0_wifi_cb(uint8_t type, void *msg)
{
    /**
     * This function is executed in thread context. There is no need to call
     * netdev_trigger_event_isr and to handle the events in _atwinc15x0_isr
     */

    DEBUG("%s %u %p\n", __func__, type, msg);

    atwinc15x0_event_t* event = (atwinc15x0_event_t *)msg;

    switch (type) {
        case M2M_WIFI_RESP_SCAN_DONE:
            LOG_INFO("[atwinc15x0] scan done\n");
            break;
        case M2M_WIFI_RESP_CON_STATE_CHANGED:
            switch (event->state_changed.u8CurrState) {
                case M2M_WIFI_DISCONNECTED:
                    LOG_INFO("[atwinc15x0] WiFi disconnected\n");
                    atwinc15x0->connected = false;
                    atwinc15x0->netdev.event_callback(&atwinc15x0->netdev,
                                                      NETDEV_EVENT_LINK_DOWN);
                    break;
                case M2M_WIFI_CONNECTED:
                    LOG_INFO("[atwinc15x0] WiFi connected\n");
                    atwinc15x0->connected = true;
                    atwinc15x0->netdev.event_callback(&atwinc15x0->netdev,
                                                      NETDEV_EVENT_LINK_UP);
                    break;
                default:
                    break;
            }
        default:
            break;
    }
}

static int _atwinc15x0_send(netdev_t *netdev, const iolist_t *iolist)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    assert(dev);
    assert(dev == atwinc15x0);
    assert(iolist);

    if (!dev->connected) {
        DEBUG("%s WiFi is still not connected to AP, cannot send", __func__);
        return -ENODEV;
    }

    uint32_t state = irq_disable();
    dev->tx_len = 0;

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (dev->tx_len + iol->iol_len > ETHERNET_MAX_LEN) {
            irq_restore(state);
            return -EOVERFLOW;
        }
        if (iol->iol_len) {
            memcpy (dev->tx_buf + dev->tx_len, iol->iol_base, iol->iol_len);
            dev->tx_len += iol->iol_len;
        }
    }

#if ENABLE_DEBUG
    DEBUG("%s send %d byte", __func__, dev->tx_len);
#if MODULE_OD && ENABLE_DEBUG_HEXDUMP
    od_hex_dump(dev->tx_buf, dev->tx_len, OD_WIDTH_DEFAULT);
#endif /* MODULE_OD && ENABLE_DEBUG_HEXDUMP */
#endif
    irq_restore(state);

    /* send the the packet to the peer(s) mac address */
    if (m2m_wifi_send_ethernet_pkt(dev->tx_buf, dev->tx_len) == M2M_SUCCESS) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
        return dev->tx_len;
    }
    else {
        DEBUG("%s sending WiFi packet failed", __func__);
        return -EIO;
    }
}

static int _atwinc15x0_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    assert(dev);
    assert(dev == atwinc15x0);

    uint16_t size;
    uint32_t state = irq_disable();

    if (ringbuffer_peek(&dev->rx_buf,
                        (char *)&size, sizeof(uint16_t)) < sizeof(uint16_t)) {
        /* there is nothing in ringbuffer */
        irq_restore(state);
        return 0;
    }

    if (!buf) {
        /* get the size of the frame */
        if (len > 0 && size) {
            /* if len > 0, drop the frame */
            ringbuffer_remove(&dev->rx_buf, sizeof(uint16_t) + size);
        }
        irq_restore(state);
        return size;
    }

    if (len < size) {
        /* buffer is smaller than the number of received bytes */
        DEBUG("%s not enough space in receive buffer", __func__);
        /* newest API requires to drop the frame in that case */
        ringbuffer_remove(&dev->rx_buf, sizeof(uint16_t) + size);
        irq_restore(state);
        return -ENOBUFS;
    }

    /* remove length bytes, copy the buffer to the ringbuffer and free it */
    ringbuffer_remove(&dev->rx_buf, sizeof(uint16_t));
    ringbuffer_get(&dev->rx_buf, buf, size);

#if ENABLE_DEBUG
    ethernet_hdr_t *hdr = (ethernet_hdr_t *)buf;
    DEBUG("%s received %u byte from addr " ATWINC15X0_MAC_STR,
          __func__, size, ATWINC15X0_MAC_STR_ARG(hdr->src));
#if MODULE_OD && ENABLE_DEBUG_HEXDUMP
    od_hex_dump(buf, size, OD_WIDTH_DEFAULT);
#endif /* MODULE_OD && ENABLE_DEBUG_HEXDUMP */
#endif /* ENABLE_DEBUG */

    irq_restore(state);

    return size;
}

static int _atwinc15x0_get(netdev_t *netdev, netopt_t opt, void *val,
                           size_t max_len)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    (void)max_len;
    assert(val);
    assert(dev);
    assert(dev == atwinc15x0);

    DEBUG("%s dev=%p opt=%u val=%p max_len=%u\n", __func__,
          netdev, opt, val, max_len);

    switch (opt) {
        case NETOPT_IS_WIRED:
            return -ENOTSUP;
        case NETOPT_ADDRESS:
            assert(max_len == ETHERNET_ADDR_LEN);
            uint8_t valid;
            m2m_wifi_get_otp_mac_address((uint8_t *)val, &valid);
            return (valid) ? ETHERNET_ADDR_LEN : 0;
        case NETOPT_LINK:
            assert(max_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)val) = (dev->connected) ? NETOPT_ENABLE
                                                         : NETOPT_DISABLE;
            return sizeof(netopt_enable_t);
        default:
            return netdev_eth_get(netdev, opt, val, max_len);
    }
}

static int _atwinc15x0_set(netdev_t *netdev, netopt_t opt, const void *val,
                           size_t max_len)
{
    assert(val);

    DEBUG("%s dev=%p opt=%u val=%p max_len=%u\n", __func__,
          netdev, opt, val, max_len);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len == ETHERNET_ADDR_LEN);
            m2m_wifi_set_mac_address((uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(netdev, opt, val, max_len);
    }
}

int _atwinc15x0_init(netdev_t *netdev)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    (void)netdev;
    assert(dev);
    assert(dev == atwinc15x0);

    DEBUG("%s dev=%p\n", __func__, dev);

    atwinc15x0->bsp_isr = NULL;
    atwinc15x0->bsp_irq_enabled = true;
    atwinc15x0->connected = false;

    /* initialize buffer */
    ringbuffer_init(&dev->rx_buf, (char*)dev->rx_mem, sizeof(dev->rx_mem));

    nm_bsp_init();

    int8_t res;

    /*intilize the WINC Driver*/
    if ((res = m2m_wifi_init(&atwinc15x0_wifi_params)) != M2M_SUCCESS) {
        DEBUG("m2m_wifi_init failed with code %d\n", res);
        if (res == M2M_ERR_FW_VER_MISMATCH) {
            LOG_WARNING("[atwinc15x0] Firmware version mismatch, "
                        "this may lead to problems.\n");
        }
        else {
            LOG_ERROR("[atwinc15x0] Driver initialization error %d\n", res);
            return res;
        }
    }

    /* disable the built-in DHCP client */
    if ((res = m2m_wifi_enable_dhcp(false)) != M2M_SUCCESS) {
        LOG_ERROR("[atwinc15x0] m2m_wifi_enable_dhcp failed with %d\n", res);
        return res;
    }

    /* connect */
    if ((res = m2m_wifi_connect(WIFI_SSID, sizeof(WIFI_SSID),
                                M2M_WIFI_SEC_WPA_PSK, WIFI_PASS,
                                M2M_WIFI_CH_ALL)) != M2M_SUCCESS) {
        LOG_ERROR("[atwinc15x0] WiFi connect failed with %d\n", res);
        return res;
    }

    return 0;
}

static void _atwinc15x0_isr(netdev_t *netdev)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    assert(dev);
    assert(dev == atwinc15x0);

    DEBUG("%s dev=%p\n", __func__, dev);

    /* handle pending ATWINC15x0 module events */
    while (m2m_wifi_handle_events(NULL) != M2M_SUCCESS) { }
}

const netdev_driver_t atwinc15x0_netdev_driver = {
    .send = _atwinc15x0_send,
    .recv = _atwinc15x0_recv,
    .init = _atwinc15x0_init,
    .isr = _atwinc15x0_isr,
    .get = _atwinc15x0_get,
    .set = _atwinc15x0_set,
};

void atwinc15x0_setup(atwinc15x0_t *dev, const atwinc15x0_params_t *params)
{
    assert(dev);

    atwinc15x0 = dev;
    atwinc15x0->netdev.driver = &atwinc15x0_netdev_driver;
    atwinc15x0->params = *params;
}

void atwinc15x0_irq(void)
{
    if (atwinc15x0) {
        netdev_trigger_event_isr(&atwinc15x0->netdev);
    }
}
