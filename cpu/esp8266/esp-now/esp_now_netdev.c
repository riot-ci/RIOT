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
 */

#if MODULE_ESP_NOW

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "log.h"
#include "tools.h"

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "net/gnrc/netif/raw.h"
#include "net/gnrc.h"
#include "xtimer.h"

#include "common.h"
#include "espressif/c_types.h"
#include "espnow.h"
#include "esp_now_params.h"
#include "esp_now_netdev.h"
#include "irq_arch.h"
#include "esp/common_macros.h"
#include "sdk/sdk.h"

#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6/nib.h"

#define ESP_NOW_UNICAST          1

#define ESP_NOW_ROLE_IDLE        0
#define ESP_NOW_ROLE_CONTROLLER  1
#define ESP_NOW_ROLE_SLAVE       2
#define ESP_NOW_ROLE_COMBO       3

#define ESP_NOW_WIFI_STA         1
#define ESP_NOW_WIFI_SOFTAP      2
#define ESP_NOW_WIFI_STA_SOFTAP  (ESP_NOW_WIFI_STA + ESP_NOW_WIFI_SOFTAP)

#define ESP_NOW_AP_PREFIX        "RIOT_ESP_"

/**
 * There is only one ESP-NOW device. We define it as static device variable
 * to have accesss to the device inside ESP-NOW interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP-NOW
 * device which triggers the interrupt.
 */
static esp_now_netdev_t _esp_now_dev;
static const netdev_driver_t _esp_now_driver;

/* device thread stack */
static char _esp_now_stack[ESP_NOW_STACKSIZE];

static inline int _get_mac_from_iid(uint8_t *iid, uint8_t *mac)
{
    CHECK_PARAM_RET (iid != NULL, -EINVAL);
    CHECK_PARAM_RET (mac != NULL, -EINVAL);

    /* interface id according to */
    /* https://tools.ietf.org/html/rfc4291#section-2.5.1 */
    mac[0] = iid[0] ^ 0x02; /* invert bit1 */
    mac[1] = iid[1];
    mac[2] = iid[2];
    mac[3] = iid[5];
    mac[4] = iid[6];
    mac[5] = iid[7];

    return 0;
}

#if ESP_NOW_UNICAST
static xtimer_t _esp_now_scan_peers_timer;
static bool _esp_now_scan_peers_done = false;

static void IRAM esp_now_scan_peers_done_cb(void *arg, STATUS status)
{
    DEBUG("%s: %p %d\n", __func__, arg, status);

    CHECK_PARAM (status == OK);
    CHECK_PARAM (arg != NULL);

    critical_enter();

    struct bss_info *bss_link = (struct bss_info*)arg;

    while (bss_link) {
        if (strncmp((char*)bss_link->ssid, ESP_NOW_AP_PREFIX, strlen(ESP_NOW_AP_PREFIX)) == 0) {
            char*    mac_str = (char*)bss_link->ssid + strlen(ESP_NOW_AP_PREFIX);
            uint64_t mac_64 = strtoull(mac_str, 0, 16);
            uint8_t  mac[6];

            mac[0] = ((uint8_t*)&mac_64)[5];
            mac[1] = ((uint8_t*)&mac_64)[4];
            mac[2] = ((uint8_t*)&mac_64)[3];
            mac[3] = ((uint8_t*)&mac_64)[2];
            mac[4] = ((uint8_t*)&mac_64)[1];
            mac[5] = ((uint8_t*)&mac_64)[0];

            if (!esp_now_is_peer_exist(mac)) {
                int res = esp_now_add_peer(bss_link->bssid, ESP_NOW_ROLE_COMBO,
                                           bss_link->channel,
                                           esp_now_params.key, (esp_now_params.key) ? 16 : 0);
                DEBUG("%s: new node %s added %d\n", __func__, bss_link->ssid, res);
            }
            else {
                /* DEBUG("%s: existing node %s\n", __func__, bss_link->ssid); */
            }
        }
        bss_link = STAILQ_NEXT(bss_link, next);
    }

    esp_now_get_cnt_info (&_esp_now_dev.peers_all, &_esp_now_dev.peers_enc);
    #if ENABLE_DEBUG
    DEBUG("associated peers all=%d, encrypted=%d\n",
          _esp_now_dev.peers_all, _esp_now_dev.peers_enc);
    #endif

    _esp_now_scan_peers_done = true;

    xtimer_set (&_esp_now_scan_peers_timer, esp_now_params.scan_period);

    critical_exit();
    mutex_unlock(&_esp_now_dev.dev_lock);
}

