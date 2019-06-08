/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <stdbool.h>

#include "byteorder.h"
#include "net/ipv6/ext/frag.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/ext.h"
#include "net/gnrc/ipv6/ext/frag.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pktbuf.h"
#include "random.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static gnrc_ipv6_ext_frag_send_t _snd_bufs[GNRC_IPV6_EXT_FRAG_SEND_SIZE];
/**
 * @todo    Implement better mechanism as described in
 *          https://tools.ietf.org/html/rfc7739 (for minimal approach
 *          destination cache is required)
 */
static uint32_t _last_id;

void gnrc_ipv6_ext_frag_init(void)
{
    _last_id = random_uint32();
}

/*
 * ==================
 * IPv6 fragmentation
 * ==================
 */
static gnrc_ipv6_ext_frag_send_t *_snd_buf_alloc(void);
static void _snd_buf_free(gnrc_ipv6_ext_frag_send_t *snd_buf);
static void _snd_buf_del(gnrc_ipv6_ext_frag_send_t *snd_buf);
static gnrc_pktsnip_t *_determine_last_per_frag(gnrc_pktsnip_t *pkt);

void gnrc_ipv6_ext_frag_send_pkt(gnrc_pktsnip_t *pkt, unsigned path_mtu)
{
    gnrc_ipv6_ext_frag_send_t *snd_buf = _snd_buf_alloc();
    gnrc_pktsnip_t *last_per_frag;

    assert(pkt->type == GNRC_NETTYPE_NETIF);
    if (snd_buf == NULL) {
        DEBUG("ipv6_ext_frag: can not allocate fragmentation send buffer\n");
        gnrc_pktbuf_release_error(pkt, ENOMEM);
        return;
    }
    last_per_frag = _determine_last_per_frag(pkt);
    snd_buf->per_frag = pkt;
    snd_buf->pkt = last_per_frag->next;
    /* separate per-fragment headers from rest */
    last_per_frag->next = NULL;
    snd_buf->id = _last_id;
    _last_id += random_uint32_range(1, 64);
    snd_buf->path_mtu = path_mtu;
    snd_buf->offset = 0;
    gnrc_ipv6_ext_frag_send(snd_buf);
}

void gnrc_ipv6_ext_frag_send(gnrc_ipv6_ext_frag_send_t *snd_buf)
{
    assert(snd_buf != NULL);
    gnrc_pktsnip_t *last = NULL, *ptr, *to_send = NULL;
    ipv6_ext_frag_t *frag_hdr;
    uint8_t *nh = NULL;
    network_uint16_t *len = NULL;
    msg_t msg;
    /* see if fragment to send fits into the path MTU */
    bool last_fragment = (snd_buf->path_mtu >
                          (gnrc_pkt_len(snd_buf->per_frag->next) +
                           sizeof(ipv6_ext_frag_t) +
                           gnrc_pkt_len(snd_buf->pkt)));
    uint16_t remaining = snd_buf->path_mtu & 0xfff8; /* lower multiple of 8 */

    ptr = snd_buf->per_frag;
    if (!last_fragment) {
        /* this won't be the last fragment
         * => we need to duplicate the per-fragment headers */
        gnrc_pktbuf_hold(ptr, 1);
    }
    else {
        /* prevent duplicate release of per_frag */
        snd_buf->per_frag = NULL;
    }
    /* first add per-fragment headers */
    while (ptr) {
        gnrc_pktsnip_t *tmp = gnrc_pktbuf_start_write(ptr);
        if (tmp == NULL) {
            DEBUG("ipv6_ext_frag: packet buffer full, canceling fragmentation\n");
            if (ptr->users > 1) {
                /* we are not the last fragment, so we need to also release
                 * our hold on the snips we did not duplicate so far
                 * and all also release all the snips we did duplicated so far
                 */
                if (to_send != NULL) {
                    gnrc_pktbuf_release(to_send);
                }
                else {
                    gnrc_pktbuf_release(ptr);
                }
            }
            _snd_buf_free(snd_buf);
            return;
        }
        ptr = tmp;
        if (to_send == NULL) {
            to_send = ptr;
        }
        switch (ptr->type) {
            case GNRC_NETTYPE_IPV6: {
                ipv6_hdr_t *hdr = ptr->data;
                nh = &hdr->nh;
                len = &hdr->len;
                break;
            }
            case GNRC_NETTYPE_IPV6_EXT: {
                ipv6_ext_t *hdr = ptr->data;
                nh = &hdr->nh;
                break;
            }
            default:
                break;
        }
        if (ptr->type != GNRC_NETTYPE_NETIF) {
            remaining -= ptr->size;
        }
        if (last) {
            last->next = ptr;
        }
        last = ptr;
        ptr = ptr->next;
    }
    assert(nh != NULL);
    /* then the fragment header */
    ptr = gnrc_ipv6_ext_build(last, last->next, *nh, sizeof(ipv6_ext_frag_t));
    if (ptr == NULL) {
        DEBUG("ipv6_ext_frag: unable to create fragmentation header\n");
        gnrc_pktbuf_release(to_send);
        _snd_buf_free(snd_buf);
        return;
    }
    remaining -= sizeof(ipv6_ext_frag_t);
    frag_hdr = ptr->data;
    ipv6_ext_frag_set_offset(frag_hdr, snd_buf->offset);
    if (!last_fragment) {
        ipv6_ext_frag_set_more(frag_hdr);
    }
    frag_hdr->id = byteorder_htonl(snd_buf->id);
    *nh = PROTNUM_IPV6_EXT_FRAG;
    last = ptr;
    /* then the rest */
    while (remaining && snd_buf->pkt) {
        if (last_fragment ||
            (snd_buf->pkt->size <= remaining)) {
            ptr = snd_buf->pkt;
            snd_buf->pkt = ptr->next;
            ptr->next = NULL;
        }
        else {
            ptr = gnrc_pktbuf_mark(snd_buf->pkt, remaining,
                                   GNRC_NETTYPE_UNDEF);
            if (ptr == NULL) {
                DEBUG("ipv6_ext_frag: packet buffer full, canceling fragmentation\n");
                gnrc_pktbuf_release(to_send);
                _snd_buf_free(snd_buf);
                return;
            }
            assert(snd_buf->pkt->next == ptr);  /* we just created it with mark */
            snd_buf->pkt->next = snd_buf->pkt->next->next;
            ptr->next = NULL;
        }
        last->next = ptr;
        last = ptr;
        remaining -= ptr->size;
        snd_buf->offset += ptr->size;
    }
    assert(len != NULL);
    /* adapt IPv6 header length field */
    *len = byteorder_htons(gnrc_pkt_len(to_send->next->next));
    msg.type = GNRC_IPV6_EXT_FRAG_SEND_FRAG;
    msg.content.ptr = to_send;
    msg_try_send(&msg, gnrc_ipv6_pid);
    if (last_fragment) {
        _snd_buf_del(snd_buf);
    }
    else {
        msg.type = GNRC_IPV6_EXT_FRAG_SEND;
        msg.content.ptr = snd_buf;
        msg_try_send(&msg, gnrc_ipv6_pid);
    }
}

