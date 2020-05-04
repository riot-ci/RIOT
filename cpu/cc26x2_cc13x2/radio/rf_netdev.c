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

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "cpu.h"

#define TX_BUF_SIZE (144)

static uint8_t _tx_buf0[TX_BUF_SIZE] __attribute__ ((aligned(4)));

#define RX_BUF_SIZE (144)

static uint8_t _rx_buf0[RX_BUF_SIZE] __attribute__ ((aligned(4)));
static uint8_t _rx_buf1[RX_BUF_SIZE] __attribute__ ((aligned(4)));
static uint8_t _rx_buf2[RX_BUF_SIZE] __attribute__ ((aligned(4)));
static uint8_t _rx_buf3[RX_BUF_SIZE] __attribute__ ((aligned(4)));

static rfc_data_queue_t _rx_queue; /**< RX queue */

static cc26x2_cc13x2_rf_netdev_t *_netdev; /**< Netdev pointer to use in IRQs */

/**
 * @brief   Is NETOPT_TX_END_IRQ enabled?
 */
static bool _tx_end_irq;

static void _rx_start(void);
static bool _is_in_rx(void);

static void _rfc_isr(void)
{
    /* Check if CPE_IRQ_RX_ENTRY_DONE is enabled and the flag is present */
    if ((RFC_DBELL->RFCPEIFG & CPE_IRQ_RX_ENTRY_DONE) &&
        (RFC_DBELL->RFCPEIEN & CPE_IRQ_RX_ENTRY_DONE)) {
        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_RX_ENTRY_DONE;

        _netdev->rx_complete = true;
        netdev_trigger_event_isr((netdev_t *)_netdev);
    }

    if (RFC_DBELL->RFCPEIFG & CPE_IRQ_LAST_COMMAND_DONE) {
        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;

        if (rf_cmd_prop_rx_adv.status == RFC_PROP_DONE_STOPPED ||
            rf_cmd_prop_rx_adv.status == RFC_PROP_DONE_ABORT) {
            DEBUG_PUTS("_rfc_isr: RX abort");
            rf_cmd_prop_rx_adv.status = 0;
            /* Do nothing */
        }
        else if (rf_cmd_prop_radio_div_setup.status == RFC_PROP_DONE_OK &&
                 rf_cmd_fs.status == RFC_DONE_OK) {
            DEBUG_PUTS("_rfc_isr: init done");

            rf_cmd_prop_radio_div_setup.status = 0;
            rf_cmd_fs.status = 0;
            _rx_start();
        }
        else if (rf_cmd_prop_tx_adv.status == RFC_PROP_DONE_OK) {
            DEBUG_PUTS("_rfc_isr: TX done");
            rf_cmd_prop_tx_adv.status = RFC_IDLE;
            _rx_start();
            if (_tx_end_irq) {
                _netdev->tx_complete = true;
                netdev_trigger_event_isr((netdev_t *)_netdev);
            }
        }
    }
}

static void _rx_start(void)
{
    DEBUG_PUTS("_rx_start()");
    rf_cmd_prop_rx_adv.status = RFC_PENDING;
    rf_cmd_prop_rx_adv.queue = &_rx_queue;

    /* Start RX */
    uint32_t cmdsta = rfc_send_command((rfc_op_t *)&rf_cmd_prop_rx_adv);

    if ((cmdsta & 0xff) != RFC_CMDSTA_DONE) {
        DEBUG_PUTS("_rx_start: RX failed");
    }
}

static bool _is_in_rx(void)
{
    uint8_t status = rf_cmd_prop_rx_adv.status;

    return (status == RFC_ACTIVE) || (status == RFC_PENDING);
}

static bool _is_in_tx(void)
{
    uint8_t status = rf_cmd_prop_tx_adv.status;

    return (status == RFC_ACTIVE) || (status == RFC_PENDING);
}

