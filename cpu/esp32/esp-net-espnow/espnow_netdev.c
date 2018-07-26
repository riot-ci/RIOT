/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_net_espnow
 * @{
 *
 * @file
 * @brief       Netdev interface for the ESP-NOW WiFi P2P protocol
 *
 * With ESP-NOW the ESP32 SDK provides a connectionless communication
 * technology, featuring short packet transmission. It applies the IEEE802.11
 * Action Vendor frame technology, along with the IE function developed by
 * Espressif, and CCMP encryption technology, realizing a secure, connectionless
 * communication solution.
 *
 * This netdev driver uses ESP-NOW to realize an link layer interface to a
 * meshed network of ESP32 nodes. In this network, each node can send short
 * packets to all other nodes that are visible in its area.
 *
 * The ESP32 nodes are used in the ESP-NOW COMBO role along with the
 * integrated WiFi interface's SoftAP + station mode to advertise their SSID
 * and become visible to other ESP32 nodes.
 *
 * The SSID of an ESP32 node is the concatenation of the prefix "RIOT_ESP_"
 * with the MAC address of its SoftAP WiFi interface. The driver periodically
 * scans all visible ESP32 nodes. The period can be configured by driver
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

#ifdef MODULE_ESP_NET_ESPNOW

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

#include "esp_common.h"
#include "esp_attr.h"
#include "esp_event_loop.h"
#include "esp_now.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "irq_arch.h"

#include "nvs_flash/include/nvs_flash.h"

#include "espnow_params.h"
#include "espnow_netdev.h"

#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6/nib.h"

#define ESPNOW_UNICAST          1

#define ESPNOW_ROLE_IDLE        0
#define ESPNOW_ROLE_CONTROLLER  1
#define ESPNOW_ROLE_SLAVE       2
#define ESPNOW_ROLE_COMBO       3

#define ESPNOW_WIFI_STA         1
#define ESPNOW_WIFI_SOFTAP      2
#define ESPNOW_WIFI_STA_SOFTAP  (ESPNOW_WIFI_STA + ESPNOW_WIFI_SOFTAP)

#define ESPNOW_AP_PREFIX        "RIOT_ESP_"
#define ESPNOW_AP_PREFIX_LEN    (strlen(ESPNOW_AP_PREFIX))

/**
 * There is only one ESP-NOW device. We define it as static device variable
 * to have accesss to the device inside ESP-NOW interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP-NOW
 * device which triggers the interrupt.
 */
static espnow_netdev_t _espnow_dev;
static const netdev_driver_t _espnow_driver;

/* device thread stack */
static char _espnow_stack[ESPNOW_STACKSIZE];

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

#if ESPNOW_UNICAST
static xtimer_t _espnow_scan_peers_timer;
static bool _espnow_scan_peers_done = false;

static bool _espnow_add_peer (uint8_t* bssid, uint8_t channel, uint8_t* key)
{
    if (esp_now_is_peer_exist(bssid)) {
        return false;
    }

    esp_now_peer_info_t peer = {};

    memcpy(peer.peer_addr, bssid, ESP_NOW_ETH_ALEN);
    peer.channel = channel;
    peer.ifidx = ESP_IF_WIFI_AP;

    if (espnow_params.key) {
        peer.encrypt = true;
        memcpy(peer.lmk, espnow_params.key, ESP_NOW_KEY_LEN);
    }

    esp_err_t ret = esp_now_add_peer(&peer);
    DEBUG("esp_now_add_peer node %02x:%02x:%02x:%02x:%02x:%02x "
          "added with return value %d\n",
          bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], ret);
    return (ret == ESP_OK);
}

#define ESPNOW_APS_BLOCK_SIZE 8 /* has to be power of two */

static wifi_ap_record_t* aps = NULL;
static uint32_t aps_size = 0;

