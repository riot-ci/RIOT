/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266_esp_now
 * @{
 *
 * @file
 * @brief       Netdev interface for the ESP-NOW WiFi P2P protocol
 *
 * With ESP-NOW the ESP8266 SDK provides a connectionless communication
 * technology, featuring short packet transmission. It applies the IEEE802.11
 * Action Vendor frame technology, along with the IE function developed by
 * Espressif, and CCMP encryption technology, realizing a secure, connectionless
 * communication solution.
 *
 * This netdev driver uses ESP-NOW to realize an link layer interface to a
 * meshed network of ESP8266 nodes. In this network, each node can send short
 * packets to all other nodes that are visible in its area.
 *
 * The ESP8266 nodes are used in the ESP-NOW COMBO role along with the
 * integrated WiFi interface's SoftAP + station mode to advertise their SSID
 * and become visible to other ESP8266 nodes.
 *
 * The SSID of an ESP8266 node is the concatenation of the prefix "RIOT_ESP_"
 * with the MAC address of its SoftAP WiFi interface. The driver periodically
 * scans all visible ESP8266 nodes. The period can be configured by driver
 * parameters.
 *
 * Using the driver parameters, the encrypted communication can be enabled or
 * disabled. All nodes in a network must use either encrypted or unencrypted
 * communication.
 *
 * Please note: If encrypted communication is used, a maximum of 6 nodes can
 * communicate with each other, while in unencrypted mode, up to 20 nodes can
 * communicate.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Timo Rothenpieler <timo.rothenpieler@uni-bremen.de>
 */

#include "log.h"
#include "tools.h"

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "net/gnrc.h"
#include "xtimer.h"

#include "common.h"
#include "espressif/c_types.h"
#include "espnow.h"
#include "esp_now_params.h"
#include "esp_now_netdev.h"
#include "esp_now_netif.h"
#include "irq_arch.h"
#include "od.h"
#include "esp/common_macros.h"
#include "sdk/sdk.h"

#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6/nib.h"

#define ENABLE_DEBUG             (0)
#include "debug.h"

#define ESP_NOW_UNICAST          (1)

#define ESP_NOW_WIFI_STA         (1)
#define ESP_NOW_WIFI_SOFTAP      (2)
#define ESP_NOW_WIFI_STA_SOFTAP  (ESP_NOW_WIFI_STA + ESP_NOW_WIFI_SOFTAP)

#define ESP_NOW_AP_PREFIX        "RIOT_ESP_"
#define ESP_NOW_AP_PREFIX_LEN    (strlen(ESP_NOW_AP_PREFIX))

#define ESP_NOW_ROLE_IDLE        (0)
#define ESP_NOW_ROLE_CONTROLLER  (1)
#define ESP_NOW_ROLE_SLAVE       (2)
#define ESP_NOW_ROLE_COMBO       (3)
#define ESP_NOW_KEY_LEN          (16)

#define ESP_OK                   (0)

#ifndef IRAM_ATTR
#define IRAM_ATTR                IRAM
#endif

/**
 * There is only one ESP-NOW device. We define it as static device variable
 * to have accesss to the device inside ESP-NOW interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP-NOW
 * device which triggers the interrupt.
 */
static esp_now_netdev_t _esp_now_dev;
static const netdev_driver_t _esp_now_driver;

static bool _esp_now_add_peer(uint8_t* bssid, uint8_t channel, uint8_t* key)
{
    if (esp_now_is_peer_exist(bssid)) {
        return false;
    }

    int ret = esp_now_add_peer(bssid, ESP_NOW_ROLE_COMBO, channel,
                               esp_now_params.key,
                               esp_now_params.key ? ESP_NOW_KEY_LEN : 0);
    DEBUG("esp_now_add_peer node %02x:%02x:%02x:%02x:%02x:%02x "
          "added with return value %d\n",
          bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], ret);
    return (ret == ESP_OK);
}

#if ESP_NOW_UNICAST

static xtimer_t _esp_now_scan_peers_timer;
static bool _esp_now_scan_peers_done = false;

static const struct scan_config scan_conf = {
        .ssid = 0,
        .bssid = 0,
        .channel = ESP_NOW_CHANNEL,
        .show_hidden = true
};

