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

#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "cpu.h"

/**
 * @brief    Align a buffer size to a 4-byte boundary
 */
#define ALIGN_TO_4(size) (((size) + 3) & ~3)

#define TX_BUF_SIZE  ALIGN_TO_4(IEEE802154_PHY_MR_FSK_PHR_LEN + IEEE802154G_FRAME_LEN_MAX)
#define RX_BUF_SIZE  ALIGN_TO_4(IEEE802154G_FRAME_LEN_MAX + sizeof(rfc_data_entry_t))
#define RX_BUF_NUMOF (CONFIG_CC26x2_CC13X2_RF_RX_BUF_NUMOF)

/**
 * @brief   TX buffer
 */
static uint8_t _tx_buf[TX_BUF_SIZE];
/**
 * @brief   RX buffers
 */
static uint8_t _rx_buf[RX_BUF_NUMOF][RX_BUF_SIZE];
/**
 * @brief   RX queue
 */
static rfc_data_queue_t _rx_queue;
/**
 * @brief   RAT timer offset
 */
static rfc_ratmr_t _rat_offset;
/**
 * @brief   Netdev pointer to use in IRQs
 */
static cc26x2_cc13x2_rf_netdev_t *_netdev;

/**
 * @brief   Is NETOPT_TX_END_IRQ enabled?
 */
static bool _tx_end_irq;

static mutex_t _last_cmd = MUTEX_INIT;

static void _rx_start(void);

static void _rfc_isr(void)
{
    /* Check if CPE_IRQ_RX_ENTRY_DONE is enabled and the flag is present */
    if ((RFC_DBELL->RFCPEIFG & CPE_IRQ_RX_ENTRY_DONE) &&
        (RFC_DBELL->RFCPEIEN & CPE_IRQ_RX_ENTRY_DONE)) {
        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_RX_ENTRY_DONE;
        _netdev->rx_events++;
        netdev_trigger_event_isr((netdev_t *)_netdev);
    }

    if ((RFC_DBELL->RFCPEIFG & CPE_IRQ_LAST_COMMAND_DONE) &&
        (RFC_DBELL->RFCPEIEN & CPE_IRQ_LAST_COMMAND_DONE)) {
        RFC_DBELL_NONBUF->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;
        mutex_unlock(&_last_cmd);
    }
}

static void _rx_start(void)
{
    DEBUG_PUTS("_rx_start()");
    uint32_t cmdsta;

    /* Clear queue */
    if (rf_cmd_prop_rx_adv.status == RFC_PROP_ERROR_RXBUF &&
        _netdev->rx_events == 0) {
        rf_cmd_clear_rx.queue = &_rx_queue;
        cmdsta = cc26x2_cc13x2_rfc_send_cmd((rfc_op_t *)&rf_cmd_clear_rx);
        if ((cmdsta & 0xff) != RFC_CMDSTA_DONE) {
            DEBUG_PUTS("_rx_start: CLEAR_RX failed");
        }
    }

    /* Start RX */
    rf_cmd_prop_rx_adv.status = RFC_IDLE;
    cmdsta = cc26x2_cc13x2_rfc_send_cmd((rfc_op_t *)&rf_cmd_prop_rx_adv);
    if ((cmdsta & 0xff) != RFC_CMDSTA_DONE) {
        if ((cmdsta & 0xFF) == RFC_CMDSTA_SCHEDULINGERROR) {
            cc26x2_cc13x2_rfc_abort_cmd();
            mutex_lock(&_last_cmd);
        }

        cmdsta = cc26x2_cc13x2_rfc_send_cmd((rfc_op_t *)&rf_cmd_prop_rx_adv);
        if ((cmdsta & 0xff) != RFC_CMDSTA_DONE) {
            DEBUG_PUTS("_rx_start: RX failed");
        }
    }
}

static int _send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev;

    if (rf_cmd_prop_rx_adv.status == RFC_PENDING ||
        rf_cmd_prop_rx_adv.status == RFC_ACTIVE) {
        cc26x2_cc13x2_rfc_abort_cmd();
        mutex_lock(&_last_cmd);
    }

    size_t len = 0;
    /* Reserve the first bytes of the PHR */
    uint8_t *bufpos = _tx_buf + IEEE802154_PHY_MR_FSK_PHR_LEN;

    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        len += iol->iol_len;
        if (len > (TX_BUF_SIZE - IEEE802154_PHY_MR_FSK_PHR_LEN)) {
            DEBUG_PUTS("_send: payload is too big!");
            return -EOVERFLOW;
        }

        memcpy(bufpos, iol->iol_base, iol->iol_len);
        bufpos += iol->iol_len;
    }

    /* Length in .15.4g PHY HDR. Includes the CRC but not the HDR itself */
    const uint16_t total_length = len + sizeof(uint16_t);

    if (total_length > IEEE802154G_FRAME_LEN_MAX) {
        return -EOVERFLOW;
    }

    /*
     * Prepare the .15.4g PHY header
     * MS=0, Length MSBits=0, DW and CRC configurable
     * Total length = len (payload) + CRC length
     *
     * The Radio will flip the bits around, so _tx_buf[0] must have the
     * length LSBs (PHR[15:8] and _tx_buf[1] will have PHR[7:0]
     */
    _tx_buf[0] = ((total_length >> 0) & 0xFF);
    _tx_buf[1] = ((total_length >> 8) & 0xFF) + 0x08 + 0x10;

    rf_cmd_prop_tx_adv.status = RFC_IDLE;
    rf_cmd_prop_tx_adv.pkt = _tx_buf;
    rf_cmd_prop_tx_adv.pkt_len = IEEE802154_PHY_MR_FSK_PHR_LEN + len;

    uint32_t cmdsta = cc26x2_cc13x2_rfc_send_cmd((rfc_op_t *)&rf_cmd_prop_tx_adv);
    if ((cmdsta & 0xFF) != RFC_CMDSTA_DONE) {
        if ((cmdsta & 0xFF) == RFC_CMDSTA_SCHEDULINGERROR) {
            cc26x2_cc13x2_rfc_abort_cmd();
            mutex_lock(&_last_cmd);
        }

        cmdsta = cc26x2_cc13x2_rfc_send_cmd((rfc_op_t *)&rf_cmd_prop_tx_adv);
        if ((cmdsta & 0xFF) != RFC_CMDSTA_DONE) {
            DEBUG_PUTS("_send: TX send failed!");
            rf_cmd_prop_tx_adv.status = RFC_IDLE;
            return -EIO;
        }
    }
    mutex_lock(&_last_cmd);

    /* Restart RX */
    _rx_start();

    /* Inform TX ended if we were transmitting */
    if (_tx_end_irq) {
        _netdev->tx_events++;
        netdev_trigger_event_isr((netdev_t *)_netdev);
    }

    return len;
}

