/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_wifi
 * @{
 *
 * @file
 * @brief       Network device driver for the ESP32 WiFi interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifdef MODULE_ESP_WIFI

#include "log.h"
#include "tools.h"

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "net/gnrc/netif/ethernet.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc.h"
#include "net/ethernet.h"
#include "net/netdev/eth.h"
#include "od.h"
#include "xtimer.h"

#include "esp_common.h"
#include "esp_attr.h"
#include "esp_event_loop.h"
#include "esp_now.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wifi_internal.h"
#include "irq_arch.h"

#include "nvs_flash/include/nvs_flash.h"

#include "esp_wifi_params.h"
#include "esp_wifi_netdev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define ESP_WIFI_DEBUG(f, ...) \
        DEBUG("[esp_wifi] %s: " f "\n", __func__, ## __VA_ARGS__)

#define ESP_WIFI_LOG_INFO(f, ...) \
        LOG_INFO("[esp_wifi] " f "\n", ## __VA_ARGS__)

#define ESP_WIFI_LOG_ERROR(f, ...) \
        LOG_ERROR("[esp_wifi] " f "\n", ## __VA_ARGS__)

#define ESP_WIFI_EVENT_RX_DONE          BIT(0)
#define ESP_WIFI_EVENT_TX_DONE          BIT(1)
#define ESP_WIFI_EVENT_STA_CONNECTED    BIT(2)
#define ESP_WIFI_EVENT_STA_DISCONNECTED BIT(3)

#define MAC_STR                         "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_STR_ARG(m)                  m[0], m[1], m[2], m[3], m[4], m[5]

/**
 * There is only one ESP WiFi device. We define it as static device variable
 * to have accesss to the device inside ESP WiFi interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP WiFi
 * device which triggers the interrupt.
 */
esp_wifi_netdev_t _esp_wifi_dev;
static const netdev_driver_t _esp_wifi_driver;

/* device thread stack */
static char _esp_wifi_stack[ESP_WIFI_STACKSIZE];

extern esp_err_t esp_system_event_add_handler (system_event_cb_t handler,
                                               void *arg);

/** guard variable to avoid reentrance to _send */
static bool _in_send = false;

/** guard variable to avoid reentrance to _esp_wifi_recv_cb */
static bool _in_esp_wifi_recv_cb = false;

/**
 * Callback when ethernet frame is received. Has to run in IRAM.
 */
esp_err_t _esp_wifi_rx_cb(void *buffer, uint16_t len, void *eb)
{
    assert(buffer);
    assert(eb);

    /*
     * Function `_esp_wifi_rx_cb` is executed in the context of the
     * `wifi-event-loop` thread. ISRs which handle hardware interrupts from
     * the WiFi interface simply pass events to a message queue of the
     * `wifi-event-loop` thread which are then sequentially processed by the
     * `wifi-event-loop` thread to asynchronously execute callback functions
     * such as `esp_wifi_recv_cb`.
     *
     * It should be therefore not possible to reenter function
     * `_esp_wifi_rx_cb`. If it does occur inspite of that, we use a
     * guard variable to avoid inconsistencies. This can not be realized
     * by a mutex because `_esp_wifi_rx_cb` would be reentered from same
     * thread context.
     */
    ESP_WIFI_DEBUG("buf=%p len=%d eb=%p", buffer, len, eb);

    if (_in_esp_wifi_recv_cb) {
        esp_wifi_internal_free_rx_buffer(eb);
        return ESP_ERR_INVALID_STATE;
    }
    _in_esp_wifi_recv_cb = true;

    critical_enter();

    /* check whether the receive buffer is already holding a frame */
    if (_esp_wifi_dev.rx_len) {
ets_printf("#");
        ESP_WIFI_DEBUG("buffer used, dropping incoming frame of %d bytes",
                       len);
        esp_wifi_internal_free_rx_buffer(eb);
        _in_esp_wifi_recv_cb = false;
        critical_exit();
        return ESP_ERR_INVALID_STATE;
    }
ets_printf(".");

    /* check packet buffer for the minimum packet size */
    if (len < sizeof(ethernet_hdr_t)) {
        ESP_WIFI_DEBUG("frame length is less than the size of an Ethernet"
                       "header (%u < %u)", len, sizeof(ethernet_hdr_t));
        esp_wifi_internal_free_rx_buffer(eb);
        _in_esp_wifi_recv_cb = false;
        critical_exit();
        return ESP_ERR_INVALID_SIZE;
    }

    /* check whether packet buffer fits into receive buffer */
    if (len > ETHERNET_MAX_LEN) {
        ESP_WIFI_DEBUG("frame length is greater than the maximum size of an "
                       "Ethernet frame (%u > %u)", len, ETHERNET_MAX_LEN);
        esp_wifi_internal_free_rx_buffer(eb);
        _in_esp_wifi_recv_cb = false;
        critical_exit();
        return ESP_ERR_INVALID_SIZE;
    }
#if 0
    /* copy the buffer and free WiFi driver buffer */
    memcpy(_esp_wifi_dev.rx_buf, buffer, len);
    esp_wifi_internal_free_rx_buffer(eb);

    /*
     * Because this function is not executed in interrupt context but in thread
     * context, following msg_send could block on heavy network load, if frames
     * are coming in faster than the ISR events can be handled. To avoid
     * blocking during msg_send, we pretend we are in an ISR by incrementing
     * the IRQ nesting counter. If IRQ nesting counter is greater 0, function
     * irq_is_in returns true and the non-blocking version of msg_send is used.
     */
    irq_interrupt_nesting++;

    /* trigger netdev event to read the data */
    _esp_wifi_dev.rx_len = len;
    _esp_wifi_dev.event |= ESP_WIFI_EVENT_RX_DONE;
    _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

    /* reset IRQ nesting counter */
    irq_interrupt_nesting--;
#else
    /* we have to store the frame in the buffer and free eb immediatly */
    _esp_wifi_dev.rx_len = len;
    memcpy(_esp_wifi_dev.rx_buf, buffer, len);
    esp_wifi_internal_free_rx_buffer(eb);

    /*
     * Since _esp_wifi_rx_cb is not executed in interrupt context but in the
     * context of the `wifi-event-loop` thread, it is not necessary to pass the
     * `NETDEV_EVENT_ISR` event first. Instead, the receive function can be
     * called directly which results in much faster handling, a less frame lost
     * rate and more robustness. There is no need for a mutex anymore to
     * synchronize the access to the receive buffer between _esp_wifi_rx_cb
     * and _esp_wifi_recv function.
     */
    if (_esp_wifi_dev.netdev.event_callback) {
        _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev,
                                            NETDEV_EVENT_RX_COMPLETE);
    }
#endif

    critical_exit();
    _in_esp_wifi_recv_cb = false;

    return ESP_OK;
}

/*
 * Event handler for esp system events.
 */
static esp_err_t IRAM_ATTR _esp_system_event_handler(void *ctx, system_event_t *event)
{
    esp_err_t result;

    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            ESP_WIFI_DEBUG("WiFi started");
            result = esp_wifi_connect();
            if (result != ESP_OK) {
                LOG_TAG_ERROR("esp_wifi", "esp_wifi_connect failed with return "
                              "value %d\n", result);
            }
            break;

        case SYSTEM_EVENT_SCAN_DONE:
            ESP_WIFI_DEBUG("WiFi scan done");
            break;

        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_WIFI_DEBUG("WiFi connected to ssid %s",
                           event->event_info.connected.ssid);

            /* register RX callback function */
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, _esp_wifi_rx_cb);

            _esp_wifi_dev.connected = true;
            _esp_wifi_dev.event |= ESP_WIFI_EVENT_STA_CONNECTED;
            _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_WIFI_DEBUG("WiFi disconnected from ssid %s, reason %d",
                           event->event_info.disconnected.ssid,
                           event->event_info.disconnected.reason);

            /* unregister RX callback function */
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);

            _esp_wifi_dev.connected = false;
            _esp_wifi_dev.event |= ESP_WIFI_EVENT_STA_DISCONNECTED;
            _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

            /* call disconnect to reset internal state */
            result = esp_wifi_disconnect();
            if (result != ESP_OK) {
                LOG_TAG_ERROR("esp_wifi", "esp_wifi_disconnect failed with "
                             "return value %d\n", result);
                return result;
            }

            /* try to reconnect */
            result = esp_wifi_connect();
            if (result != ESP_OK) {
               LOG_TAG_ERROR("esp_wifi", "esp_wifi_connect failed with "
                             "return value %d\n", result);
            }

            break;

        default:
            ESP_WIFI_DEBUG("event %d", event->event_id);
            break;
    }
    return ESP_OK;
}

