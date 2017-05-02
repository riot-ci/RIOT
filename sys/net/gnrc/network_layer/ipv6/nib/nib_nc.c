/*
 * Copyright (C) 2017 Freie Universität Berlin
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

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"

#include "net/gnrc/ipv6/nib/nc.h"

#include "_nib-internal.h"

int gnrc_ipv6_nib_nc_set(const ipv6_addr_t *ipv6, unsigned iface,
                         const uint8_t *l2addr, size_t l2addr_len)
{
    _nib_t *nib;

    assert((ipv6 != NULL) && (l2addr != NULL));
    assert(l2addr < GNRC_IPV6_NIB_L2ADDR_MAX_LEN);
    assert(iface <= KERNEL_PID_LAST);
    mutex_lock(&_nib_mutex);
    nib = _nib_nc_add(ipv6, iface, GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED);
    if (nib == NULL) {
        mutex_unlock(&_nib_mutex);
        return -ENOMEM;
    }
#if GNRC_IPV6_NIB_CONF_ARSM
    memcpy(nib->l2addr, l2addr, l2addr_len);
    nib->l2addr_len = l2addr_len;
#else
    (void)l2addr;
    (void)l2addr_len;
#endif
    nib->info &= ~(GNRC_IPV6_NIB_NC_INFO_AR_STATE_MASK |
                   GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK);
    nib->info |= (GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL |
                  GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED);
    mutex_unlock(&_nib_mutex);
    return 0;
}

void gnrc_ipv6_nib_nc_del(const ipv6_addr_t *ipv6)
{
    _nib_t *nib = NULL;

    mutex_lock(&_nib_mutex);
    while ((nib = _nib_iter(nib)) != NULL) {
        if (ipv6_addr_equal(ipv6, &nib->ipv6)) {
            _nib_nc_remove(nib);
            break;
        }
    }
    mutex_unlock(&_nib_mutex);
}

void gnrc_ipv6_nib_nc_mark_reachable(const ipv6_addr_t *ipv6)
{
    _nib_t *nib = NULL;

    mutex_lock(&_nib_mutex);
    while ((nib = _nib_iter(nib)) != NULL) {
        if ((nib->mode & _NC) && ipv6_addr_equal(ipv6, &nib->ipv6)) {
            /* only set reachable if not unmanaged */
            if ((nib->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK)) {
                _nib_nc_set_reachable(nib);
            }
            break;
        }
    }
    mutex_unlock(&_nib_mutex);
}

bool gnrc_ipv6_nib_nc_iter(unsigned iface, void **state,
                           gnrc_ipv6_nib_nc_t *entry)
{
    _nib_t *nib = *state;

    mutex_lock(&_nib_mutex);
    while ((nib = _nib_iter(nib)) != NULL) {
        if ((nib->mode & _NC) &&
            ((iface == 0) || (_nib_get_if(nib) == iface))) {
            memcpy(&entry->ipv6, &nib->ipv6, sizeof(entry->ipv6));
#if gnrc_ipv6_nib_has_unique_id
            memcpy(&entry->unique_id, &nib->eui64, sizeof(entry->unique_id));
#endif
            entry->info = nib->info;
#if !gnrc_ipv6_nib_always_rev_trans
            memcpy(entry->l2addr, nib->l2addr, nib->l2addr_len);
            entry->l2addr_len = nib->l2addr_len;
#endif
            *state = nib;
            mutex_unlock(&_nib_mutex);
            return true;
        }
    }
    *state = NULL;
    mutex_unlock(&_nib_mutex);
    return false;
}

static const char *_nud_str[] = {
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED]     = "-",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE]   = "UNREACHABLE",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE]    = "INCOMPLETE",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE]         = "STALE",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY]         = "DELAY",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE]         = "PROBE",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE]     = "REACHABLE",
};

void gnrc_ipv6_nib_nc_print(gnrc_ipv6_nib_nc_t *entry)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    mutex_lock(&_nib_mutex);
    printf("%s ", ipv6_addr_to_str(addr_str, &entry->ipv6, sizeof(addr_str)));
    if (gnrc_ipv6_nib_nc_get_iface(entry) != KERNEL_PID_UNDEF) {
        printf("dev #%u ", gnrc_ipv6_nib_nc_get_iface(entry));
    }
    printf("lladdr %s ", gnrc_netif_addr_to_str(addr_str, sizeof(addr_str),
                                                entry->l2addr,
                                                entry->l2addr_len));
    if (gnrc_ipv6_nib_nc_is_router(entry)) {
        printf("router ");
    }
    puts(_nud_str[gnrc_ipv6_nib_nc_get_nud_state(entry)]);
    mutex_unlock(&_nib_mutex);
}

/** @} */