static int _recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    (void)dev;

    unsigned key = irq_disable();

    rfc_data_entry_general_t *entry =
        (rfc_data_entry_general_t *)cc26x2_cc13x2_rfc_queue_recv(&_rx_queue);

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
        ieee_info->rssi = (int8_t)payload[payload_len];
        ieee_info->lqi = payload[payload_len + 1];
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
            if (payload_len == 0) {
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

    if (payload_len > len) {
        entry->status = RFC_DATA_ENTRY_PENDING;
        irq_restore(key);
        return -ENOSPC;
    }

    memcpy(buf, payload, payload_len);

    entry->status = RFC_DATA_ENTRY_PENDING;

    irq_restore(key);
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

    /* Initialize data entries as general, with a length field of 2 bytes
     * (uint16_t) and the correct RX buffer size */
    for (unsigned i = 0; i < RX_BUF_NUMOF; i++) {
        rfc_data_entry_t *entry = (rfc_data_entry_t *)_rx_buf[i];

        entry->status = RFC_DATA_ENTRY_PENDING;
        entry->config.type = RFC_DATA_ENTRY_TYPE_GEN;
        entry->config.lensz = sizeof(uint16_t);
        entry->length = RX_BUF_SIZE - sizeof(rfc_data_entry_t);

        if (i + 1 == RX_BUF_NUMOF) {
            /* Point to the first entry if this is the last */
            entry->next_entry = _rx_buf[0];
        }
        else {
            entry->next_entry = _rx_buf[i + 1];
        }
    }

    /* Initialize RX data queue */
    _rx_queue.curr_entry = _rx_buf[0];
    _rx_queue.last_entry = NULL;

    /* Use _rx_queue in RX command */
    rf_cmd_prop_rx_adv.queue = &_rx_queue;

    /* Tune the radio to the given frequency */
    rf_cmd_fs.frequency = 915;
    rf_cmd_fs.fract_freq = 0;

    /* Chain the CMD_FS command after running setup */
    rf_cmd_prop_radio_div_setup.condition.rule = RFC_COND_ALWAYS;
    rf_cmd_prop_radio_div_setup.condition.skip_no = 0;
    rf_cmd_prop_radio_div_setup.next_op = (rfc_op_t *)&rf_cmd_fs;

    /* Maximum TX power by default */
    cc26x2_cc13x2_rf_pa_t *tx_power = &cc26x2_cc13x2_rf_patable[0];
    rf_cmd_prop_radio_div_setup.tx_power = tx_power->val;

    /* Initialize radio driver in proprietary setup for the Sub-GHz band.
     * To use Sub-GHz we need to use CMD_PROP_RADIO_DIV_SETUP as our setup
     * command. */
    cc26x2_cc13x2_rfc_init(rf_patch_cpe_prop, _rfc_isr);

    if (cc26x2_cc13x2_rfc_power_on() < 0) {
        DEBUG_PUTS("_init: cc26x2_cc13x2_rfc_power_on failed!");
        return -1;
    }

    RFC_DBELL_NONBUF->RFCPEIEN |= CPE_IRQ_LAST_COMMAND_DONE;

    /* Run radio setup command after we start the RAT */
    rf_cmd_sync_start_rat.next_op = &rf_cmd_prop_radio_div_setup;
    rf_cmd_sync_start_rat.condition.rule = RFC_COND_ALWAYS;
    rf_cmd_sync_start_rat.rat0 = _rat_offset;

    uint32_t cmdsta = cc26x2_cc13x2_rfc_send_cmd((rfc_op_t *)&rf_cmd_sync_start_rat);
    if (cmdsta != RFC_CMDSTA_DONE) {
        DEBUG("rfc: radio setup failed! CMDSTA = %lx\n", cmdsta);
        return -1;
    }
    mutex_lock(&_last_cmd);

    _rx_start();

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

        case NETOPT_TX_END_IRQ:
            assert(max_len >= sizeof(netopt_enable_t));
            if (max_len < sizeof(netopt_enable_t)) {
                return -EOVERFLOW;
            }
            else {
                *(netopt_enable_t *)val = _tx_end_irq ? NETOPT_ENABLE :
                                                        NETOPT_DISABLE;
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

    while (dev->rx_events) {
        dev->rx_events--;
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }

    while (dev->tx_events) {
        dev->tx_events--;
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
}

void cc26x2_cc13x2_rf_setup(cc26x2_cc13x2_rf_netdev_t *dev)
{
    memset(dev, 0, sizeof(*dev));

    dev->rx_events = 0;
    dev->tx_events = 0;

    _netdev = dev;

    mutex_lock(&_last_cmd);

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
