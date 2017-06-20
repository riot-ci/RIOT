/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_lwmac
 * @{
 *
 * @file
 * @brief       Implementation of internal functions of LWMAC
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include <stdbool.h>

#include "periph/rtt.h"
#include "net/gnrc.h"
#include "net/gnrc/mac/mac.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/lwmac/lwmac.h"
#include "include/lwmac_internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

uint32_t _phase_to_ticks(uint32_t phase)
{
    uint32_t rtt_now = rtt_get_counter();
    uint32_t phase_now = _ticks_to_phase(rtt_now);

    /* Start of current interval */
    rtt_now -= phase_now;

    /* Phase only in next interval */
    if (phase < phase_now) {
        rtt_now += RTT_US_TO_TICKS(LWMAC_WAKEUP_INTERVAL_US);
    }

    /* Advance to phase */
    return (rtt_now + phase);
}

gnrc_mac_tx_neighbor_t *_next_tx_neighbor(gnrc_netdev_t *gnrc_netdev)
{
    int next = -1;

    uint32_t phase_nearest = LWMAC_PHASE_MAX;

    for (int i = 0; i < GNRC_MAC_NEIGHBOR_COUNT; i++) {
        if (gnrc_priority_pktqueue_length(&gnrc_netdev->tx.neighbors[i].queue) > 0) {
            /* Unknown destinations are initialized with their phase at the end
             * of the local interval, so known destinations that still wakeup
             * in this interval will be preferred. */
            uint32_t phase_check = _ticks_until_phase(gnrc_netdev->tx.neighbors[i].phase);

            if (phase_check <= phase_nearest) {
                next = i;
                phase_nearest = phase_check;
                DEBUG("[LWMAC-int] Advancing queue #%d\n", i);
            }
        }
    }

    return (next < 0) ? NULL : &(gnrc_netdev->tx.neighbors[next]);
}

int _parse_packet(gnrc_pktsnip_t *pkt, lwmac_packet_info_t *info)
{
    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *lwmac_snip;
    lwmac_hdr_t *lwmac_hdr;

    assert(info != NULL);
    assert(pkt != NULL);

    netif_hdr = (gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF))->data;
    if (netif_hdr == NULL) {
        return -1;
    }

    /* Dissect LWMAC header, Every frame has header as first member */
    lwmac_hdr = (lwmac_hdr_t *) pkt->data;
    switch (lwmac_hdr->type) {
        case FRAMETYPE_WR: {
            lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(lwmac_frame_wr_t),
                                          GNRC_NETTYPE_LWMAC);
            break;
        }
        case FRAMETYPE_WA: {
            lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(lwmac_frame_wa_t),
                                          GNRC_NETTYPE_LWMAC);
            break;
        }
        case FRAMETYPE_DATA_PENDING:
        case FRAMETYPE_DATA: {
            lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(lwmac_frame_data_t),
                                          GNRC_NETTYPE_LWMAC);
            break;
        }
        case FRAMETYPE_BROADCAST: {
            lwmac_snip = gnrc_pktbuf_mark(pkt, sizeof(lwmac_frame_broadcast_t),
                                          GNRC_NETTYPE_LWMAC);
            break;
        }
        default: {
            return -2;
        }
    }

    /* Memory location may have changed while marking */
    lwmac_hdr = lwmac_snip->data;

    if (lwmac_hdr->type == FRAMETYPE_WA) {
        /* WA is broadcast, so get dst address out of header instead of netif */
        info->dst_addr = ((lwmac_frame_wa_t *)lwmac_hdr)->dst_addr;
    }
    else if (lwmac_hdr->type == FRAMETYPE_WR) {
        /* WR is broadcast, so get dst address out of header instead of netif */
        info->dst_addr = ((lwmac_frame_wr_t *)lwmac_hdr)->dst_addr;
    }
    else {
        if (netif_hdr->dst_l2addr_len) {
            info->dst_addr.len = netif_hdr->dst_l2addr_len;
            memcpy(info->dst_addr.addr,
                   gnrc_netif_hdr_get_dst_addr(netif_hdr),
                   netif_hdr->dst_l2addr_len);
        }
    }

    if (netif_hdr->src_l2addr_len) {
        info->src_addr.len = netif_hdr->src_l2addr_len;
        memcpy(info->src_addr.addr,
               gnrc_netif_hdr_get_src_addr(netif_hdr),
               netif_hdr->src_l2addr_len);
    }

    info->header = lwmac_hdr;
    return 0;
}