static void IRAM esp_now_scan_peers_done(void *arg, STATUS status)
{
    /* This function is executed in interrupt context */

    DEBUG("%s: %p %d\n", __func__, arg, status);

    CHECK_PARAM (status == OK);
    CHECK_PARAM (arg != NULL);

    critical_enter();

    struct bss_info *bss_link = (struct bss_info*)arg;

    /* iterate over APs records */
    while (bss_link) {
        /* check whether the AP is an ESP_NOW node */
        if (strncmp((char*)bss_link->ssid, ESP_NOW_AP_PREFIX, ESP_NOW_AP_PREFIX_LEN) == 0) {
            /* add the AP as peer */
            _esp_now_add_peer(bss_link->bssid, bss_link->channel, esp_now_params.key);
        }
        bss_link = STAILQ_NEXT(bss_link, next);
    }

#if ENABLE_DEBUG
    uint8_t peers_all;
    uint8_t peers_enc;
    esp_now_get_cnt_info (&peers_all, &peers_enc);
    DEBUG("associated peers total=%d, encrypted=%d\n", peers_all, peers_enc);
#endif

    _esp_now_scan_peers_done = true;

    critical_exit();
}

static void esp_now_scan_peers_start(void)
{
    DEBUG("%s\n", __func__);

    /* set the time for next scan */
    xtimer_set (&_esp_now_scan_peers_timer, esp_now_params.scan_period);
    /* start the scan */
    wifi_station_scan((struct scan_config*)&scan_conf, esp_now_scan_peers_done);
}

static void IRAM_ATTR esp_now_scan_peers_timer_cb(void* arg)
{
    DEBUG("%s\n", __func__);

    esp_now_netdev_t* dev = (esp_now_netdev_t*)arg;

    if (dev->netdev.event_callback) {
        dev->scan_event = true;
        dev->netdev.event_callback((netdev_t*)dev, NETDEV_EVENT_ISR);
    }
}

#else

static const uint8_t _esp_now_mac[6] = { 0x82, 0x73, 0x79, 0x84, 0x79, 0x83 }; /* RIOTOS */

#endif /* ESP_NOW_UNICAST */

static IRAM_ATTR void esp_now_recv_cb(uint8_t *mac, uint8_t *data, uint8_t len)
{
    /* This function is executed in thread context */

#if ESP_NOW_UNICAST
    if (!_esp_now_scan_peers_done) {
        /* if peers are not scanned, we cannot receive anything */
        return;
    }
#endif

    mutex_lock(&_esp_now_dev.rx_lock);
    critical_enter();

    /*
     * The ring buffer uses a single byte for the pkt length, followed by the mac address,
     * followed by the actual packet data. The MTU for ESP-NOW is 250 bytes, so len will never
     * exceed the limits of a byte as the mac address length is not included.
     */
    if ((int)ringbuffer_get_free(&_esp_now_dev.rx_buf) < 1 + ESP_NOW_ADDR_LEN + len) {
        critical_exit();
        mutex_unlock(&_esp_now_dev.rx_lock);
        DEBUG("%s: buffer full, dropping incoming packet of %d bytes\n", __func__, len);
        return;
    }

#if 0 /* don't printf anything in ISR */
    printf("%s\n", __func__);
    printf("%s: received %d byte from %02x:%02x:%02x:%02x:%02x:%02x\n",
           __func__, len,
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    od_hex_dump(data, len, OD_WIDTH_DEFAULT);
#endif

    ringbuffer_add_one(&_esp_now_dev.rx_buf, len);
    ringbuffer_add(&_esp_now_dev.rx_buf, (char*)mac, ESP_NOW_ADDR_LEN);
    ringbuffer_add(&_esp_now_dev.rx_buf, (char*)data, len);

    if (_esp_now_dev.netdev.event_callback) {
        _esp_now_dev.recv_event = true;
        _esp_now_dev.netdev.event_callback((netdev_t*)&_esp_now_dev, NETDEV_EVENT_ISR);
    }

    critical_exit();
    mutex_unlock(&_esp_now_dev.rx_lock);
}

static int _esp_now_sending = 0;

static void IRAM esp_now_send_cb(uint8_t *mac, uint8_t status)
{
    DEBUG("%s: sent to %02x:%02x:%02x:%02x:%02x:%02x with status %d\n",
          __func__,
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], status);

    if (_esp_now_sending) {
        _esp_now_sending--;
    }
}