static void IRAM_ATTR espnow_scan_peers_done (void)
{
    mutex_lock(&_espnow_dev.dev_lock);

    esp_err_t ret;
    uint16_t ap_num;
    uint32_t state;

    ret = esp_wifi_scan_get_ap_num(&ap_num);
    DEBUG("wifi_scan_get_ap_num ret=%d num=%d\n", ret ,ap_num);

    if (ret == ESP_OK && ap_num) {
        /* reallocation of memory must not be disturbed */
        critical_enter_var(state);
        /* allocate memory for APs record list blockwise and fetch them the list */
        if (ap_num > aps_size) {
            if (aps) {
                /* free allocated AP record list memory */
                aps_size = 0;
                free (aps);
            }
            /* allocate new memory */
            aps_size = (ap_num & ~(ESPNOW_APS_BLOCK_SIZE - 1)) + ESPNOW_APS_BLOCK_SIZE;
            aps = malloc(sizeof(wifi_ap_record_t) * aps_size);
            ap_num = aps_size;
        }
        critical_exit_var(state);

        ret = esp_wifi_scan_get_ap_records(&ap_num, aps);

        DEBUG("wifi_scan_get_aps ret=%d num=%d\n", ret, ap_num);

        critical_enter_var(state);
        /* iterate over APs records */
        for (uint16_t i = 0; i < ap_num; i++) {

            /* check whether the AP is an ESPNOW node which is not already a peer */
            if (strncmp((char*)aps[i].ssid, ESPNOW_AP_PREFIX, ESPNOW_AP_PREFIX_LEN) == 0 &&
                !esp_now_is_peer_exist(aps[i].bssid)) {
                /* add the AP as peer */
                _espnow_add_peer(aps[i].bssid, aps[i].primary, espnow_params.key);
            }
        }
        critical_exit_var(state);
    }

    #if ENABLE_DEBUG
    esp_now_peer_num_t peer_num;
    esp_now_get_peer_num (&peer_num);
    DEBUG("associated peers total=%d, encrypted=%d\n",
          peer_num.total_num, peer_num.encrypt_num);
    #endif

    _espnow_scan_peers_done = true;

    /* set the time for next scan */
    xtimer_set (&_espnow_scan_peers_timer, espnow_params.scan_period);

    mutex_unlock(&_espnow_dev.dev_lock);
}

static void espnow_scan_peers_start (void)
{
    DEBUG("%s\n", __func__);

    wifi_scan_config_t scan_cfg = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = espnow_params.channel,
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active.min = 0,
        .scan_time.active.max = 120 /* TODO tune value */
    };

    esp_wifi_scan_start(&scan_cfg, false);
}

#define ESPNOW_EVENT_SCAN_PEERS 1

static kernel_pid_t espnow_event_handler_pid;
char espnow_event_handler_stack [THREAD_STACKSIZE_DEFAULT];

static void *espnow_event_handler (void *arg)
{
    msg_t event;
    while (1) {
        msg_receive(&event);
        switch (event.content.value) {
            case ESPNOW_EVENT_SCAN_PEERS:
                espnow_scan_peers_start ();
                break;
        }
    }
    return NULL;
}

static void IRAM_ATTR espnow_scan_peers_timer_cb (void* arg)
{
    DEBUG("%s\n", __func__);

    static msg_t event = { .content = { .value = ESPNOW_EVENT_SCAN_PEERS } };
    msg_send(&event, espnow_event_handler_pid);
}

#else

static const uint8_t _espnow_mac[6] = { 0x82, 0x73, 0x79, 0x84, 0x79, 0x83 }; /* RIOTOS */

#endif /* ESPNOW_UNICAST */

static IRAM_ATTR void espnow_recv_cb (const uint8_t *mac, const uint8_t *data, int len)
{
    #if ESPNOW_UNICAST
    if (!_espnow_scan_peers_done) {
        /* if peers are not scanned, we cannot receive anything */
        return;
    }
    #endif

    if (_espnow_dev.rx_len) {
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

    _espnow_dev.rx_len = len;
    memcpy(_espnow_dev.rx_buf, data, len);
    memcpy(_espnow_dev.rx_mac, mac, ESPNOW_ADDR_LEN);

    if (_espnow_dev.netdev.event_callback) {
        _espnow_dev.netdev.event_callback((netdev_t*)&_espnow_dev, NETDEV_EVENT_ISR);
    }

    critical_exit();
}

static int _espnow_sending = 0;

static void IRAM_ATTR espnow_send_cb (const uint8_t *mac, esp_now_send_status_t status)
{
    DEBUG("%s: sent to %02x:%02x:%02x:%02x:%02x:%02x with status %d\n",
          __func__,
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], status);

    if (_espnow_sending) {
        _espnow_sending--;
    }
}

/*
 * Event handler for esp system events.
 */
static esp_err_t IRAM_ATTR _esp_system_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            DEBUG("%s WiFi started\n", __func__);
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            DEBUG("%s WiFi scan done\n", __func__);
            espnow_scan_peers_done();
            break;
        default:
            break;
    }
    return ESP_OK;
}

/**
 * Default WiFi configuration, overwrite them with your configs
 */