void _set_netdev_state(gnrc_netdev_t *gnrc_netdev, netopt_state_t devstate)
{
    gnrc_netdev->dev->driver->set(gnrc_netdev->dev,
                                  NETOPT_STATE,
                                  &devstate,
                                  sizeof(devstate));

#if (LWMAC_ENABLE_DUTYCYLE_RECORD == 1)
    if (devstate == NETOPT_STATE_IDLE) {
        if (!(gnrc_netdev->lwmac.lwmac_info & LWMAC_RADIO_IS_ON)) {
            gnrc_netdev->lwmac.last_radio_on_time_ticks = rtt_get_counter();
            gnrc_netdev->lwmac.lwmac_info |= LWMAC_RADIO_IS_ON;
        }
        return;
    }
    else if (devstate == NETOPT_STATE_SLEEP) {
        if (gnrc_netdev->lwmac.lwmac_info & LWMAC_RADIO_IS_ON) {
            gnrc_netdev->lwmac.radio_off_time_ticks = rtt_get_counter();

            gnrc_netdev->lwmac.awake_duration_sum_ticks +=
                (gnrc_netdev->lwmac.radio_off_time_ticks -
                 gnrc_netdev->lwmac.last_radio_on_time_ticks);

            gnrc_netdev->lwmac.lwmac_info &= ~LWMAC_RADIO_IS_ON;
        }
    }
#endif
}

netopt_state_t _get_netdev_state(gnrc_netdev_t *gnrc_netdev)
{
    netopt_state_t state;

    if (0 < gnrc_netdev->dev->driver->get(gnrc_netdev->dev,
                                          NETOPT_STATE,
                                          &state,
                                          sizeof(state))) {
        return state;
    }
    return -1;
}

uint32_t _next_inphase_event(uint32_t last, uint32_t interval)
{
    /* Counter did overflow since last wakeup */
    if (rtt_get_counter() < last) {
        /* TODO: Not sure if this was tested :) */
        uint32_t tmp = -last;
        tmp /= interval;
        tmp++;
        last += tmp * interval;
    }

    /* Add margin to next wakeup so that it will be at least 2ms in the future */
    while (last < (rtt_get_counter() + LWMAC_RTT_EVENT_MARGIN_TICKS)) {
        last += interval;
    }

    return last;
}

void lwmac_print_hdr(lwmac_hdr_t *hdr)
{
    assert(hdr != NULL);

    printf("LWMAC header:\n  Type: ");
    switch (hdr->type) {
        case FRAMETYPE_WR: {
            puts("Wakeup request (WR)");
            break;
        }
        case FRAMETYPE_WA: {
            puts("Wakeup acknowledge (WA)");
            printf("  Src addr:");
            lwmac_frame_wa_t *wa = (lwmac_frame_wa_t *) hdr;
            for (int i = 0; i < wa->dst_addr.len; i++) {
                printf("0x%02x", wa->dst_addr.addr[i]);
                if (i < (wa->dst_addr.len - 1)) {
                    printf(":");
                }
            }
            break;
        }
        case FRAMETYPE_DATA: {
            puts("User data");
            break;
        }
        case FRAMETYPE_BROADCAST: {
            puts("Broadcast user data");
            printf("  Sequence number: %d\n", ((lwmac_frame_broadcast_t *)hdr)->seq_nr);
            break;
        }
        default: {
            puts("Unkown type");
            printf("  Raw:  0x%02x\n", hdr->type);
        }
    }
}

int _dispatch_defer(gnrc_pktsnip_t *buffer[], gnrc_pktsnip_t *pkt)
{
    assert(buffer != NULL);
    assert(pkt != NULL);

    /* We care about speed here, so assume packet structure */
    assert(pkt->next->type == GNRC_NETTYPE_LWMAC);
    assert(pkt->next->next->type == GNRC_NETTYPE_NETIF);

    lwmac_frame_broadcast_t *bcast = NULL;
    if (((lwmac_hdr_t *)pkt->next->data)->type == FRAMETYPE_BROADCAST) {
        bcast = pkt->next->data;
    }

    for (unsigned i = 0; i < GNRC_MAC_DISPATCH_BUFFER_SIZE; i++) {
        /* Buffer will be filled bottom-up and emptied completely so no holes */
        if (buffer[i] == NULL) {
            buffer[i] = pkt;
            return 0;
        }
        else if (bcast &&
                (((lwmac_hdr_t *)buffer[i]->next->data)->type == FRAMETYPE_BROADCAST) &&
                (bcast->seq_nr == ((lwmac_frame_broadcast_t *)buffer[i]->next->data)->seq_nr)) {
                /* Filter same broadcasts, compare sequence number */
                gnrc_netif_hdr_t *hdr_queued, *hdr_new;
                hdr_new = pkt->next->next->data;
                hdr_queued = buffer[i]->next->next->data;

                /* Sequence numbers match, compare source addresses */
                if ((hdr_new->src_l2addr_len == hdr_queued->src_l2addr_len) &&
                    (memcmp(gnrc_netif_hdr_get_src_addr(hdr_new),
                            gnrc_netif_hdr_get_src_addr(hdr_queued),
                            hdr_new->src_l2addr_len) == 0)) {
                    /* Source addresses match, same packet */
                    DEBUG("[LWMAC] Found duplicate broadcast packet, dropping\n");
                    gnrc_pktbuf_release(pkt);
                    return -2;
                }
        }
    }

    DEBUG("[LWMAC] Dispatch buffer full, dropping packet\n");
    gnrc_pktbuf_release(pkt);

    return -1;
}