/* we use predefined station configuration */
static wifi_config_t wifi_config_sta = {
    .sta = {
        .ssid = ESP_WIFI_SSID,
        .password = ESP_WIFI_PASS,
        .bssid_set = 0,
        .channel = 0,
        .scan_method = WIFI_ALL_CHANNEL_SCAN,
        .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
        .threshold.rssi = -127,
        .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK
    }
};

void esp_wifi_setup (esp_wifi_netdev_t* dev)
{
    ESP_WIFI_DEBUG("%p", dev);

    /* initialize buffer */
    dev->rx_len = 0;

    /* set the event handler */
    esp_system_event_add_handler(_esp_system_event_handler, NULL);

    /*
     * Init the WiFi driver. TODO It is not only required before ESP_WIFI is
     * initialized but also before other WiFi functions are used. Once other
     * WiFi functions are realized it has to be moved to a more common place.
     */
    esp_err_t result;

#ifndef MODULE_ESP_NOW
    /* if esp_now is used, the following part is already done */
    extern portMUX_TYPE g_intr_lock_mux;
    mutex_init(&g_intr_lock_mux);

#if CONFIG_ESP32_WIFI_NVS_ENABLED
    result = nvs_flash_init();
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_wifi", "nfs_flash_init failed "
                      "with return value %d\n", result);
        return;
    }