static gnrc_ipv6_ext_frag_send_t *_snd_buf_alloc(void)
{
    for (unsigned i = 0; i < GNRC_IPV6_EXT_FRAG_SEND_SIZE; i++) {
        gnrc_ipv6_ext_frag_send_t *snd_buf = &_snd_bufs[i];
        if (snd_buf->pkt == NULL) {
            return snd_buf;
        }
    }
    return NULL;
}

static void _snd_buf_del(gnrc_ipv6_ext_frag_send_t *snd_buf)
{
    snd_buf->per_frag = NULL;
    snd_buf->pkt = NULL;
}

static void _snd_buf_free(gnrc_ipv6_ext_frag_send_t *snd_buf)
{
    if (snd_buf->per_frag) {
        gnrc_pktbuf_release(snd_buf->per_frag);
    }
    if (snd_buf->pkt) {
        gnrc_pktbuf_release(snd_buf->pkt);
    }
    _snd_buf_del(snd_buf);
}

static gnrc_pktsnip_t *_determine_last_per_frag(gnrc_pktsnip_t *ptr)
{
    gnrc_pktsnip_t *last_per_frag = NULL;
    unsigned nh = PROTNUM_RESERVED;

    /* ignore NETIF header */
    ptr = ptr->next;
    while (ptr) {
        switch (ptr->type) {
            case GNRC_NETTYPE_IPV6: {
                ipv6_hdr_t *hdr = ptr->data;
                last_per_frag = ptr;
                nh = hdr->nh;
                break;
            }
            case GNRC_NETTYPE_IPV6_EXT: {
                ipv6_ext_t *hdr = ptr->data;
                switch (nh) {
                    /* "[...] that is, all headers up to and including the
                     * Routing header if present, else the Hop-by-Hop Options
                     * header if present, [...]"
                     * (IPv6 header comes before Hop-by-Hop Options comes before
                     * Routing header, so an override to keep the quoted
                     * priorities is ensured) */
                    case PROTNUM_IPV6_EXT_HOPOPT:
                    case PROTNUM_IPV6_EXT_RH:
                        last_per_frag = ptr;
                        break;
                    default:
                        break;
                }
                nh = hdr->nh;
                break;
            }
            default:
                assert(last_per_frag != NULL);
                return last_per_frag;
        }
        ptr = ptr->next;
    }
    /* should not be reached */
    assert(false);
    return NULL;
}

/** @} */