static void esp_now_scan_peers (void* arg)
{
    DEBUG("%s\n", __func__);

    CHECK_PARAM (arg != NULL);

    esp_now_netdev_t* dev = (esp_now_netdev_t*)arg;
    mutex_lock(&dev->dev_lock);

    struct scan_config scan_conf =
    {
        .ssid = 0,
        .bssid = 0,
        .channel = esp_now_params.channel,
        .show_hidden = true
    };

    wifi_station_scan(&scan_conf, &esp_now_scan_peers_done_cb);
}

#else

static const uint8_t _esp_now_mac[6] = { 0x82, 0x73, 0x79, 0x84, 0x79, 0x83 }; /* RIOTOS */

#endif /* ESP_NOW_UNICAST */

static void IRAM esp_now_recv_cb(uint8_t *mac, uint8_t *data, uint8_t len)
{
    #if ESP_NOW_UNICAST
    if (!_esp_now_scan_peers_done) {
        /* if peers are not scanned, we cannot receive anything */
        return;
    }
    #endif

    if (_esp_now_dev.rx_len) {
        /* there is already a packet in receive buffer, we drop the new one */
        return;
    }

    critical_enter();

    #if 0 /* don't printf anything in ISR */
    printf ("%s\n", __func__);
    printf ("%s: received %d byte from %02x:%02x:%02x:%02x:%02x:%02x\n",
            __func__, len,
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    hexdump (data, len, 'b', 16);
    #endif

    _esp_now_dev.rx_len = len;
    memcpy(_esp_now_dev.rx_buf, data, len);
    memcpy(_esp_now_dev.rx_mac, mac, ESP_NOW_ADDR_LEN);

    if (_esp_now_dev.netdev.event_callback) {
        _esp_now_dev.netdev.event_callback((netdev_t*)&_esp_now_dev, NETDEV_EVENT_ISR);
    }

    critical_exit();
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

static void esp_now_setup (esp_now_netdev_t* dev)
{
    DEBUG("%s: %p\n", __func__, dev);

    /* set the WiFi interface to Station + SoftAP mode without DHCP */
    wifi_set_opmode_current(ESP_NOW_WIFI_STA_SOFTAP);
    wifi_softap_dhcps_stop();

    /* get SoftAP mac address and store it in device address */
    wifi_get_macaddr(SOFTAP_IF, dev->addr);
    #if ESP_NOW_UNICAST==0
    /* all ESP-NOW nodes get the shared mac address on their station interface */
    wifi_set_macaddr(STATION_IF, (uint8_t*)_esp_now_mac);
    #endif

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

    /* set the netdev driver */
    dev->netdev.driver = &_esp_now_driver;

    /* initialize netdev data structure */
    dev->peers_all = 0;
    dev->peers_enc = 0;
    mutex_init(&dev->dev_lock);

    /* initialize ESP-NOW */
    esp_now_init();
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

    #if ESP_NOW_UNICAST
    /* initialize the scan for other nodes */
    _esp_now_scan_peers_done = false;
    _esp_now_scan_peers_timer.callback = &esp_now_scan_peers;
    _esp_now_scan_peers_timer.arg = dev;
    esp_now_scan_peers(dev);
    #else /* ESP_NOW_UNICAST */
    #if 0
    int res = esp_now_add_peer((uint8_t*)_esp_now_mac, ESP_NOW_ROLE_COMBO,
                               esp_now_params.channel, NULL, 0);
    DEBUG("%s: multicast node added %d\n", __func__, res);
    #endif
    #endif /* ESP_NOW_UNICAST */

    esp_now_register_send_cb (esp_now_send_cb);
    esp_now_register_recv_cb (esp_now_recv_cb);
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
    #if ESP_NOW_UNICAST
    if (!_esp_now_scan_peers_done) {
        return -ENODEV;
    }
    #endif

    DEBUG("%s: %p %p\n", __func__, netdev, iolist);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (iolist != NULL, -EINVAL);

    esp_now_netdev_t* dev = (esp_now_netdev_t*)netdev;

    mutex_lock(&dev->dev_lock);
    dev->tx_len = 0;

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (dev->tx_len + iol->iol_len > ESP_NOW_MAX_SIZE) {
            mutex_unlock(&dev->dev_lock);
            return -EOVERFLOW;
        }
        memcpy (dev->tx_buf + dev->tx_len, iol->iol_base, iol->iol_len);
        dev->tx_len += iol->iol_len;
    }

    #if ENABLE_DEBUG
    printf ("%s: send %d byte\n", __func__, dev->tx_len);
    hexdump (dev->tx_buf, dev->tx_len, 'b', 16);
    #endif

    _esp_now_sending = 1;

    uint8* _esp_now_dst = 0;

    #if ESP_NOW_UNICAST
    ipv6_hdr_t* ipv6_hdr = (ipv6_hdr_t*)dev->tx_buf;
    uint8  _esp_now_dst_from_iid[6];

    if (ipv6_hdr->dst.u8[0] == 0xff) {
        /* packets to multicast prefix ff::/8 are sent to all peers */
        DEBUG("multicast to all peers\n");
        _esp_now_dst = 0;
        _esp_now_sending = dev->peers_all;

        #ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_mcast_count++;
        #endif
    }

    else if ((byteorder_ntohs(ipv6_hdr->dst.u16[0]) & 0xffc0) == 0xfe80) {
        /* for link local addresses fe80::/10, the MAC address is derived from dst address */
        _get_mac_from_iid(&ipv6_hdr->dst.u8[8], _esp_now_dst_from_iid);
        DEBUG("link local to %02x:%02x:%02x:%02x:%02x:%02x\n",
              _esp_now_dst_from_iid[0], _esp_now_dst_from_iid[1],
              _esp_now_dst_from_iid[2], _esp_now_dst_from_iid[3],
              _esp_now_dst_from_iid[4], _esp_now_dst_from_iid[5]);
        _esp_now_dst = _esp_now_dst_from_iid;
        _esp_now_sending = 1;
    }

    else {
        /* for other addresses, try to find an entry in NIB cache */
        gnrc_ipv6_nib_nc_t nce;
        int ret = gnrc_ipv6_nib_get_next_hop_l2addr (&ipv6_hdr->dst, dev->netif,
                                                     NULL, &nce);
        if (ret == 0) {
            /* entry was found in NIB, use MAC adress from the NIB cache entry */
            DEBUG("global, next hop to neighbor %02x:%02x:%02x:%02x:%02x:%02x\n",
                  nce.l2addr[0], nce.l2addr[1], nce.l2addr[2],
                  nce.l2addr[3], nce.l2addr[4], nce.l2addr[5]);
            _esp_now_dst = nce.l2addr;
            _esp_now_sending = 1;
        }
        else {
            /* entry was not found in NIB, send to all peers */
            DEBUG("global, no neibhbor found, multicast to all peers\n");
            _esp_now_dst = 0;
            _esp_now_sending = dev->peers_all;

            #ifdef MODULE_NETSTATS_L2
            netdev->stats.tx_mcast_count++;
            #endif
        }
    }

    #else /* ESP_NOW_UNICAST */

    ipv6_hdr_t* ipv6_hdr = (ipv6_hdr_t*)dev->tx_buf;
    uint8  _esp_now_dst_from_iid[6];

    _esp_now_dst = (uint8_t*)_esp_now_mac;
    _esp_now_sending = 1;

    if (ipv6_hdr->dst.u8[0] == 0xff) {
        /* packets to multicast prefix ff::/8 are sent to all peers */
        DEBUG("multicast to all peers\n");

        #ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_mcast_count++;
        #endif
    }

    else if ((byteorder_ntohs(ipv6_hdr->dst.u16[0]) & 0xffc0) == 0xfe80) {
        /* for link local addresses fe80::/10, the MAC address is derived from dst address */
        _get_mac_from_iid(&ipv6_hdr->dst.u8[8], _esp_now_dst_from_iid);
        DEBUG("link local to %02x:%02x:%02x:%02x:%02x:%02x\n",
              _esp_now_dst_from_iid[0], _esp_now_dst_from_iid[1],
              _esp_now_dst_from_iid[2], _esp_now_dst_from_iid[3],
              _esp_now_dst_from_iid[4], _esp_now_dst_from_iid[5]);
        if (esp_now_is_peer_exist(_esp_now_dst_from_iid) > 0) {
            _esp_now_dst = _esp_now_dst_from_iid;
        }
    }

    else
    {
        /* for other addresses, try to find an entry in NIB cache */
        gnrc_ipv6_nib_nc_t nce;
        int ret = gnrc_ipv6_nib_get_next_hop_l2addr (&ipv6_hdr->dst, dev->netif,
                                                     NULL, &nce);
        if (ret == 0 && esp_now_is_peer_exist(nce.l2addr) > 0) {
            /* entry was found in NIB, use MAC adress from the NIB cache entry */
            DEBUG("global, next hop to neighbor %02x:%02x:%02x:%02x:%02x:%02x\n",
                  nce.l2addr[0], nce.l2addr[1], nce.l2addr[2],
                  nce.l2addr[3], nce.l2addr[4], nce.l2addr[5]);
            _esp_now_dst = nce.l2addr;
        }
        else {
            /* entry was not found in NIB, send to all peers */
            DEBUG("global, no neibhbor found, multicast to all peers\n");

            #ifdef MODULE_NETSTATS_L2
            netdev->stats.tx_mcast_count++;
            #endif
        }
    }

    #endif /* ESP_NOW_UNICAST */

    if (_esp_now_dst) {
        DEBUG("%s: send to espnow addr %02x:%02x:%02x:%02x:%02x:%02x\n", __func__,
              _esp_now_dst[0], _esp_now_dst[1], _esp_now_dst[2],
              _esp_now_dst[3], _esp_now_dst[4], _esp_now_dst[5]);
    }
    else {
        DEBUG("%s: send to espnow addr = NULL", __func__);
    }

    /* send the the packet to the peer(s) mac address */
    if (esp_now_send (_esp_now_dst, dev->tx_buf, dev->tx_len) == 0) {
        while (_esp_now_sending > 0) {
            thread_yield_higher();
        }

        #ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_bytes += dev->tx_len;
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
        #endif

        mutex_unlock(&dev->dev_lock);
        return dev->tx_len;
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

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);

    esp_now_netdev_t* dev = (esp_now_netdev_t*)netdev;

    mutex_lock(&dev->dev_lock);

    uint8_t size = dev->rx_len;

    if (!buf && !len) {
        /* return the size without dropping received data */
        mutex_unlock(&dev->dev_lock);
        return size;
    }

    if (!buf && len) {
        /* return the size and drop received data */
        mutex_unlock(&dev->dev_lock);
        dev->rx_len = 0;
        return size;
    }

    if (buf && len && dev->rx_len) {
        if (dev->rx_len > len) {
            DEBUG("[mrf24j40] No space in receive buffers\n");
            mutex_unlock(&dev->dev_lock);
            return -ENOBUFS;
        }

        #if ENABLE_DEBUG
        printf ("%s: received %d byte from %02x:%02x:%02x:%02x:%02x:%02x\n",
                __func__, dev->rx_len,
                dev->rx_mac[0], dev->rx_mac[1], dev->rx_mac[2],
                dev->rx_mac[3], dev->rx_mac[4], dev->rx_mac[5]);
        hexdump (dev->rx_buf, dev->rx_len, 'b', 16);
        #endif

        if (esp_now_is_peer_exist(dev->rx_mac) <= 0) {
            int res = esp_now_add_peer(dev->rx_mac, ESP_NOW_ROLE_COMBO,
                                       esp_now_params.channel,
                                       esp_now_params.key, (esp_now_params.key) ? 16 : 0);
            DEBUG("%s: new node added %d\n", __func__, res);
            esp_now_get_cnt_info (&_esp_now_dev.peers_all, &_esp_now_dev.peers_enc);
        }

        memcpy(buf, dev->rx_buf, dev->rx_len);
        dev->rx_len = 0;

        #ifdef MODULE_NETSTATS_L2
        netdev->stats.rx_count++;
        netdev->stats.rx_bytes += size;
        #endif

        mutex_unlock(&dev->dev_lock);
        return size;
    }

    mutex_unlock(&dev->dev_lock);
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

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (val != NULL, -EINVAL);

    esp_now_netdev_t *dev = (esp_now_netdev_t *)netdev;
    int res = -ENOTSUP;

    switch (opt) {

        case NETOPT_DEVICE_TYPE:
            CHECK_PARAM_RET (max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = NETDEV_TYPE_RAW;
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
            CHECK_PARAM_RET (max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = ESP_NOW_MAX_SIZE;
            res = sizeof(uint16_t);
            break;

        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            CHECK_PARAM_RET (max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = sizeof(dev->addr);
            res = sizeof(uint16_t);
            break;

        case NETOPT_ADDRESS:
            CHECK_PARAM_RET (max_len >= sizeof(dev->addr), -EOVERFLOW);
            memcpy(val, dev->addr, sizeof(dev->addr));
            res = sizeof(dev->addr);
            break;

        case NETOPT_IPV6_IID:
            res = _get_iid(dev, val, max_len);
            break;

        #ifdef MODULE_NETSTATS_L2
        case NETOPT_STATS:
            CHECK_PARAM_RET (max_len == sizeof(uintptr_t), -EOVERFLOW);
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

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (val != NULL, -EINVAL);

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

    CHECK_PARAM (netdev != NULL);

    esp_now_netdev_t *dev = (esp_now_netdev_t *) netdev;
    dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
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

void auto_init_esp_now (void)
{
    LOG_DEBUG("[netif] initializing espnow0\n");

    esp_now_setup(&_esp_now_dev);
    _esp_now_dev.netif = gnrc_netif_raw_create(_esp_now_stack,
                                              ESP_NOW_STACKSIZE, ESP_NOW_PRIO,
                                              "espnow",
                                              (netdev_t *)&_esp_now_dev);
}

#endif /* MODULE_ESP_NOW */