#endif /* CONFIG_ESP32_WIFI_NVS_ENABLED */

    /* initialize the WiFi driver with default configuration */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result = esp_wifi_init(&cfg);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_wifi", "esp_wifi_init failed "
                      "with return value %d\n", result);
        return;
    }

    /* set configuration storage type */
    result = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now", "esp_wifi_set_storage failed "
                      "with return value %d\n", result);
        return NULL;
    }

#ifdef CONFIG_WIFI_COUNTRY
    /* TODO */
#endif /* CONFIG_WIFI_COUNTRY */

    result = esp_wifi_set_mode(WIFI_MODE_STA);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_wifi", "esp_wifi_set_mode failed "
                      "with return value %d\n", result);
        return;
    }
#endif /* MODULE_ESP_NOW */

    /* set the Station configuration */
    result = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_wifi", "esp_wifi_set_config station failed "
                      "with return value %d\n", result);
        return;
    }

    /* start the WiFi driver */
    result = esp_wifi_start();
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_wifi", "esp_wifi_start failed "
                      "with return value %d\n", result);
        return;
    }

    /* register RX callback function */
    esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, _esp_wifi_rx_cb);

    /* set the netdev driver */
    dev->netdev.driver = &_esp_wifi_driver;

    /* initialize netdev data structure */
    dev->connected = false;
}

static int _esp_wifi_init(netdev_t *netdev)
{
    ESP_WIFI_DEBUG("%p", netdev);

    _esp_wifi_dev.event = 0; /* no event */

    return 0;
}

static int _esp_wifi_send(netdev_t *netdev, const iolist_t *iolist)
{
    ESP_WIFI_DEBUG("%p %p", netdev, iolist);

    assert(netdev != NULL);
    assert(iolist != NULL);

    if (_in_send) {
        return 0;
    }
    _in_send = true;

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;

    critical_enter();

    if (!_esp_wifi_dev.connected) {
        ESP_WIFI_DEBUG("WiFi is still not connected to AP, cannot send");
        _in_send = false;
        critical_exit();
        return -EIO;
    }

    dev->tx_len = 0;

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (dev->tx_len + iol->iol_len > ETHERNET_MAX_LEN) {
            _in_send = false;
            critical_exit();
           return -EOVERFLOW;
        }
        if (iol->iol_len) {
            memcpy (dev->tx_buf + dev->tx_len, iol->iol_base, iol->iol_len);
            dev->tx_len += iol->iol_len;
        }
    }

#if ENABLE_DEBUG
    const ethernet_hdr_t* hdr = (const ethernet_hdr_t *)dev->tx_buf;

    ESP_WIFI_DEBUG("send %u byte to " MAC_STR,
                   (unsigned)dev->tx_len, MAC_STR_ARG(hdr->dst));
#if MODULE_OD
    od_hex_dump(dev->tx_buf, dev->tx_len, OD_WIDTH_DEFAULT);
#endif /* MODULE_OD */
#endif /* ENABLE_DEBUG */

    int ret = 0;

    /* send the the packet to the peer(s) mac address */
    if (esp_wifi_internal_tx(ESP_IF_WIFI_STA, dev->tx_buf, dev->tx_len) == ESP_OK) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    else {
        ESP_WIFI_DEBUG("sending WiFi packet failed");
        ret = -EIO;
    }

    _in_send = false;
    critical_exit();

    return ret;
}

