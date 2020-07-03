/*
 * Copyright (C) Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Neighbor level stats for netdev
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 * @}
 */

#include <errno.h>

#include "net/netdev.h"
#include "net/netstats/neighbor.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static bool l2_addr_equal(const uint8_t *a, uint8_t a_len, const uint8_t *b, uint8_t b_len)
{
    if (a_len != b_len) {
        return false;
    }

    return memcmp(a, b, a_len) == 0;
}

static void netstats_nb_half_freshness(netstats_nb_t *stats, timex_t *cur)
{
    uint16_t now = cur->seconds;
    uint8_t diff = (now - stats->last_halved) / NETSTATS_NB_FRESHNESS_HALF;
    stats->freshness >>= diff;

    if (diff) {
        /* Set to the last time point where this should have been halved */
        stats->last_halved = now - diff;
    }
}

static void netstats_nb_incr_freshness(netstats_nb_t *stats)
{
    timex_t cur;
    xtimer_now_timex(&cur);

    /* First halve the freshness if applicable */
    netstats_nb_half_freshness(stats, &cur);

    /* Increment the freshness capped at FRESHNESS_MAX */
    if (stats->freshness < NETSTATS_NB_FRESHNESS_MAX) {
        stats->freshness++;
    }

    stats->last_updated = cur.seconds;
}

bool netstats_nb_isfresh(netstats_nb_t *stats)
{
    timex_t cur;
    xtimer_now_timex(&cur);
    uint16_t now = cur.seconds;

    /* Half freshness if applicable to update to current freshness */
    netstats_nb_half_freshness(stats, &cur);

    return (stats->freshness >= NETSTATS_NB_FRESHNESS_TARGET) &&
           (now - stats->last_updated < NETSTATS_NB_FRESHNESS_EXPIRATION);
}

void netstats_nb_init(netif_t *dev)
{
    memset(dev->pstats, 0, sizeof(netstats_nb_t) * NETSTATS_NB_SIZE);
    cib_init(&dev->stats_idx, NETSTATS_NB_QUEUE_SIZE);
}

static void netstats_nb_create(netstats_nb_t *entry, const uint8_t *l2_addr, uint8_t l2_len)
{
    memset(entry, 0, sizeof(netstats_nb_t));
    memcpy(entry->l2_addr, l2_addr, l2_len);
    entry->l2_addr_len = l2_len;
    entry->etx = NETSTATS_NB_ETX_INIT * NETSTATS_NB_ETX_DIVISOR;
}

netstats_nb_t *netstats_nb_get(netif_t *dev, const uint8_t *l2_addr, uint8_t len)
{
    netstats_nb_t *stats = dev->pstats;

    for (int i = 0; i < NETSTATS_NB_SIZE; i++) {

        /* Check if this is the matching entry */
        if (l2_addr_equal(stats[i].l2_addr, stats[i].l2_addr_len, l2_addr, len)) {
            return &stats[i];
        }
    }

    return NULL;
}

/* find the oldest inactive entry to replace. Empty entries are infinity old */
static netstats_nb_t *netstats_nb_get_or_create(netif_t *dev, const uint8_t *l2_addr, uint8_t len)
{
    netstats_nb_t *old_entry = NULL;
    netstats_nb_t *stats = dev->pstats;

    timex_t cur;
    xtimer_now_timex(&cur);

    for (int i = 0; i < NETSTATS_NB_SIZE; i++) {

        /* Check if this is the matching entry */
        if (l2_addr_equal(stats[i].l2_addr, stats[i].l2_addr_len, l2_addr, len)) {
            return &stats[i];
        }

        /* Entry is oldest if it is empty */
        if (stats[i].l2_addr_len == 0) {
            old_entry = &stats[i];
            continue;
        }

        /* Check if the entry is expired */
        if ((netstats_nb_isfresh(&stats[i]))) {
            continue;
        }

        /* Entry is oldest if it is expired */
        if (old_entry == NULL) {
            old_entry = &stats[i];
            continue;
        }

        /* don't replace old entry if there are still empty ones */
        if (old_entry->l2_addr_len == 0) {
            continue;
        }

        /* Check if current entry is older than current oldest entry */
        old_entry = netstats_nb_comp(old_entry, &stats[i], cur.seconds);
    }

    /* if there is no matching entry,
     * create a new entry if we have an expired one */
    if (old_entry) {
        netstats_nb_create(old_entry, l2_addr, len);
    }

    return old_entry;
}