#ifndef CONFIG_WIFI_STA_SSID
#define CONFIG_WIFI_STA_SSID        "RIOT_AP"
#endif
#ifndef CONFIG_WIFI_STA_PASSWORD
#define CONFIG_WIFI_STA_PASSWORD    "ThisistheRIOTporttoESP"
#endif
#ifndef CONFIG_WIFI_STA_CHANNEL
#define CONFIG_WIFI_STA_CHANNEL     0
#endif

#define CONFIG_WIFI_STA_SCAN_METHOD WIFI_ALL_CHANNEL_SCAN
#define CONFIG_WIFI_STA_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#define CONFIG_WIFI_STA_RSSI        -127
#define CONFIG_WIFI_STA_AUTHMODE    WIFI_AUTH_WPA_WPA2_PSK

#define CONFIG_WIFI_AP_PASSWORD     ESPNOW_SOFT_AP_PASSPHRASE
#define CONFIG_WIFI_AP_CHANNEL      ESPNOW_CHANNEL
#define CONFIG_WIFI_AP_AUTH         WIFI_AUTH_WPA_WPA2_PSK
#define CONFIG_WIFI_AP_HIDDEN       false
#define CONFIG_WIFI_AP_BEACON       100
#define CONFIG_WIFI_AP_MAX_CONN     4

extern esp_err_t esp_system_event_add_handler (system_event_cb_t handler,
                                               void *arg);