static int _esp_wifi_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    ESP_WIFI_DEBUG("%p %p %u %p", netdev, buf, len, info);

    assert(netdev != NULL);

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;

    uint16_t size = dev->rx_len ? dev->rx_len : 0;

    if (!buf) {
        /* get the size of the frame */
        if (len > 0 && size) {
            /* if len > 0, drop the frame */
            dev->rx_len = 0;
        }
        return size;
    }

    if (len < size) {
        /* buffer is smaller than the number of received bytes */
        ESP_WIFI_DEBUG("not enough space in receive buffer");
        /* newest API requires to drop the frame in that case */
        dev->rx_len = 0;
        return -ENOBUFS;
    }

    /* copy the buffer and free */
    memcpy(buf, dev->rx_buf, dev->rx_len);
    dev->rx_len = 0;

#if ENABLE_DEBUG
    ethernet_hdr_t *hdr = (ethernet_hdr_t *)buf;

    ESP_WIFI_DEBUG("received %u byte from addr " MAC_STR,
                   size, MAC_STR_ARG(hdr->src));
#if MODULE_OD
    od_hex_dump(buf, size, OD_WIDTH_DEFAULT);
#endif /* MODULE_OD */
#endif /* ENABLE_DEBUG */

    return size;
}

static int _esp_wifi_get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    ESP_WIFI_DEBUG("%s %p %p %u", netopt2str(opt), netdev, val, max_len);

    assert(netdev != NULL);
    assert(val != NULL);

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            esp_wifi_get_mac(ESP_MAC_WIFI_STA,(uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        case NETOPT_IS_WIRED:
            return false;
        case NETOPT_LINK_CONNECTED:
            assert(max_len == 1);
            *((netopt_enable_t *)val) = (dev->connected) ? NETOPT_ENABLE
                                                         : NETOPT_DISABLE;
            return 1;
        default:
            return netdev_eth_get(netdev, opt, val, max_len);
    }
}

static int _esp_wifi_set(netdev_t *netdev, netopt_t opt, const void *val, size_t max_len)
{
    ESP_WIFI_DEBUG("%s %p %p %u", netopt2str(opt), netdev, val, max_len);

    assert(netdev != NULL);
    assert(val != NULL);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len == ETHERNET_ADDR_LEN);
            esp_wifi_set_mac(ESP_MAC_WIFI_STA, (uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(netdev, opt, val, max_len);
    }
}

static void _esp_wifi_isr(netdev_t *netdev)
{
    ESP_WIFI_DEBUG("%p", netdev);

    assert(netdev != NULL);

    esp_wifi_netdev_t *dev = (esp_wifi_netdev_t *) netdev;

    if (dev->event & ESP_WIFI_EVENT_RX_DONE) {
        dev->event &= ~ESP_WIFI_EVENT_RX_DONE;
        if (dev->rx_len) {
ets_printf("i");
            dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
    }
    if (dev->event & ESP_WIFI_EVENT_TX_DONE) {
        dev->event &= ~ESP_WIFI_EVENT_TX_DONE;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    if (dev->event & ESP_WIFI_EVENT_STA_CONNECTED) {
        dev->event &= ~ESP_WIFI_EVENT_STA_CONNECTED;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_UP);
    }
    if (dev->event & ESP_WIFI_EVENT_STA_DISCONNECTED) {
        dev->event &= ~ESP_WIFI_EVENT_STA_DISCONNECTED;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
    }

    return;
}

static const netdev_driver_t _esp_wifi_driver =
{
    .send = _esp_wifi_send,
    .recv = _esp_wifi_recv,
    .init = _esp_wifi_init,
    .isr = _esp_wifi_isr,
    .get = _esp_wifi_get,
    .set = _esp_wifi_set,
};

void auto_init_esp_wifi (void)
{
    LOG_TAG_DEBUG("esp_wifi", "initializing ESP WiFi device\n");

    esp_wifi_setup(&_esp_wifi_dev);
    _esp_wifi_dev.netif = gnrc_netif_ethernet_create(_esp_wifi_stack,
                                                    ESP_WIFI_STACKSIZE,
#ifdef MODULE_ESP_NOW
                                                    ESP_WIFI_PRIO - 1,
#else
                                                    ESP_WIFI_PRIO,
#endif
                                                    "esp-wifi",
                                                    (netdev_t *)&_esp_wifi_dev);
}

#endif /* MODULE_ESP_WIFI */
/**@}*/
