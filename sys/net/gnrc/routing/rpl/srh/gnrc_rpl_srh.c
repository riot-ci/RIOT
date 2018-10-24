/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Cenk Gündoğan <cnkgndgn@gmail.com>
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <string.h>
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/ipv6/ext/rh.h"
#include "net/gnrc/rpl/srh.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

#define GNRC_RPL_SRH_PADDING(X)     ((X & 0xF0) >> 4)
#define GNRC_RPL_SRH_COMPRE(X)      (X & 0x0F)
#define GNRC_RPL_SRH_COMPRI(X)      ((X & 0xF0) >> 4)

int gnrc_rpl_srh_process(ipv6_hdr_t *ipv6, gnrc_rpl_srh_t *rh)
{
    ipv6_addr_t addr, tmp;
    uint8_t *addr_vec = (uint8_t *) (rh + 1);
    bool found = false;
    uint8_t n;
    uint8_t i, pref_elided, tmp_pref_elided, addr_len, compri_addr_len, tmp_addr_len, found_pos = 0;
    const uint8_t new_seg_left = rh->seg_left - 1;

    assert(rh->seg_left > 0);
    n = (((rh->len * 8) - GNRC_RPL_SRH_PADDING(rh->pad_resv) -
        (16 - GNRC_RPL_SRH_COMPRE(rh->compr))) /
        (16 - GNRC_RPL_SRH_COMPRI(rh->compr))) + 1;

    DEBUG("RPL SRH: %u addresses in the routing header\n", (unsigned) n);

    if (rh->seg_left > n) {
        DEBUG("RPL SRH: number of segments left > number of addresses - discard\n");
        /* TODO ICMP Parameter Problem - Code 0 */
        return GNRC_IPV6_EXT_RH_ERROR;
    }

    i = n - new_seg_left;
    pref_elided = new_seg_left ? GNRC_RPL_SRH_COMPRI(rh->compr) : GNRC_RPL_SRH_COMPRE(rh->compr);
    compri_addr_len = sizeof(ipv6_addr_t) - GNRC_RPL_SRH_COMPRI(rh->compr);
    addr_len = sizeof(ipv6_addr_t) - pref_elided;
    memcpy(&addr, &ipv6->dst, pref_elided);
    memcpy(&addr.u8[pref_elided], &addr_vec[(i - 1) * compri_addr_len], addr_len);

    if (ipv6_addr_is_multicast(&ipv6->dst) || ipv6_addr_is_multicast(&addr)) {
        DEBUG("RPL SRH: found a multicast address - discard\n");
        /* TODO discard the packet */
        return GNRC_IPV6_EXT_RH_ERROR;
    }

    /* check if multiple addresses of my interface exist */
    tmp_pref_elided = GNRC_RPL_SRH_COMPRI(rh->compr);
    tmp_addr_len = sizeof(ipv6_addr_t) - tmp_pref_elided;
    memcpy(&tmp, &ipv6->dst, tmp_pref_elided);
    for (uint8_t k = 0; k < n; k++) {
        if (k == n - 1) {
            tmp_pref_elided = GNRC_RPL_SRH_COMPRE(rh->compr);
            tmp_addr_len = sizeof(ipv6_addr_t) - tmp_pref_elided;
        }
        memcpy(&tmp.u8[tmp_pref_elided], &addr_vec[k * compri_addr_len], tmp_addr_len);
        if (gnrc_netif_get_by_ipv6_addr(&tmp) != NULL) {
            if (found && ((k - found_pos) > 1)) {
                DEBUG("RPL SRH: found multiple addresses that belong to me - discard\n");
                /* TODO send an ICMP Parameter Problem (Code 0) and discard the packet */
                return GNRC_IPV6_EXT_RH_ERROR;
            }
            found_pos = k;
            found = true;
        }
    }
    rh->seg_left = new_seg_left;
    memcpy(&addr_vec[(i - 1) * compri_addr_len], &ipv6->dst.u8[pref_elided], addr_len);

    DEBUG("RPL SRH: Next hop: %s at position %d\n",
          ipv6_addr_to_str(addr_str, &addr, sizeof(addr_str)), i);

    memcpy(&ipv6->dst, &addr, sizeof(ipv6->dst));

    return GNRC_IPV6_EXT_RH_FORWARDED;
}

/** @} */