static int _send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev;
    DEBUG_PUTS("_send");

    if (_is_in_tx()) {
        DEBUG_PUTS("_send: already in TX");
        return -EAGAIN;
    }

    if (_is_in_rx()) {
        DEBUG_PUTS("_send: aborting");
        rfc_abort_command();
    }

    size_t len = 0;
    /* Reserve the first bytes of the PHR */
    uint8_t *bufpos = _tx_buf0 + 2;

    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        len += iol->iol_len;
        if (len > (TX_BUF_SIZE - 2)) {
            DEBUG_PUTS("_send: payload is too big!");
            return -EOVERFLOW;
        }

        memcpy(bufpos, iol->iol_base, iol->iol_len);
        bufpos += iol->iol_len;
    }

    /* Length in .15.4g PHY HDR. Includes the CRC but not the HDR itself */
    const uint16_t total_length = len + sizeof(uint16_t);
    /*
     * Prepare the .15.4g PHY header
     * MS=0, Length MSBits=0, DW and CRC configurable
     * Total length = len (payload) + CRC length
     *
     * The Radio will flip the bits around, so _tx_buf[0] must have the
     * length LSBs (PHR[15:8] and _tx_buf[1] will have PHR[7:0]
     */
    _tx_buf0[0] = ((total_length >> 0) & 0xFF);
    _tx_buf0[1] = ((total_length >> 8) & 0xFF) + 0x08 + 0x10;

    rf_cmd_prop_tx_adv.status = RFC_PENDING;
    rf_cmd_prop_tx_adv.pkt = _tx_buf0;
    rf_cmd_prop_tx_adv.pkt_len = len + 2;

    uint32_t cmdsta = rfc_send_command((rfc_op_t *)&rf_cmd_prop_tx_adv);

    if ((cmdsta & 0xFF) != RFC_CMDSTA_DONE) {
        DEBUG_PUTS("_send: TX send failed!");
        return -EIO;
    }

    return len;
}

static int _recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    (void)dev;

    rfc_data_entry_general_t *entry =
        (rfc_data_entry_general_t *)rfc_data_queue_available(&_rx_queue);

    if (!entry) {
        DEBUG_PUTS("_recv: no entry available");
        return 0;
    }

    uint8_t *payload = &entry->data;
    uint16_t payload_len = *(uint16_t *)payload;

    payload += sizeof(uint16_t);
    payload_len -= 2;

    /* Save RX information. */
    if (info) {
        netdev_ieee802154_rx_info_t *ieee_info = info;
        ieee_info->rssi = payload[payload_len];
        ieee_info->lqi = payload[payload_len + 1];
    }

    if (buf == NULL) {
        /* Without buf return only the length so it can be read later */
        if (len == 0) {
            return payload_len;
        }
        else {
            /* Discard entry */
            entry->status = RFC_DATA_ENTRY_PENDING;
            return 0;
        }
    }

    if (payload_len > len) {
        return -ENOSPC;
    }

    memcpy(buf, payload, payload_len);

    entry->status = RFC_DATA_ENTRY_PENDING;

    return payload_len;
}

static void _read_device_ieee_eui64(uint8_t *out)
{
    /*
     * The IEEE MAC address can be stored two places. We check the Customer
     * Configuration was not set before defaulting to the Factory
     * Configuration.
     */
    uint8_t *eui64 = (uint8_t *)&CCFG->IEEE_MAC_0;

    size_t i;
    for (i = 0; i < IEEE802154_LONG_ADDRESS_LEN; i++) {
        if (eui64[i] != 0xFF) {
            break;
        }
    }

    if (i >= IEEE802154_LONG_ADDRESS_LEN) {
        /* The ccfg address was all 0xFF, switch to the fcfg which always has
         * an address */
        eui64 = (uint8_t *)&FCFG->MAC_15_4_0;
    }

    /* The IEEE MAC address is stored in network byte order. */
    for (i = 0; i < IEEE802154_LONG_ADDRESS_LEN; i++) {
        out[i] = eui64[(IEEE802154_LONG_ADDRESS_LEN - 1) - i];
    }
}