esp_now_netdev_t *netdev_esp_now_setup(void)
{
    esp_now_netdev_t* dev = &_esp_now_dev;

    DEBUG("%s: %p\n", __func__, dev);

    if (dev->netdev.driver) {
        DEBUG("%s: early returning previously initialized device\n", __func__);
        return dev;
    }

    ringbuffer_init(&dev->rx_buf, (char*)dev->rx_mem, sizeof(dev->rx_mem));

    /* set the WiFi interface to Station + SoftAP mode without DHCP */
    wifi_set_opmode_current(ESP_NOW_WIFI_STA_SOFTAP);
    wifi_softap_dhcps_stop();

    /* get SoftAP mac address and store it in device address */
    wifi_get_macaddr(SOFTAP_IF, dev->addr);

    /* set the SoftAP configuration */
    struct softap_config ap_conf;

    strcpy((char*)ap_conf.password, esp_now_params.softap_pass);
    sprintf((char*)ap_conf.ssid, "%s%02x%02x%02x%02x%02x%02x", ESP_NOW_AP_PREFIX,
            dev->addr[0], dev->addr[1], dev->addr[2],
            dev->addr[3], dev->addr[4], dev->addr[5]);

    ap_conf.ssid_len = strlen((char*)ap_conf.ssid);
    ap_conf.channel = esp_now_params.channel; /* support 1 ~ 13 */
    ap_conf.authmode = AUTH_WPA2_PSK;        /* don't support AUTH_WEP in softAP mode. */
    ap_conf.ssid_hidden = 0;                 /* default 0 */
    ap_conf.max_connection = 4;              /* default 4, max 4 */
    ap_conf.beacon_interval = 100;           /* support 100 ~ 60000 ms, default 100 */

    wifi_softap_set_config_current(&ap_conf);

#if !ESP_NOW_UNICAST
    /* all ESP-NOW nodes get the shared mac address on their station interface */
    wifi_set_macaddr(STATION_IF, (uint8_t*)_esp_now_mac);
#endif

    /* set the netdev driver */
    dev->netdev.driver = &_esp_now_driver;

    /* initialize netdev data structure */
    dev->recv_event = false;
    dev->scan_event = false;

    mutex_init(&dev->dev_lock);
    mutex_init(&dev->rx_lock);

    /* initialize ESP-NOW */
    esp_now_init();
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_register_send_cb (esp_now_send_cb);
    esp_now_register_recv_cb (esp_now_recv_cb);

#if ESP_NOW_UNICAST
    /* timer for peer scan initialization */
    _esp_now_scan_peers_done = false;
    _esp_now_scan_peers_timer.callback = &esp_now_scan_peers_timer_cb;
    _esp_now_scan_peers_timer.arg = dev;

    /* execute the first scan */
    esp_now_scan_peers_start();
#else /* ESP_NOW_UNICAST */
    bool res = _esp_now_add_peer((uint8_t*)_esp_now_mac, esp_now_params.channel,
                                                         esp_now_params.key);
    DEBUG("%s: multicast node add %s\n", __func__, res ? "success" : "error");
#endif /* ESP_NOW_UNICAST */

    return dev;
}

static int _init(netdev_t *netdev)
{
    DEBUG("%s: %p\n", __func__, netdev);

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0x00, sizeof(netstats_t));
#endif

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    DEBUG("%s: %p %p\n", __func__, netdev, iolist);

#if ESP_NOW_UNICAST
    if (!_esp_now_scan_peers_done) {
        return -ENODEV;
    }
#endif

    CHECK_PARAM_RET(netdev != NULL, -ENODEV);
    CHECK_PARAM_RET(iolist != NULL && iolist->iol_len == ESP_NOW_ADDR_LEN, -EINVAL);
    CHECK_PARAM_RET(iolist->iol_next != NULL, -EINVAL);

    esp_now_netdev_t* dev = (esp_now_netdev_t*)netdev;

    mutex_lock(&dev->dev_lock);