netstats_nb_t *netstats_nb_get_next(netstats_nb_t *first, netstats_nb_t *current)
{
    netstats_nb_t *last = first + NETSTATS_NB_SIZE;

    for (++current; current < last; ++current) {
        if (current->l2_addr_len) {
            return current;
        }
    }

    return NULL;
}

void netstats_nb_record(netif_t *dev, const uint8_t *l2_addr, uint8_t len)
{
    int idx = cib_put(&dev->stats_idx);

    if (idx < 0) {
        return;
    }

    if (len == 0) {
        /* Fill queue with a NOP */
        dev->stats_queue[idx] = NULL;
    } else {
        dev->stats_queue[idx] = netstats_nb_get_or_create(dev, l2_addr, len);
        dev->stats_queue[idx]->time_tx = xtimer_now_usec();
    }
}

/* Get the first available neighbor in the transmission queue
 * and increment pointer. */
static netstats_nb_t *netstats_nb_get_recorded(netif_t *dev)
{
    int idx = cib_get(&dev->stats_idx);

    if (idx < 0) {
        return NULL;
    }

    return dev->stats_queue[idx];
}

static uint32_t _ewma(bool fresh, uint32_t old_val, uint32_t new_val)
{
    uint8_t ewma_alpha;

    /* If the stats are not fresh, use a larger alpha to average aggressive */
    if (fresh) {
        ewma_alpha = NETSTATS_NB_EWMA_ALPHA;
    } else {
        ewma_alpha = NETSTATS_NB_EWMA_ALPHA_RAMP;
    }

    /* Exponential weighted moving average */
    return (old_val * (NETSTATS_NB_EWMA_SCALE - ewma_alpha)
         +  new_val * ewma_alpha) / NETSTATS_NB_EWMA_SCALE;
}

static void netstats_nb_update_etx(netstats_nb_t *stats, netstats_nb_result_t result, uint8_t transmissions)
{
    if (result != NETSTATS_NB_SUCCESS) {
        transmissions = NETSTATS_NB_ETX_NOACK_PENALTY;
    }

    stats->etx = _ewma(netstats_nb_isfresh(stats), stats->etx, transmissions * NETSTATS_NB_ETX_DIVISOR);
}

static void netstats_nb_update_time(netstats_nb_t *stats, netstats_nb_result_t result)
{
    uint32_t duration = xtimer_now_usec() - stats->time_tx;

    if (result != NETSTATS_NB_SUCCESS) {
        duration *= 2;
    }

    if (stats->time_tx_avg == 0) {
        stats->time_tx_avg = duration;
    } else {
        stats->time_tx_avg = _ewma(netstats_nb_isfresh(stats), stats->time_tx_avg, duration);
    }
}

netstats_nb_t *netstats_nb_update_tx(netif_t *dev, netstats_nb_result_t result, uint8_t transmissions)
{
    netstats_nb_t *stats = netstats_nb_get_recorded(dev);

    if (result == NETSTATS_NB_BUSY || stats == NULL) {
        return stats;
    }

    netstats_nb_update_time(stats, result);

    if (transmissions) {
        netstats_nb_update_etx(stats, result, transmissions);
    }
    netstats_nb_incr_freshness(stats);
    stats->tx_count++;

    return stats;
}

#ifdef MODULE_NETSTATS_NEIGHBOR_EXT
netstats_nb_t *netstats_nb_update_rx(netif_t *dev, const uint8_t *l2_addr,
                                     uint8_t l2_addr_len, uint8_t rssi, uint8_t lqi)
{
    netstats_nb_t *stats = netstats_nb_get_or_create(dev, l2_addr, l2_addr_len);

    if (stats == NULL) {
        return NULL;
    }

    bool fresh = netstats_nb_isfresh(stats);

    if (stats->rssi == 0 && stats->lqi == 0) {
        stats->rssi = rssi;
        stats->lqi = lqi;
    } else {
        /* Exponential weighted moving average */
        stats->rssi = _ewma(fresh, stats->rssi, rssi);
        stats->lqi = _ewma(fresh, stats->lqi, lqi);
    }

    netstats_nb_incr_freshness(stats);
    stats->rx_count++;

    return stats;
}
#endif