static void espnow_setup (espnow_netdev_t* dev)
{
    DEBUG("%s: %p\n", __func__, dev);

    /*
     * Init the WiFi driver. TODO It is not only required before ESPNOW is
     * initialized but also before other WiFi functions are used. Once other
     * WiFi functions are realized it has to be moved to a more common place.
     */
    extern portMUX_TYPE g_intr_lock_mux;
    mutex_init(&g_intr_lock_mux);

    esp_system_event_add_handler (_esp_system_event_handler, NULL);

    esp_err_t result;
    #if CONFIG_ESP32_WIFI_NVS_ENABLED
    result = nvs_flash_init();
    if (result != ESP_OK) {
        LOG_ERROR("nfs_flash_init failed with return value %d\n", result);
        return;
    }
    #endif

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result = esp_wifi_init(&cfg);
    if (result != ESP_OK) {
        LOG_ERROR("esp_wifi_init failed with return value %d\n", result);
        return;
    }

    #ifdef CONFIG_WIFI_COUNTRY
    /* TODO */
    #endif

    /* we use predefined station configuration */
    wifi_config_t wifi_config_sta = {
        .sta = {
            .ssid = CONFIG_WIFI_STA_SSID,
            .password = CONFIG_WIFI_STA_PASSWORD,
            .channel = CONFIG_WIFI_STA_CHANNEL,
            .scan_method = CONFIG_WIFI_STA_SCAN_METHOD,
            .sort_method = CONFIG_WIFI_STA_SORT_METHOD,
            .threshold.rssi = CONFIG_WIFI_STA_RSSI,
            .threshold.authmode = CONFIG_WIFI_STA_AUTHMODE
        }
    };

    /* get SoftAP interface mac address and store it as device addresss */
    esp_read_mac(dev->addr, ESP_MAC_WIFI_SOFTAP);

    /* prepare the ESPNOW configuration for SoftAP */
    wifi_config_t wifi_config_ap = {};

    strcpy ((char*)wifi_config_ap.ap.password, espnow_params.softap_pass);
    sprintf((char*)wifi_config_ap.ap.ssid, "%s%02x%02x%02x%02x%02x%02x",
            ESPNOW_AP_PREFIX,
            dev->addr[0], dev->addr[1], dev->addr[2],
            dev->addr[3], dev->addr[4], dev->addr[5]);
    wifi_config_ap.ap.ssid_len = strlen((char*)wifi_config_ap.ap.ssid);

    wifi_config_ap.ap.channel = espnow_params.channel;
    wifi_config_ap.ap.authmode = CONFIG_WIFI_AP_AUTH;
    wifi_config_ap.ap.ssid_hidden = CONFIG_WIFI_AP_HIDDEN;
    wifi_config_ap.ap.max_connection = CONFIG_WIFI_AP_MAX_CONN;
    wifi_config_ap.ap.beacon_interval = CONFIG_WIFI_AP_BEACON;

    /* set the WiFi interface to Station + SoftAP mode without DHCP */
    result = esp_wifi_set_mode(WIFI_MODE_STA | WIFI_MODE_AP);
    if (result != ESP_OK) {
        LOG_ERROR("esp_wifi_set_mode failed with return value %d\n", result);
        return;
    }

    /* set the Station and SoftAP configuration */
    result = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta);
    if (result != ESP_OK) {
        LOG_ERROR("esp_wifi_set_config station failed with return value %d\n", result);
        return;
    }
    result = esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_ap);
    if (result != ESP_OK) {
        LOG_ERROR("esp_wifi_set_mode softap failed with return value %d\n", result);
        return;
    }

    /* start the WiFi driver */
    result = esp_wifi_start();
    if (result != ESP_OK) {
        LOG_ERROR("esp_wifi_start failed with return value %d\n", result);
        return;
    }

    #if ESPNOW_UNICAST==0 /* TODO */
    /* all ESP-NOW nodes get the shared mac address on their station interface */
    wifi_set_macaddr(STATION_IF, (uint8_t*)_espnow_mac);
    #endif

    /* set the netdev driver */
    dev->netdev.driver = &_espnow_driver;

    /* initialize netdev data structure */
    dev->peers_all = 0;
    dev->peers_enc = 0;
    mutex_init(&dev->dev_lock);

    /* initialize ESP-NOW and register callback functions */
    result = esp_now_init();
    if (result != ESP_OK) {
        LOG_ERROR("esp_now_init failed with return value %d\n", result);
        return;
    }
    esp_now_register_send_cb (espnow_send_cb);
    esp_now_register_recv_cb (espnow_recv_cb);

    #if ESPNOW_UNICAST
    /* create the ESPNOW event handler thread */
    espnow_event_handler_pid = thread_create(espnow_event_handler_stack,
                                             sizeof(espnow_event_handler_stack),
                                             ESPNOW_PRIO + 1,
                                             THREAD_CREATE_WOUT_YIELD |
                                             THREAD_CREATE_STACKTEST,
                                             espnow_event_handler,
                                             NULL, "espnow-event-loop");

    /* timer for peer scan initialization */
    _espnow_scan_peers_done = false;
    _espnow_scan_peers_timer.callback = &espnow_scan_peers_timer_cb;
    _espnow_scan_peers_timer.arg = dev;

    /* execute the first scan */
    espnow_scan_peers_done();

    #else /* ESPNOW_UNICAST */
    #if 0
    int res = esp_now_add_peer((uint8_t*)_espnow_mac, ESP_NOW_ROLE_COMBO,
                               espnow_params.channel, NULL, 0);
    DEBUG("%s: multicast node added %d\n", __func__, res);
    #endif
    #endif /* ESPNOW_UNICAST */
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
    #if ESPNOW_UNICAST
    if (!_espnow_scan_peers_done) {
        return -ENODEV;
    }
    #endif

    DEBUG("%s: %p %p\n", __func__, netdev, iolist);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (iolist != NULL, -EINVAL);

    espnow_netdev_t* dev = (espnow_netdev_t*)netdev;

    mutex_lock(&dev->dev_lock);
    dev->tx_len = 0;

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (dev->tx_len + iol->iol_len > ESPNOW_MAX_SIZE) {
            mutex_unlock(&dev->dev_lock);
            return -EOVERFLOW;
        }
        memcpy (dev->tx_buf + dev->tx_len, iol->iol_base, iol->iol_len);
        dev->tx_len += iol->iol_len;
    }

    #if ENABLE_DEBUG
    printf ("%s: send %d byte\n", __func__, dev->tx_len);
    /* hexdump (dev->tx_buf, dev->tx_len, 'b', 16); */
    #endif

    _espnow_sending = 1;

    uint8_t* _espnow_dst = 0;

    #if ESPNOW_UNICAST
    ipv6_hdr_t* ipv6_hdr = (ipv6_hdr_t*)dev->tx_buf;
    uint8_t  _espnow_dst_from_iid[6];

    if (ipv6_hdr->dst.u8[0] == 0xff) {
        /* packets to multicast prefix ff::/8 are sent to all peers */
        DEBUG("multicast to all peers\n");
        _espnow_dst = 0;
        _espnow_sending = dev->peers_all;

        #ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_mcast_count++;
        #endif
    }

    else if ((byteorder_ntohs(ipv6_hdr->dst.u16[0]) & 0xffc0) == 0xfe80) {
        /* for link local addresses fe80::/10, the MAC address is derived from dst address */
        _get_mac_from_iid(&ipv6_hdr->dst.u8[8], _espnow_dst_from_iid);
        DEBUG("link local to %02x:%02x:%02x:%02x:%02x:%02x\n",
              _espnow_dst_from_iid[0], _espnow_dst_from_iid[1],
              _espnow_dst_from_iid[2], _espnow_dst_from_iid[3],
              _espnow_dst_from_iid[4], _espnow_dst_from_iid[5]);
        _espnow_dst = _espnow_dst_from_iid;
        _espnow_sending = 1;
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
            _espnow_dst = nce.l2addr;
            _espnow_sending = 1;
        }
        else {
            /* entry was not found in NIB, send to all peers */
            DEBUG("global, no neibhbor found, multicast to all peers\n");
            _espnow_dst = 0;
            _espnow_sending = dev->peers_all;

            #ifdef MODULE_NETSTATS_L2
            netdev->stats.tx_mcast_count++;
            #endif
        }
    }

    #else /* ESPNOW_UNICAST */

    ipv6_hdr_t* ipv6_hdr = (ipv6_hdr_t*)dev->tx_buf;
    uint8_t  _espnow_dst_from_iid[6];

    _espnow_dst = (uint8_t*)_espnow_mac;
    _espnow_sending = 1;

    if (ipv6_hdr->dst.u8[0] == 0xff) {
        /* packets to multicast prefix ff::/8 are sent to all peers */
        DEBUG("multicast to all peers\n");

        #ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_mcast_count++;
        #endif
    }

    else if ((byteorder_ntohs(ipv6_hdr->dst.u16[0]) & 0xffc0) == 0xfe80) {
        /* for link local addresses fe80::/10, the MAC address is derived from dst address */
        _get_mac_from_iid(&ipv6_hdr->dst.u8[8], _espnow_dst_from_iid);
        DEBUG("link local to %02x:%02x:%02x:%02x:%02x:%02x\n",
              _espnow_dst_from_iid[0], _espnow_dst_from_iid[1],
              _espnow_dst_from_iid[2], _espnow_dst_from_iid[3],
              _espnow_dst_from_iid[4], _espnow_dst_from_iid[5]);
        if (esp_now_is_peer_exist(_espnow_dst_from_iid) > 0) {
            _espnow_dst = _espnow_dst_from_iid;
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
            _espnow_dst = nce.l2addr;
        }
        else {
            /* entry was not found in NIB, send to all peers */
            DEBUG("global, no neibhbor found, multicast to all peers\n");

            #ifdef MODULE_NETSTATS_L2
            netdev->stats.tx_mcast_count++;
            #endif
        }
    }

    #endif /* ESPNOW_UNICAST */
    if (_espnow_dst) {
        DEBUG("%s: send to espnow addr %02x:%02x:%02x:%02x:%02x:%02x\n", __func__,
              _espnow_dst[0], _espnow_dst[1], _espnow_dst[2],
              _espnow_dst[3], _espnow_dst[4], _espnow_dst[5]);
    }

    /* send the the packet to the peer(s) mac address */
    if (esp_now_send (_espnow_dst, dev->tx_buf, dev->tx_len) == 0) {
        while (_espnow_sending > 0) {
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

    espnow_netdev_t* dev = (espnow_netdev_t*)netdev;

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
            DEBUG("[espnow] No space in receive buffers\n");
            mutex_unlock(&dev->dev_lock);
            return -ENOBUFS;
        }

        #if ENABLE_DEBUG
        printf ("%s: received %d byte from %02x:%02x:%02x:%02x:%02x:%02x\n",
                __func__, dev->rx_len,
                dev->rx_mac[0], dev->rx_mac[1], dev->rx_mac[2],
                dev->rx_mac[3], dev->rx_mac[4], dev->rx_mac[5]);
        /* hexdump (dev->rx_buf, dev->rx_len, 'b', 16); */
        #endif

        if (esp_now_is_peer_exist(dev->rx_mac) <= 0) {
            _espnow_add_peer(dev->rx_mac, espnow_params.channel, espnow_params.key);
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

static inline int _get_iid(espnow_netdev_t *dev, eui64_t *value, size_t max_len)
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

    espnow_netdev_t *dev = (espnow_netdev_t *)netdev;
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
            *((uint16_t *)val) = ESPNOW_MAX_SIZE;
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

    espnow_netdev_t *dev = (espnow_netdev_t *) netdev;
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

    espnow_netdev_t *dev = (espnow_netdev_t *) netdev;
    dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    return;
}

static const netdev_driver_t _espnow_driver =
{
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

void auto_init_esp_net_espnow (void)
{
    LOG_DEBUG("[netif] initializing espnow device\n");

    espnow_setup(&_espnow_dev);
    _espnow_dev.netif = gnrc_netif_raw_create(_espnow_stack,
                                              ESPNOW_STACKSIZE, ESPNOW_PRIO,
                                              "espnow-netdev",
                                              (netdev_t *)&_espnow_dev);
}

#endif /* MODULE_ESP_NET_ESPNOW */