static int _init(netdev_t *dev)
{
    cc26x2_cc13x2_rf_netdev_t *netdev = (cc26x2_cc13x2_rf_netdev_t *)dev;

    if (netdev == NULL) {
        DEBUG_PUTS("_init: netdev is null!");
        return -1;
    }

    /* Reset netdev_ieee802154 defaults */
    netdev_ieee802154_reset(&netdev->netdev);

    /* Set up EUI-64 address */
    _read_device_ieee_eui64(netdev->netdev.long_addr);
    memcpy(netdev->netdev.short_addr, netdev->netdev.long_addr + 6, 2);

    /* https://tools.ietf.org/html/rfc4944#section-12
     * Requires the first bit to 0 for unicast addresses */
    netdev->netdev.short_addr[1] &= 0x7F;

    /* Initialize data entries */
    rfc_data_entry_gen_init(_rx_buf0, sizeof(_rx_buf0), sizeof(uint16_t), _rx_buf1);
    rfc_data_entry_gen_init(_rx_buf1, sizeof(_rx_buf1), sizeof(uint16_t), _rx_buf2);
    rfc_data_entry_gen_init(_rx_buf2, sizeof(_rx_buf2), sizeof(uint16_t), _rx_buf3);
    rfc_data_entry_gen_init(_rx_buf3, sizeof(_rx_buf3), sizeof(uint16_t), _rx_buf0);

    /* Initialize RX data queue */
    rfc_data_queue_init(&_rx_queue, _rx_buf0);

    /* Tune the radio to the given frequency */
    rf_cmd_fs.frequency = 915;
    rf_cmd_fs.fract_freq = 0;

    /* Chain the CMD_FS command after running setup */
    rf_cmd_prop_radio_div_setup.condition.rule = RFC_COND_ALWAYS;
    rf_cmd_prop_radio_div_setup.condition.skip_no = 0;
    rf_cmd_prop_radio_div_setup.next_op = (rfc_op_t *)&rf_cmd_fs;

    /* Initialize radio driver in proprietary setup for the Sub-GHz band.
     * To use Sub-GHz we need to use CMD_PROP_RADIO_DIV_SETUP as our setup
     * command. */
    rfc_init((rfc_op_t *)&rf_cmd_prop_radio_div_setup, rf_patch_cpe_prop,
             _rfc_isr);

    if (rfc_enable() < 0) {
        DEBUG_PUTS("_init: rfc_enable failed!");
        return -1;
    }

    return 0;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    cc26x2_cc13x2_rf_netdev_t *dev = (cc26x2_cc13x2_rf_netdev_t *)netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_RX_END_IRQ:
            if (len != sizeof(bool)) {
                return -EINVAL;
            }
            else {
                if (*(const bool *)val) {
                    RFC_DBELL_NONBUF->RFCPEIEN |= CPE_IRQ_RX_ENTRY_DONE;
                }
                else {
                    RFC_DBELL_NONBUF->RFCPEIEN &= ~CPE_IRQ_RX_ENTRY_DONE;
                }
            }
            return sizeof(netopt_enable_t);

        case NETOPT_TX_END_IRQ:
            if (len != sizeof(bool)) {
                return -EINVAL;
            }
            else {
                _tx_end_irq = *(const bool *)val;
            }
            return sizeof(netopt_enable_t);

        default:
            res = -ENOTSUP;
            break;
    }

    if (res == -ENOTSUP) {
        res = netdev_ieee802154_set((netdev_ieee802154_t *)netdev,
                                    opt,
                                    val,
                                    len);
    }

    return res;
}


static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    cc26x2_cc13x2_rf_netdev_t *dev = (cc26x2_cc13x2_rf_netdev_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_RX_END_IRQ:
            if (max_len < sizeof(netopt_enable_t)) {
                return -EOVERFLOW;
            }
            else {
                if (RFC_DBELL->RFCPEIEN & CPE_IRQ_RX_ENTRY_DONE) {
                    *(netopt_enable_t *)val = NETOPT_ENABLE;
                }
                else {
                    *(netopt_enable_t *)val = NETOPT_DISABLE;
                }
            }
            return sizeof(netopt_enable_t);

        case NETOPT_TX_END_IRQ:
            if (max_len < sizeof(netopt_enable_t)) {
                return -EOVERFLOW;
            }
            else {
                if (_tx_end_irq) {
                    *(netopt_enable_t *)val = NETOPT_ENABLE;
                }
                else {
                    *(netopt_enable_t *)val = NETOPT_DISABLE;
                }
            }
            return sizeof(netopt_enable_t);

        default:
            break;
    }

    return netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt, val,
                                 max_len);
}

static void _isr(netdev_t *netdev)
{
    cc26x2_cc13x2_rf_netdev_t *dev = (cc26x2_cc13x2_rf_netdev_t *)netdev;

    if (dev->rx_complete) {
        dev->rx_complete = false;
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }

    if (dev->tx_complete) {
        dev->tx_complete = false;
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
}

void cc26x2_cc13x2_rf_setup(cc26x2_cc13x2_rf_netdev_t *dev)
{
    memset(dev, 0, sizeof(*dev));

    dev->rx_complete = false;
    dev->tx_complete = false;

    _netdev = dev;

    dev->netdev.netdev.driver = &cc26x2_cc13x2_rf_driver;
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