#if ESP_NOW_UNICAST
    uint8_t* _esp_now_dst = NULL;

    for (uint8_t i = 0; i < ESP_NOW_ADDR_LEN; i++) {
        if (((uint8_t*)iolist->iol_base)[i] != 0xff) {
            _esp_now_dst = iolist->iol_base;
            break;
        }
    }
#else
    const uint8_t* _esp_now_dst = _esp_now_mac;
#endif
    iolist = iolist->iol_next;

    DEBUG("%s: send %u byte\n", __func__, (unsigned)iolist->iol_len);
#if MODULE_OD && ENABLE_DEBUG
    od_hex_dump(iolist->iol_base, iolist->iol_len, OD_WIDTH_DEFAULT);
#endif

    if (_esp_now_dst) {
        DEBUG("%s: send to esp_now addr %02x:%02x:%02x:%02x:%02x:%02x\n", __func__,
              _esp_now_dst[0], _esp_now_dst[1], _esp_now_dst[2],
              _esp_now_dst[3], _esp_now_dst[4], _esp_now_dst[5]);
    } else {
        DEBUG("%s: send esp_now broadcast\n", __func__);
    }

    /* send the packet to the peer(s) mac address */
    if (esp_now_send(_esp_now_dst, iolist->iol_base, iolist->iol_len) == 0) {
        _esp_now_sending = 1;
        while (_esp_now_sending > 0) {
            thread_yield_higher();
        }

#ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_bytes += iolist->iol_len;
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
#endif

        mutex_unlock(&dev->dev_lock);
        return iolist->iol_len;
    }
    else {
#ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_failed++;
#endif
    }

    mutex_unlock(&dev->dev_lock);
    return -EIO;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    DEBUG("%s: %p %p %u %p\n", __func__, netdev, buf, len, info);

    CHECK_PARAM_RET(netdev != NULL, -ENODEV);

    esp_now_netdev_t* dev = (esp_now_netdev_t*)netdev;

    mutex_lock(&dev->rx_lock);

    uint16_t size = ringbuffer_empty(&dev->rx_buf)
        ? 0
        : (ringbuffer_peek_one(&dev->rx_buf) + ESP_NOW_ADDR_LEN);

    if (size && dev->rx_buf.avail < size) {
        /* this should never happen unless this very function messes up */
        mutex_unlock(&dev->rx_lock);
        return -EIO;
    }

    if (!buf && !len) {
        /* return the size without dropping received data */
        mutex_unlock(&dev->rx_lock);
        return size;
    }

    if (!buf && len) {
        /* return the size and drop received data */
        if (size) {
            ringbuffer_remove(&dev->rx_buf, 1 + size);
        }
        mutex_unlock(&dev->rx_lock);
        return size;
    }

    if (buf && len && !size) {
        mutex_unlock(&dev->rx_lock);
        return 0;
    }

    if (buf && len && size) {
        if (size > len) {
            DEBUG("[esp_now] No space in receive buffers\n");
            mutex_unlock(&dev->rx_lock);
            return -ENOBUFS;
        }

        /* remove already peeked size byte */
        ringbuffer_remove(&dev->rx_buf, 1);
        ringbuffer_get(&dev->rx_buf, buf, size);
        uint8_t *mac = buf;

        DEBUG("%s: received %d byte from %02x:%02x:%02x:%02x:%02x:%02x\n",
              __func__, size - ESP_NOW_ADDR_LEN,
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#if MODULE_OD && ENABLE_DEBUG
        od_hex_dump(buf + ESP_NOW_ADDR_LEN, size - ESP_NOW_ADDR_LEN, OD_WIDTH_DEFAULT);
#endif

#if ESP_NOW_UNICAST
        if (esp_now_is_peer_exist(mac) <= 0) {
            _esp_now_add_peer(mac, esp_now_params.channel, esp_now_params.key);
        }
#endif

#ifdef MODULE_NETSTATS_L2
        netdev->stats.rx_count++;
        netdev->stats.rx_bytes += size;
#endif

        mutex_unlock(&dev->rx_lock);
        return size;
    }

    mutex_unlock(&dev->rx_lock);
    return -EINVAL;
}
static inline int _get_iid(esp_now_netdev_t *dev, eui64_t *value, size_t max_len)
{
    CHECK_PARAM_RET (max_len >= sizeof(eui64_t), -EOVERFLOW);

    /* interface id according to */
    /* https://tools.ietf.org/html/rfc4291#section-2.5.1 */
    value->uint8[0] = dev->addr[0] ^ 0x02; /* invert bit1 */
    value->uint8[1] = dev->addr[1];
    value->uint8[2] = dev->addr[2];
    value->uint8[3] = 0xff;
    value->uint8[4] = 0xfe;
    value->uint8[5] = dev->addr[3];
    value->uint8[6] = dev->addr[4];
    value->uint8[7] = dev->addr[5];

    return sizeof(eui64_t);
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    DEBUG("%s: %s %p %p %u\n", __func__, netopt2str(opt), netdev, val, max_len);

    CHECK_PARAM_RET(netdev != NULL, -ENODEV);
    CHECK_PARAM_RET(val != NULL, -EINVAL);

    esp_now_netdev_t *dev = (esp_now_netdev_t*)netdev;
    int res = -ENOTSUP;

    switch (opt) {

        case NETOPT_DEVICE_TYPE:
            CHECK_PARAM_RET(max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = NETDEV_TYPE_ESP_NOW;
            res = sizeof(uint16_t);
            break;

#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            CHECK_PARAM_RET(max_len == sizeof(gnrc_nettype_t), -EOVERFLOW);
            *((gnrc_nettype_t *)val) = dev->proto;
            res = sizeof(gnrc_nettype_t);
            break;
#endif

        case NETOPT_MAX_PACKET_SIZE:
            CHECK_PARAM_RET(max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = ESP_NOW_MAX_SIZE;
            res = sizeof(uint16_t);
            break;

        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            CHECK_PARAM_RET(max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = sizeof(dev->addr);
            res = sizeof(uint16_t);
            break;

        case NETOPT_ADDRESS:
            CHECK_PARAM_RET(max_len >= sizeof(dev->addr), -EOVERFLOW);
            memcpy(val, dev->addr, sizeof(dev->addr));
            res = sizeof(dev->addr);
            break;

        case NETOPT_IPV6_IID:
            res = _get_iid(dev, val, max_len);
            break;

#ifdef MODULE_NETSTATS_L2
        case NETOPT_STATS:
            CHECK_PARAM_RET(max_len == sizeof(uintptr_t), -EOVERFLOW);
            *((netstats_t **)val) = &netdev->stats;
            res = sizeof(uintptr_t);
            break;
#endif

        default:
            DEBUG("%s: %s not supported\n", __func__, netopt2str(opt));
            break;
    }
    return res;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t max_len)
{
    DEBUG("%s: %s %p %p %u\n", __func__, netopt2str(opt), netdev, val, max_len);

    CHECK_PARAM_RET(netdev != NULL, -ENODEV);
    CHECK_PARAM_RET(val != NULL, -EINVAL);

    esp_now_netdev_t *dev = (esp_now_netdev_t *) netdev;
    int res = -ENOTSUP;

    switch (opt) {

#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            CHECK_PARAM_RET(max_len == sizeof(gnrc_nettype_t), -EOVERFLOW);
            dev->proto = *((gnrc_nettype_t *)val);
            res = sizeof(gnrc_nettype_t);
            break;
#endif

        case NETOPT_ADDRESS:
            CHECK_PARAM_RET(max_len >= sizeof(dev->addr), -EOVERFLOW);
            memcpy(dev->addr, val, sizeof(dev->addr));
            res = sizeof(dev->addr);
            break;

        default:
            DEBUG("%s: %s not supported\n", __func__, netopt2str(opt));
            break;
    }
    return res;
}

static void _isr(netdev_t *netdev)
{
    DEBUG("%s: %p\n", __func__, netdev);

    CHECK_PARAM(netdev != NULL);

    esp_now_netdev_t *dev = (esp_now_netdev_t*)netdev;

    if (dev->recv_event) {
        dev->recv_event = false;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
    else if (dev->scan_event) {
        dev->scan_event = false;
        esp_now_scan_peers_start();
    }
    return;
}

static const netdev_driver_t _esp_now_driver =
{
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

/** @} */
