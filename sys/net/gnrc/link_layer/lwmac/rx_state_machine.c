/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_lwmac
 * @{
 *
 * @file
 * @brief       Implementation of RX state machine of LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include "net/gnrc.h"
#include "net/gnrc/lwmac/lwmac.h"
#include "net/gnrc/mac/internal.h"
#include "net/gnrc/lwmac/timeout.h"
#include "include/rx_state_machine.h"
#include "include/lwmac_internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef LOG_LEVEL
/**
 * @brief Default log level define
 */
#define LOG_LEVEL LOG_WARNING
#endif
#include "log.h"

/**
 * @brief   Flag to track if the receiver has got a broadcast packet
 */
#define GNRC_LWMAC_RX_FOUND_BROADCAST         (0x01U)

/**
 * @brief   Flag to track if the receiver has got a WR packet
 */
#define GNRC_LWMAC_RX_FOUND_WR                (0x02U)

/**
 * @brief   Flag to track if the receiver has got a data packet
 */
#define GNRC_LWMAC_RX_FOUND_DATA              (0x04U)

static uint8_t _packet_process_in_wait_for_wr(gnrc_netdev_t *gnrc_netdev)
{
    uint8_t rx_info = 0;
    gnrc_pktsnip_t *pkt;

    assert(gnrc_netdev != NULL);

    while ((pkt = gnrc_priority_pktqueue_pop(&gnrc_netdev->rx.queue)) != NULL) {
        LOG(LOG_DEBUG, "[lwmac-rx] Inspecting pkt @ %p\n", pkt);

        /* Parse packet */
        lwmac_packet_info_t info;

        if (_parse_packet(pkt, &info) != 0) {
            LOG(LOG_DEBUG, "[lwmac-rx] Packet could not be parsed\n");
            gnrc_pktbuf_release(pkt);
            continue;
        }

        if (info.header->type == FRAMETYPE_BROADCAST) {
            _dispatch_defer(gnrc_netdev->rx.dispatch_buffer, pkt);
            gnrc_mac_dispatch(&gnrc_netdev->rx);
            rx_info |= GNRC_LWMAC_RX_FOUND_BROADCAST;
            /* quit listening period to avoid receiving duplicate broadcast packets */
            gnrc_netdev_lwmac_set_quit_rx(gnrc_netdev, true);
            /* quit TX in this cycle to avoid collisions with broadcast packets */
            gnrc_netdev_lwmac_set_quit_tx(gnrc_netdev, true);
            break;
        }

        /* TODO:
         * If we see a WA here we have a rough clue about the wakeup phase
         * of this node. But there is no timestamping of incoming frames yet
         * so maybe add a timestamp to every frame in event callback. */

        if (info.header->type != FRAMETYPE_WR) {
            LOG(LOG_DEBUG, "[lwmac-rx] Packet is not WR: 0x%02x\n", info.header->type);
            gnrc_pktbuf_release(pkt);
            continue;
        }

        /* No need to keep pkt anymore */
        gnrc_pktbuf_release(pkt);

        if (!(memcmp(&info.dst_addr.addr, &gnrc_netdev->l2_addr,
                     gnrc_netdev->l2_addr_len) == 0)) {
            LOG(LOG_DEBUG, "[lwmac-rx] Packet is WR but not for us\n");
            /* quit TX in this cycle to avoid collisions with other senders, since
             * found ongoing WR (preamble) stream*/
            gnrc_netdev_lwmac_set_quit_tx(gnrc_netdev, true);
            continue;
        }

        /* If reach here, the node gets a WR for itself. */
        /* Save source address for later addressing */
        gnrc_netdev->rx.l2_addr = info.src_addr;

        rx_info |= GNRC_LWMAC_RX_FOUND_WR;
        break;
    }

    return rx_info;
}

/* return false if send wa failed, otherwise return true */
static bool _send_wa(gnrc_netdev_t *gnrc_netdev)
{
    gnrc_pktsnip_t *pkt;
    gnrc_pktsnip_t *pkt_lwmac;
    gnrc_netif_hdr_t *nethdr_wa;

    assert(gnrc_netdev != NULL);
    assert(gnrc_netdev->rx.l2_addr.len != 0);

    /* if found ongoing transmission,
     * quit sending WA for collision avoidance. */
    if (_get_netdev_state(gnrc_netdev) == NETOPT_STATE_RX) {
        gnrc_netdev->rx.rx_bad_exten_count++;
        return false;
    }

    /* Assemble WA packet */
    lwmac_frame_wa_t lwmac_hdr;
    lwmac_hdr.header.type = FRAMETYPE_WA;
    lwmac_hdr.dst_addr = gnrc_netdev->rx.l2_addr;

    uint32_t phase_now = _phase_now();

    /* Embed the current 'relative phase timing' (counted from the start of this cycle)
     * of the receiver into its WA packet, thus to allow the sender to infer the
     * receiver's exact wake-up timing */
    if (phase_now > _ticks_to_phase(gnrc_netdev->lwmac.last_wakeup)) {
        lwmac_hdr.current_phase = (phase_now -
                                   _ticks_to_phase(gnrc_netdev->lwmac.last_wakeup));
    }
    else {
        lwmac_hdr.current_phase = (phase_now + RTT_US_TO_TICKS(LWMAC_WAKEUP_INTERVAL_US)) -
                                  _ticks_to_phase(gnrc_netdev->lwmac.last_wakeup);
    }

    pkt = gnrc_pktbuf_add(NULL, &lwmac_hdr, sizeof(lwmac_hdr), GNRC_NETTYPE_LWMAC);
    if (pkt == NULL) {
        LOG(LOG_ERROR, "ERROR: [lwmac-rx] Cannot allocate pktbuf of type GNRC_NETTYPE_LWMAC\n");
        gnrc_netdev_lwmac_set_quit_rx(gnrc_netdev, true);
        return false;
    }
    pkt_lwmac = pkt;

    pkt = gnrc_pktbuf_add(pkt, NULL,
                          sizeof(gnrc_netif_hdr_t) + gnrc_netdev->rx.l2_addr.len,
                          GNRC_NETTYPE_NETIF);
    if (pkt == NULL) {
        LOG(LOG_ERROR, "ERROR: [lwmac-rx] Cannot allocate pktbuf of type GNRC_NETTYPE_NETIF\n");
        gnrc_pktbuf_release(pkt_lwmac);
        gnrc_netdev_lwmac_set_quit_rx(gnrc_netdev, true);
        return false;
    }

    /* We wouldn't get here if add the NETIF header had failed, so no
       sanity checks needed */
    nethdr_wa = (gnrc_netif_hdr_t *)(gnrc_pktsnip_search_type(pkt,
                                                              GNRC_NETTYPE_NETIF)->data);
    /* Construct NETIF header and insert address for WA packet */
    gnrc_netif_hdr_init(nethdr_wa, 0, gnrc_netdev->rx.l2_addr.len);

    /* Send WA as broadcast*/
    nethdr_wa->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;

    /* Disable Auto ACK */
    netopt_enable_t autoack = NETOPT_DISABLE;
    gnrc_netdev->dev->driver->set(gnrc_netdev->dev, NETOPT_AUTOACK, &autoack,
                                  sizeof(autoack));

    /* Send WA */
    if (gnrc_netdev->send(gnrc_netdev, pkt) < 0) {
        LOG(LOG_ERROR, "ERROR: [lwmac-rx] Send WA failed.");
        if (pkt != NULL) {
            gnrc_pktbuf_release(pkt);
        }
        gnrc_netdev_lwmac_set_quit_rx(gnrc_netdev, true);
        return false;
    }

    /* Enable Auto ACK again for data reception */
    autoack = NETOPT_ENABLE;
    gnrc_netdev->dev->driver->set(gnrc_netdev->dev, NETOPT_AUTOACK, &autoack,
                                  sizeof(autoack));

    return true;
}

static uint8_t _packet_process_in_wait_for_data(gnrc_netdev_t *gnrc_netdev)
{
    uint8_t rx_info = 0;
    gnrc_pktsnip_t *pkt;

    assert(gnrc_netdev != NULL);

    pkt = NULL;

    while ((pkt = gnrc_priority_pktqueue_pop(&gnrc_netdev->rx.queue)) != NULL) {
        LOG(LOG_DEBUG, "[lwmac-rx] Inspecting pkt @ %p\n", pkt);

        /* Parse packet */
        lwmac_packet_info_t info;

        if (_parse_packet(pkt, &info) != 0) {
            LOG(LOG_DEBUG, "[lwmac-rx] Packet could not be parsed\n");
            gnrc_pktbuf_release(pkt);
            continue;
        }

        if (info.header->type == FRAMETYPE_BROADCAST) {
            _dispatch_defer(gnrc_netdev->rx.dispatch_buffer, pkt);
            gnrc_mac_dispatch(&gnrc_netdev->rx);
            /* quit listening period to avoid receiving duplicate broadcast packets */
            gnrc_netdev_lwmac_set_quit_rx(gnrc_netdev, true);
            continue;
        }

        if (!(memcmp(&info.src_addr.addr, &gnrc_netdev->rx.l2_addr.addr,
                     gnrc_netdev->rx.l2_addr.len) == 0)) {
            LOG(LOG_DEBUG, "[lwmac-rx] Packet is not from destination\n");
            gnrc_pktbuf_release(pkt);
            /* Reset timeout to wait for the data packet */
            lwmac_clear_timeout(gnrc_netdev, TIMEOUT_DATA);
            lwmac_set_timeout(gnrc_netdev, TIMEOUT_DATA, LWMAC_DATA_DELAY_US);
            continue;
        }

        if (!(memcmp(&info.dst_addr.addr, &gnrc_netdev->l2_addr,
                     gnrc_netdev->l2_addr_len) == 0)) {
            LOG(LOG_DEBUG, "[lwmac-rx] Packet is not for us\n");
            gnrc_pktbuf_release(pkt);
            /* Reset timeout to wait for the data packet */
            lwmac_clear_timeout(gnrc_netdev, TIMEOUT_DATA);
            lwmac_set_timeout(gnrc_netdev, TIMEOUT_DATA, LWMAC_DATA_DELAY_US);
            continue;
        }

        /* Sender maybe didn't get the WA */
        if (info.header->type == FRAMETYPE_WR) {
            LOG(LOG_DEBUG, "[lwmac-rx] Found a WR while waiting for DATA\n");
            lwmac_clear_timeout(gnrc_netdev, TIMEOUT_DATA);
            rx_info |= GNRC_LWMAC_RX_FOUND_WR;
            /* Push WR back to rx queue */
            gnrc_mac_queue_rx_packet(&gnrc_netdev->rx, 0, pkt);
            break;
        }

        switch (info.header->type) {
            case FRAMETYPE_DATA:
            case FRAMETYPE_DATA_PENDING: {
                /* Receiver gets the data packet */
                _dispatch_defer(gnrc_netdev->rx.dispatch_buffer, pkt);
                gnrc_mac_dispatch(&gnrc_netdev->rx);
                LOG(LOG_DEBUG, "[lwmac-rx] Found DATA!\n");
                lwmac_clear_timeout(gnrc_netdev, TIMEOUT_DATA);
                rx_info |= GNRC_LWMAC_RX_FOUND_DATA;
                return rx_info;
            }
            default: {
                gnrc_pktbuf_release(pkt);
            }
        }
    }

    return rx_info;
}

void lwmac_rx_start(gnrc_netdev_t *gnrc_netdev)
{
    if (gnrc_netdev == NULL) {
        return;
    }

    /* RX address should have been reset, probably not stopped then */
    assert(gnrc_netdev->rx.l2_addr.len == 0);

    /* Don't attempt to send a WA if channel is busy to get timings right */
    gnrc_netdev->mac_info &= ~GNRC_NETDEV_MAC_INFO_CSMA_ENABLED;
    netopt_enable_t csma_disable = NETOPT_DISABLE;
    gnrc_netdev->dev->driver->set(gnrc_netdev->dev, NETOPT_CSMA, &csma_disable,
                                  sizeof(csma_disable));

    gnrc_netdev->rx.state = RX_STATE_INIT;
}

void lwmac_rx_stop(gnrc_netdev_t *gnrc_netdev)
{
    if (!gnrc_netdev) {
        return;
    }

    lwmac_clear_timeout(gnrc_netdev, TIMEOUT_DATA);
    gnrc_netdev->rx.state = RX_STATE_STOPPED;
    gnrc_netdev->rx.l2_addr.len = 0;
}

/* Returns whether rescheduling is needed or not */
static bool _lwmac_rx_update(gnrc_netdev_t *gnrc_netdev)
{
    bool reschedule = false;

    if (!gnrc_netdev) {
        return reschedule;
    }

    switch (gnrc_netdev->rx.state) {
        case RX_STATE_INIT: {
            lwmac_clear_timeout(gnrc_netdev, TIMEOUT_DATA);
            gnrc_netdev->rx.state = RX_STATE_WAIT_FOR_WR;
            reschedule = true;
            break;
        }
        case RX_STATE_WAIT_FOR_WR: {
            LOG(LOG_DEBUG, "[lwmac-rx] RX_STATE_WAIT_FOR_WR\n");

            uint8_t rx_info = _packet_process_in_wait_for_wr(gnrc_netdev);

            /* if found broadcast packet, goto rx successful */
            if (rx_info & GNRC_LWMAC_RX_FOUND_BROADCAST) {
                gnrc_netdev->rx.state = RX_STATE_SUCCESSFUL;
                reschedule = true;
                break;
            }

            if (!(rx_info & GNRC_LWMAC_RX_FOUND_WR)) {
                LOG(LOG_DEBUG, "[lwmac-rx] No WR found, stop RX\n");
                gnrc_netdev->rx.rx_bad_exten_count++;
                gnrc_netdev->rx.state = RX_STATE_FAILED;
                reschedule = true;
                break;
            }

            /* TODO: don't flush queue */
            gnrc_priority_pktqueue_flush(&gnrc_netdev->rx.queue);
            /* Found WR packet (preamble), goto next state to send WA (preamble-ACK) */
            gnrc_netdev->rx.state = RX_STATE_SEND_WA;
            reschedule = true;
            break;
        }
        case RX_STATE_SEND_WA: {
            LOG(LOG_DEBUG, "[lwmac-rx] RX_STATE_SEND_WA\n");

            if (!_send_wa(gnrc_netdev)) {
                gnrc_netdev->rx.state = RX_STATE_FAILED;
                reschedule = true;
                break;
            }

            gnrc_netdev->rx.state = RX_STATE_WAIT_WA_SENT;
            reschedule = false;
            break;
        }
        case RX_STATE_WAIT_WA_SENT: {
            LOG(LOG_DEBUG, "[lwmac-rx] RX_STATE_WAIT_WA_SENT\n");

            if (gnrc_netdev_get_tx_feedback(gnrc_netdev) == TX_FEEDBACK_UNDEF) {
                LOG(LOG_DEBUG, "[lwmac-rx] WA not yet completely sent\n");
                break;
            }

            /* When reach here, WA has been sent, set timeout for expected data arrival */
            lwmac_set_timeout(gnrc_netdev, TIMEOUT_DATA, LWMAC_DATA_DELAY_US);

            _set_netdev_state(gnrc_netdev, NETOPT_STATE_IDLE);
            gnrc_netdev->rx.state = RX_STATE_WAIT_FOR_DATA;
            reschedule = false;
            break;
        }
        case RX_STATE_WAIT_FOR_DATA: {
            LOG(LOG_DEBUG, "[lwmac-rx] RX_STATE_WAIT_FOR_DATA\n");

            uint8_t rx_info = _packet_process_in_wait_for_data(gnrc_netdev);

            /* If WA got lost we wait for data but we will be hammered with WR
             * packets. So a WR indicates a lost WA => reset RX state machine.
             *
             * TODO: Destination may assume a wrong wakeup phase then. Maybe send a
             *       delta time to get the timing right again.
             */
            if (rx_info & GNRC_LWMAC_RX_FOUND_WR) {
                LOG(LOG_INFO, "[lwmac-rx] WA probably got lost, reset RX state machine\n");
                /* Start over again */
                gnrc_netdev->rx.state = RX_STATE_INIT;
                reschedule = true;
                break;
            }

            /* Only timeout if no packet (presumably the expected data) is being
             * received. This won't be blocked by WRs as they restart the state
             * machine (see above).
             *
             * TODO: Checking for expiration only works once and clears the timeout.
             *       If this is a false positive (other packet than DATA), we're
             *       stuck. */
            if ((lwmac_timeout_is_expired(gnrc_netdev, TIMEOUT_DATA)) &&
                (!gnrc_netdev_get_rx_started(gnrc_netdev))) {
                LOG(LOG_INFO, "[lwmac-rx] DATA timed out\n");
                gnrc_netdev->rx.rx_bad_exten_count++;
                gnrc_netdev->rx.state = RX_STATE_FAILED;
                reschedule = true;
                break;
            }

            if (!(rx_info & GNRC_LWMAC_RX_FOUND_DATA)) {
                LOG(LOG_DEBUG, "[lwmac-rx] No DATA yet\n");
                break;
            }

            gnrc_netdev->rx.state = RX_STATE_SUCCESSFUL;
            reschedule = true;
            break;
        }
        case RX_STATE_SUCCESSFUL:
        case RX_STATE_FAILED: {
            break;
        }
        case RX_STATE_STOPPED: {
            LOG(LOG_DEBUG, "[lwmac-rx] Reception state machine is stopped\n");
        }
    }
    return reschedule;
}

void lwmac_rx_update(gnrc_netdev_t *gnrc_netdev)
{
    /* Update until no rescheduling needed */
    while (_lwmac_rx_update(gnrc_netdev)) {}
}
