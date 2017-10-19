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
 * @brief       Implementation of the LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "kernel_types.h"
#include "msg.h"
#include "thread.h"
#include "timex.h"
#include "random.h"
#include "periph/rtt.h"
#include "net/gnrc.h"
#include "net/netdev.h"
//#include "net/gnrc/netdev.h"
#include "net/gnrc/lwmac/types.h"
#include "net/gnrc/lwmac/lwmac.h"
#include "net/gnrc/mac/internal.h"
#include "net/gnrc/lwmac/timeout.h"
#include "include/tx_state_machine.h"
#include "include/rx_state_machine.h"
#include "include/lwmac_internal.h"


#include "bitfield.h"
#include "net/ethernet.h"
#include "net/ipv6.h"
#include "net/gnrc.h"
#ifdef MODULE_NETSTATS_IPV6
#include "net/netstats.h"
#endif
#include "log.h"
#include "sched.h"

#include "net/gnrc/netif2.h"
#include "net/gnrc/netif2/internal.h"

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
 * @brief  LWMAC thread's PID
 */
kernel_pid_t lwmac_pid;

static void rtt_cb(void *arg);
static void lwmac_set_state(gnrc_netif2_t *netif, gnrc_lwmac_state_t newstate);
static void lwmac_schedule_update(gnrc_netif2_t *netif);
static void rtt_handler(uint32_t event, gnrc_netif2_t *netif);

//////////////////// netif2

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static gnrc_netif2_t _netifs[GNRC_NETIF_NUMOF];

static void _update_l2addr_from_dev(gnrc_netif2_t *netif);
static void *_lwmac_thread(void *args);
static void _event_cb(netdev_t *dev, netdev_event_t event);

static void _update_l2addr_from_dev(gnrc_netif2_t *netif)
{
    netdev_t *dev = netif->dev;
    int res;
    netopt_t opt = NETOPT_ADDRESS;

    switch (netif->device_type) {
#ifdef MODULE_NETDEV_IEEE802154
        case NETDEV_TYPE_IEEE802154: {
                uint16_t tmp;

                res = dev->driver->get(dev, NETOPT_SRC_LEN, &tmp, sizeof(tmp));
                assert(res == sizeof(tmp));
                netif->l2addr_len = (uint8_t)tmp;
                if (tmp == IEEE802154_LONG_ADDRESS_LEN) {
                    opt = NETOPT_ADDRESS_LONG;
                }
            }
            break;
#endif
        default:
            break;
    }
    res = dev->driver->get(dev, opt, netif->l2addr,
                           sizeof(netif->l2addr));
    if (res != -ENOTSUP) {
        netif->flags |= GNRC_NETIF2_FLAGS_HAS_L2ADDR;
    }
    if (res > 0) {
        netif->l2addr_len = res;
    }
}

static void _init_from_device(gnrc_netif2_t *netif)
{
    int res;
    netdev_t *dev = netif->dev;
    uint16_t tmp;

    res = dev->driver->get(dev, NETOPT_DEVICE_TYPE, &tmp, sizeof(tmp));
    (void)res;
    assert(res == sizeof(tmp));
    netif->device_type = (uint8_t)tmp;
    switch (netif->device_type) {
#ifdef MODULE_NETDEV_IEEE802154
        case NETDEV_TYPE_IEEE802154:
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
            netif->flags |= GNRC_NETIF2_FLAGS_6LO_HC;
#endif
#ifdef MODULE_GNRC_IPV6
            res = dev->driver->get(dev, NETOPT_MAX_PACKET_SIZE, &tmp, sizeof(tmp));
            assert(res == sizeof(tmp));
#ifdef MODULE_GNRC_SIXLOWPAN
            netif->ipv6.mtu = IPV6_MIN_MTU;
            netif->sixlo.max_frag_size = tmp;
#else
            netif->ipv6.mtu = tmp;
#endif
#endif
            break;
#endif  /* MODULE_NETDEV_IEEE802154 */
#ifdef MODULE_NETDEV_ETH
        case NETDEV_TYPE_ETHERNET:
#ifdef MODULE_GNRC_IPV6
            netif->ipv6.mtu = ETHERNET_DATA_LEN;
#endif
            break;
#endif
        default:
            res = dev->driver->get(dev, NETOPT_MAX_PACKET_SIZE, &tmp, sizeof(tmp));
            assert(res == sizeof(tmp));
#ifdef MODULE_GNRC_IPV6
            netif->ipv6.mtu = tmp;
#endif
    }
    _update_l2addr_from_dev(netif);
}

////////netif2

static gnrc_mac_tx_neighbor_t *_next_tx_neighbor(gnrc_netif2_t *netif)
{
    int next = -1;

    uint32_t phase_nearest = GNRC_LWMAC_PHASE_MAX;

    for (int i = 0; i < GNRC_MAC_NEIGHBOR_COUNT; i++) {
        if (gnrc_priority_pktqueue_length(&netif->mac.tx.neighbors[i].queue) > 0) {
            /* Unknown destinations are initialized with their phase at the end
             * of the local interval, so known destinations that still wakeup
             * in this interval will be preferred. */
            uint32_t phase_check = _gnrc_lwmac_ticks_until_phase(netif->mac.tx.neighbors[i].phase);

            if (phase_check <= phase_nearest) {
                next = i;
                phase_nearest = phase_check;
                DEBUG("[LWMAC-int] Advancing queue #%d\n", i);
            }
        }
    }

    return (next < 0) ? NULL : &(netif->mac.tx.neighbors[next]);
}

static uint32_t _next_inphase_event(uint32_t last, uint32_t interval)
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
    while (last < (rtt_get_counter() + GNRC_LWMAC_RTT_EVENT_MARGIN_TICKS)) {
        last += interval;
    }

    return last;
}

inline void lwmac_schedule_update(gnrc_netif2_t *netif)
{
    gnrc_netdev_lwmac_set_reschedule(netif, true);
}

void lwmac_set_state(gnrc_netif2_t *netif, gnrc_lwmac_state_t newstate)
{
    gnrc_lwmac_state_t oldstate = netif->mac.lwmac.state;

    if (newstate == oldstate) {
        return;
    }

    if (newstate >= GNRC_LWMAC_STATE_COUNT) {
        LOG_ERROR("ERROR: [LWMAC] Trying to set invalid state %u\n", newstate);
        return;
    }

    /* Already change state, but might be reverted to oldstate when needed */
    netif->mac.lwmac.state = newstate;

    /* Actions when leaving old state */
    switch (oldstate) {
        case GNRC_LWMAC_RECEIVING:
        case GNRC_LWMAC_TRANSMITTING: {
            /* Enable duty cycling again */
            rtt_handler(GNRC_LWMAC_EVENT_RTT_RESUME, netif);
#if (GNRC_LWMAC_ENABLE_DUTYCYLE_RECORD == 1)
            /* Output duty-cycle ratio */
            uint64_t duty;
            duty = (uint64_t) rtt_get_counter();
            duty = ((uint64_t) netif->mac.lwmac.awake_duration_sum_ticks) * 100 /
                   (duty - (uint64_t)netif->mac.lwmac.system_start_time_ticks);
            printf("[LWMAC]: achieved duty-cycle: %lu %% \n", (uint32_t)duty);
#endif
            break;
        }
        case GNRC_LWMAC_SLEEPING: {
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);
            break;
        }
        default:
            break;
    }

    /* Actions when entering new state */
    switch (newstate) {
        /*********************** Operation states *********************************/
        case GNRC_LWMAC_LISTENING: {
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_IDLE);
            break;
        }
        case GNRC_LWMAC_SLEEPING: {
            /* Put transceiver to sleep */
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_SLEEP);
            /* We may have come here through RTT handler, so timeout may still be active */
            gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);

            if (gnrc_netdev_lwmac_get_phase_backoff(netif)) {
                gnrc_netdev_lwmac_set_phase_backoff(netif, false);
                uint32_t alarm;

                rtt_clear_alarm();
                alarm = random_uint32_range(RTT_US_TO_TICKS((3 * GNRC_LWMAC_WAKEUP_DURATION_US / 2)),
                                            RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US -
                                                            (3 * GNRC_LWMAC_WAKEUP_DURATION_US / 2)));
                LOG_WARNING("WARNING: [LWMAC] phase backoffed: %lu us\n", RTT_TICKS_TO_US(alarm));
                netif->mac.lwmac.last_wakeup = netif->mac.lwmac.last_wakeup + alarm;
                alarm = _next_inphase_event(netif->mac.lwmac.last_wakeup,
                                            RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US));
                rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING);
            }

            /* Return immediately, so no rescheduling */
            return;
        }
        /* Trying to send data */
        case GNRC_LWMAC_TRANSMITTING: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, netif);    /**< No duty cycling while RXing */
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_IDLE);  /**< Power up netdev */
            break;
        }
        /* Receiving incoming data */
        case GNRC_LWMAC_RECEIVING: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, netif);    /**< No duty cycling while TXing */
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_IDLE);  /**< Power up netdev */
            break;
        }
        case GNRC_LWMAC_STOPPED: {
            _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_OFF);
            break;
        }
        /*********************** Control states ***********************************/
        case GNRC_LWMAC_START: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_START, netif);
            lwmac_set_state(netif, GNRC_LWMAC_LISTENING);
            break;
        }
        case GNRC_LWMAC_STOP: {
            rtt_handler(GNRC_LWMAC_EVENT_RTT_STOP, netif);
            lwmac_set_state(netif, GNRC_LWMAC_STOPPED);
            break;
        }
        case GNRC_LWMAC_RESET: {
            LOG_WARNING("WARNING: [LWMAC] Reset not yet implemented\n");
            lwmac_set_state(netif, GNRC_LWMAC_STOP);
            lwmac_set_state(netif, GNRC_LWMAC_START);
            break;
        }
        /**************************************************************************/
        default: {
            LOG_DEBUG("[LWMAC] No actions for entering state %u\n", newstate);
            return;
        }
    }

    lwmac_schedule_update(netif);
}

static void _sleep_management(gnrc_netif2_t *netif)
{
    /* If a packet is scheduled, no other (possible earlier) packet can be
     * sent before the first one is handled, even no broadcast
     */
    if (!gnrc_lwmac_timeout_is_running(netif, GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP)) {
        gnrc_mac_tx_neighbor_t *neighbour;

        /* Check if there is packet remaining for retransmission */
        if (netif->mac.tx.current_neighbor != NULL) {
            neighbour = netif->mac.tx.current_neighbor;
        }
        else {
            /* Check if there are broadcasts to send and transmit immediately */
            if (gnrc_priority_pktqueue_length(&(netif->mac.tx.neighbors[0].queue)) > 0) {
            	netif->mac.tx.current_neighbor = &(netif->mac.tx.neighbors[0]);
                lwmac_set_state(netif, GNRC_LWMAC_TRANSMITTING);
                return;
            }
            neighbour = _next_tx_neighbor(netif);
        }

        if (neighbour != NULL) {
            /* if phase is unknown, send immediately. */
            if (neighbour->phase > RTT_TICKS_TO_US(GNRC_LWMAC_WAKEUP_INTERVAL_US)) {
            	netif->mac.tx.current_neighbor = neighbour;
                gnrc_netdev_lwmac_set_tx_continue(netif, false);
                netif->mac.tx.tx_burst_count = 0;
                lwmac_set_state(netif, GNRC_LWMAC_TRANSMITTING);
                return;
            }

            /* Offset in microseconds when the earliest (phase) destination
             * node wakes up that we have packets for. */
            int time_until_tx = RTT_TICKS_TO_US(_gnrc_lwmac_ticks_until_phase(neighbour->phase));

            /* If there's not enough time to prepare a WR to catch the phase
             * postpone to next interval */
            if (time_until_tx < GNRC_LWMAC_WR_PREPARATION_US) {
                time_until_tx += GNRC_LWMAC_WAKEUP_INTERVAL_US;
            }
            time_until_tx -= GNRC_LWMAC_WR_PREPARATION_US;

            /* add a random time before goto TX, for avoiding one node for
             * always holding the medium (if the receiver's phase is recorded earlier in this
             * particular node) */
            uint32_t random_backoff;
            random_backoff = random_uint32_range(0, GNRC_LWMAC_TIME_BETWEEN_WR_US);
            time_until_tx = time_until_tx + random_backoff;

            gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP, time_until_tx);

            /* Register neighbour to be the next */
            netif->mac.tx.current_neighbor = neighbour;

            /* Stop dutycycling, we're preparing to send. This prevents the
             * timeout arriving late, so that the destination phase would
             * be missed. */
            /* TODO: bad for power savings */
            rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, netif);
        }
    }
    else if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP)) {
        LOG_DEBUG("[LWMAC] Got timeout for dest wakeup, ticks: %" PRIu32 "\n", rtt_get_counter());
        gnrc_netdev_lwmac_set_tx_continue(netif, false);
        netif->mac.tx.tx_burst_count = 0;
        lwmac_set_state(netif, GNRC_LWMAC_TRANSMITTING);
    }
}

static void _rx_management_failed(gnrc_netif2_t *netif)
{
    /* This may happen frequently because we'll receive WA from
     * every node in range. */
    LOG_DEBUG("[LWMAC] Reception was NOT successful\n");
    gnrc_lwmac_rx_stop(netif);

    if (netif->mac.rx.rx_bad_exten_count >= GNRC_LWMAC_MAX_RX_EXTENSION_NUM) {
        gnrc_netdev_lwmac_set_quit_rx(netif, true);
    }

    /* Here we check if we are close to the end of the cycle. If yes,
     * go to sleep. Firstly, get the relative phase. */
    uint32_t phase = rtt_get_counter();
    if (phase < netif->mac.lwmac.last_wakeup) {
        phase = (RTT_US_TO_TICKS(GNRC_LWMAC_PHASE_MAX) - netif->mac.lwmac.last_wakeup) +
                 phase;
    }
    else {
        phase = phase - netif->mac.lwmac.last_wakeup;
    }
    /* If the relative phase is beyond 4/5 cycle time, go to sleep. */
    if (phase > (4*RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US)/5)) {
        gnrc_netdev_lwmac_set_quit_rx(netif, true);
    }

    if (gnrc_netdev_lwmac_get_quit_rx(netif)) {
        lwmac_set_state(netif, GNRC_LWMAC_SLEEPING);
    }
    else {
        /* Go back to LISTENING for keep hearing on the channel */
        lwmac_set_state(netif, GNRC_LWMAC_LISTENING);
    }
}

static void _rx_management_success(gnrc_netif2_t *netif)
{
    LOG_DEBUG("[LWMAC] Reception was successful\n");
    gnrc_lwmac_rx_stop(netif);
    /* Dispatch received packets, timing is not critical anymore */
    gnrc_mac_dispatch(&netif->mac.rx);

    /* Here we check if we are close to the end of the cycle. If yes,
     * go to sleep. Firstly, get the relative phase. */
    uint32_t phase = rtt_get_counter();
    if (phase < netif->mac.lwmac.last_wakeup) {
        phase = (RTT_US_TO_TICKS(GNRC_LWMAC_PHASE_MAX) - netif->mac.lwmac.last_wakeup) +
                 phase;
    }
    else {
        phase = phase - netif->mac.lwmac.last_wakeup;
    }
    /* If the relative phase is beyond 4/5 cycle time, go to sleep. */
    if (phase > (4*RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US)/5)) {
        gnrc_netdev_lwmac_set_quit_rx(netif, true);
    }

    if (gnrc_netdev_lwmac_get_quit_rx(netif)) {
        lwmac_set_state(netif, GNRC_LWMAC_SLEEPING);
    }
    else {
        /* Go back to LISTENING after successful reception */
        lwmac_set_state(netif, GNRC_LWMAC_LISTENING);
    }
}
static void _rx_management(gnrc_netif2_t *netif)
{
    gnrc_lwmac_rx_state_t state_rx = netif->mac.rx.state;

    switch (state_rx) {
        case GNRC_LWMAC_RX_STATE_STOPPED: {
            gnrc_lwmac_rx_start(netif);
            gnrc_lwmac_rx_update(netif);
            break;
        }
        case GNRC_LWMAC_RX_STATE_FAILED: {
            _rx_management_failed(netif);
            break;
        }
        case GNRC_LWMAC_RX_STATE_SUCCESSFUL: {
            _rx_management_success(netif);
            break;
        }
        default:
            gnrc_lwmac_rx_update(netif);
    }

    /* If state has changed, reschedule main state machine */
    if (state_rx != netif->mac.rx.state) {
        lwmac_schedule_update(netif);
    }
}

static void _tx_management_stopped(gnrc_netif2_t *netif)
{
    gnrc_pktsnip_t *pkt;

    /* If there is packet remaining for retransmission,
     * retransmit it (i.e., the retransmission scheme of LWMAC). */
    if (netif->mac.tx.packet != NULL) {
        LOG_WARNING("WARNING: [LWMAC] TX %d times retry\n",
                         netif->mac.tx.tx_retry_count);
        netif->mac.tx.state = GNRC_LWMAC_TX_STATE_INIT;
        netif->mac.tx.wr_sent = 0;
        gnrc_lwmac_tx_update(netif);
    }
    else {
        if ((pkt = gnrc_priority_pktqueue_pop(
                 &netif->mac.tx.current_neighbor->queue))) {
            netif->mac.tx.tx_retry_count = 0;
            gnrc_lwmac_tx_start(netif, pkt, netif->mac.tx.current_neighbor);
            gnrc_lwmac_tx_update(netif);
        }
        else {
            /* Shouldn't happen, but never observed this case */
            lwmac_set_state(netif, GNRC_LWMAC_SLEEPING);
        }
    }
}

static void _tx_management_success(gnrc_netif2_t *netif)
{
    if (netif->mac.tx.current_neighbor == &(netif->mac.tx.neighbors[0])) {
        LOG_INFO("[LWMAC] Broadcast transmission done\n");
    }

    gnrc_lwmac_tx_stop(netif);

    /* In case have pending packets for the same receiver, continue to
     * send immediately, before the maximum transmit-limit */
    if ((gnrc_netdev_lwmac_get_tx_continue(netif)) &&
        (netif->mac.tx.tx_burst_count < GNRC_LWMAC_MAX_TX_BURST_PKT_NUM)) {
        lwmac_schedule_update(netif);
    }
    else {
        lwmac_set_state(netif, GNRC_LWMAC_SLEEPING);
    }
}

static void _tx_management(gnrc_netif2_t *netif)
{
    gnrc_lwmac_tx_state_t state_tx = netif->mac.tx.state;

    switch (state_tx) {
        case GNRC_LWMAC_TX_STATE_STOPPED: {
            _tx_management_stopped(netif);
            break;
        }
        case GNRC_LWMAC_TX_STATE_FAILED: {
            /* If transmission failure, do not try burst transmissions and quit other
             * transmission attempts in this cycle for collision avoidance */
            gnrc_netdev_lwmac_set_tx_continue(netif, false);
            gnrc_netdev_lwmac_set_quit_tx(netif, true);
            /* falls through */
            /* TX packet will therefore be dropped. No automatic resending here,
             * we did our best.
             */
        }
        case GNRC_LWMAC_TX_STATE_SUCCESSFUL: {
            _tx_management_success(netif);
            break;
        }
        default:
            gnrc_lwmac_tx_update(netif);
    }

    /* If state has changed, reschedule main state machine */
    if (state_tx != netif->mac.tx.state) {
        lwmac_schedule_update(netif);
    }
}

static void _lwmac_update_listening(gnrc_netif2_t *netif)
{
    /* In case has pending packet to send, clear rtt alarm thus to goto
     * transmission initialization (in SLEEPING management) right after the
     * listening period */
    if ((_next_tx_neighbor(netif) != NULL) ||
        (netif->mac.tx.current_neighbor != NULL)) {
        rtt_handler(GNRC_LWMAC_EVENT_RTT_PAUSE, netif);
    }

    /* Set timeout for if there's no successful rx transaction that will
     * change state to SLEEPING. */
    if (!gnrc_lwmac_timeout_is_running(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD)) {
        gnrc_lwmac_set_timeout(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD, GNRC_LWMAC_WAKEUP_DURATION_US);
    }
    else if (gnrc_lwmac_timeout_is_expired(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD)) {
        /* Dispatch first as there still may be broadcast packets. */
        gnrc_mac_dispatch(&netif->mac.rx);

        netif->mac.lwmac.state = GNRC_LWMAC_SLEEPING;
        /* Enable duty cycling again */
        rtt_handler(GNRC_LWMAC_EVENT_RTT_RESUME, netif);

        _gnrc_lwmac_set_netdev_state(netif, NETOPT_STATE_SLEEP);
        gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);

        /* if there is a packet for transmission, schedule update to start
         * transmission initialization immediately. */
        gnrc_mac_tx_neighbor_t *neighbour = _next_tx_neighbor(netif);
        if ((neighbour != NULL) || (netif->mac.tx.current_neighbor != NULL)) {
            /* This triggers packet sending procedure in sleeping immediately. */
            lwmac_schedule_update(netif);
            return;
        }
    }

    if (gnrc_priority_pktqueue_length(&netif->mac.rx.queue) > 0) {
        /* Do wake-up extension in each packet reception. */
        gnrc_lwmac_clear_timeout(netif, GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD);
        lwmac_set_state(netif, GNRC_LWMAC_RECEIVING);
    }
}

/* Main state machine. Call whenever something happens */
static bool lwmac_update(gnrc_netif2_t *netif)
{
    gnrc_netdev_lwmac_set_reschedule(netif, false);

    switch (netif->mac.lwmac.state) {
        case GNRC_LWMAC_SLEEPING: {
            /* Quit scheduling transmission if 'quit-tx' flag is found set, thus
             * to avoid potential collisions with ongoing transmissions of other
             * neighbor nodes */
            if (gnrc_netdev_lwmac_get_quit_tx(netif)) {
                return false;
            }

            _sleep_management(netif);
            break;
        }
        case GNRC_LWMAC_LISTENING: {
            _lwmac_update_listening(netif);
            break;
        }
        case GNRC_LWMAC_RECEIVING: {
            _rx_management(netif);
            break;
        }
        case GNRC_LWMAC_TRANSMITTING: {
            _tx_management(netif);
            break;
        }
        default:
            LOG_DEBUG("[LWMAC] No actions in state %u\n", netif->mac.lwmac.state);
    }

    return gnrc_netdev_lwmac_get_reschedule(netif);
}

static void rtt_cb(void *arg)
{
    msg_t msg;

    msg.content.value = ((uint32_t) arg) & 0xffff;
    msg.type = GNRC_LWMAC_EVENT_RTT_TYPE;
    msg_send(&msg, lwmac_pid);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

void rtt_handler(uint32_t event, gnrc_netif2_t *netif)
{
    uint32_t alarm;

    switch (event & 0xffff) {
        case GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING: {
            /* A new cycle starts, set sleep timing and initialize related MAC-info flags. */
        	netif->mac.lwmac.last_wakeup = rtt_get_alarm();
            alarm = _next_inphase_event(netif->mac.lwmac.last_wakeup,
                                        RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_DURATION_US));
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_SLEEP_PENDING);
            gnrc_netdev_lwmac_set_quit_tx(netif, false);
            gnrc_netdev_lwmac_set_quit_rx(netif, false);
            gnrc_netdev_lwmac_set_phase_backoff(netif, false);
            netif->mac.rx.rx_bad_exten_count = 0;
            lwmac_set_state(netif, GNRC_LWMAC_LISTENING);
            break;
        }
        case GNRC_LWMAC_EVENT_RTT_SLEEP_PENDING: {
            /* Set next wake-up timing. */
            alarm = _next_inphase_event(netif->mac.lwmac.last_wakeup,
                                        RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US));
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING);
            lwmac_set_state(netif, GNRC_LWMAC_SLEEPING);
            break;
        }
        /* Set initial wake-up alarm that starts the cycle */
        case GNRC_LWMAC_EVENT_RTT_START: {
            LOG_DEBUG("[LWMAC] RTT: Initialize duty cycling\n");
            alarm = rtt_get_counter() + RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_DURATION_US);
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_SLEEP_PENDING);
            gnrc_netdev_lwmac_set_dutycycle_active(netif, true);
            break;
        }
        case GNRC_LWMAC_EVENT_RTT_STOP:
        case GNRC_LWMAC_EVENT_RTT_PAUSE: {
            rtt_clear_alarm();
            LOG_DEBUG("[LWMAC] RTT: Stop duty cycling, now in state %u\n",
                      netif->mac.lwmac.state);
            gnrc_netdev_lwmac_set_dutycycle_active(netif, false);
            break;
        }
        case GNRC_LWMAC_EVENT_RTT_RESUME: {
            LOG_DEBUG("[LWMAC] RTT: Resume duty cycling\n");
            rtt_clear_alarm();
            alarm = _next_inphase_event(netif->mac.lwmac.last_wakeup,
                                        RTT_US_TO_TICKS(GNRC_LWMAC_WAKEUP_INTERVAL_US));
            rtt_set_alarm(alarm, rtt_cb, (void *) GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING);
            gnrc_netdev_lwmac_set_dutycycle_active(netif, true);
            break;
        }
        default:
            break;
    }
}

/**
 * @brief   Function called by the device driver on device events
 *
 * @param[in] event         type of event
 * @param[in] data          optional parameter
 */
static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    gnrc_netif2_t *netif = (gnrc_netif2_t *) dev->context;

    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;

        msg.type = NETDEV_MSG_TYPE_EVENT;
        msg.content.ptr = (void *) netif;

        if (msg_send(&msg, netif->pid) <= 0) {
            LOG_WARNING("WARNING: [LWMAC] gnrc_netdev: possibly lost interrupt.\n");
        }
    }
    else {
        DEBUG("gnrc_netdev: event triggered -> %i\n", event);
        switch (event) {
            case NETDEV_EVENT_RX_STARTED: {
                LOG_DEBUG("[LWMAC] NETDEV_EVENT_RX_STARTED\n");
                gnrc_netdev_set_rx_started(netif, true);
                break;
            }
            case NETDEV_EVENT_RX_COMPLETE: {
                LOG_DEBUG("[LWMAC] NETDEV_EVENT_RX_COMPLETE\n");
                gnrc_pktsnip_t *pkt = netif->ops->recv(netif);

                /* Prevent packet corruption when a packet is sent before the previous
                 * received packet has been downloaded. This happens e.g. when a timeout
                 * expires that causes the tx state machine to send a packet. When a
                 * packet arrives after the timeout, the notification is queued but the
                 * tx state machine continues to send and then destroys the received
                 * packet in the frame buffer. After completion, the queued notification
                 * will be handled a corrupted packet will be downloaded. Therefore
                 * keep track that RX_STARTED is followed by RX_COMPLETE.
                 *
                 * TODO: transceivers might have 2 frame buffers, so make this optional
                 */
                if (pkt == NULL) {
                    gnrc_netdev_set_rx_started(netif, false);
                    break;
                }

                gnrc_netdev_set_rx_started(netif, false);

                if (!gnrc_mac_queue_rx_packet(&netif->mac.rx, 0, pkt)) {
                    LOG_ERROR("ERROR: [LWMAC] Can't push RX packet @ %p, memory full?\n", pkt);
                    gnrc_pktbuf_release(pkt);
                    break;
                }
                lwmac_schedule_update(netif);
                break;
            }
            case NETDEV_EVENT_TX_STARTED: {
                gnrc_netdev_set_tx_feedback(netif, TX_FEEDBACK_UNDEF);
                gnrc_netdev_set_rx_started(netif, false);
                break;
            }
            case NETDEV_EVENT_TX_COMPLETE: {
                gnrc_netdev_set_tx_feedback(netif, TX_FEEDBACK_SUCCESS);
                gnrc_netdev_set_rx_started(netif, false);
                lwmac_schedule_update(netif);
                break;
            }
            case NETDEV_EVENT_TX_NOACK: {
                gnrc_netdev_set_tx_feedback(netif, TX_FEEDBACK_NOACK);
                gnrc_netdev_set_rx_started(netif, false);
                lwmac_schedule_update(netif);
                break;
            }
            case NETDEV_EVENT_TX_MEDIUM_BUSY: {
                gnrc_netdev_set_tx_feedback(netif, TX_FEEDBACK_BUSY);
                gnrc_netdev_set_rx_started(netif, false);
                lwmac_schedule_update(netif);
                break;
            }
            default:
                LOG_WARNING("WARNING: [LWMAC] Unhandled netdev event: %u\n", event);
        }
    }
}

/**
 * @brief   Startup code and event loop of the LWMAC layer
 *
 * @param[in] args          expects a pointer to the underlying netdev device
 *
 * @return                  never returns
 */
static void *_lwmac_thread(void *args)
{
    gnrc_netapi_opt_t *opt;
    gnrc_netif2_t *netif;
    netdev_t *dev;
    int res;
    msg_t reply = { .type = GNRC_NETAPI_MSG_TYPE_ACK };
    msg_t msg, msg_queue[GNRC_LWMAC_IPC_MSG_QUEUE_SIZE];

    DEBUG("lwmac: starting thread %i\n", sched_active_pid);
    netif = args;
    gnrc_netif2_acquire(netif);
    dev = netif->dev;
    netif->pid = sched_active_pid;
    /* setup the link-layer's message queue */
    msg_init_queue(msg_queue, GNRC_LWMAC_IPC_MSG_QUEUE_SIZE);
    /* register the event callback with the device driver */
    dev->event_callback = _event_cb;
    dev->context = netif;
    /* initialize low-level driver */
    dev->driver->init(dev);
    _init_from_device(netif);
    netif->cur_hl = GNRC_NETIF2_DEFAULT_HL;
#ifdef MODULE_GNRC_IPV6_NIB
    gnrc_ipv6_nib_init_iface(netif);
#endif
    if (netif->ops->init) {
        netif->ops->init(netif);
    }
    /* now let rest of GNRC use the interface */
    gnrc_netif2_release(netif);

    ///////////////// end of netif2
#if 0
    gnrc_netdev_t *gnrc_netdev = (gnrc_netdev_t *)args;
    netdev_t *dev = gnrc_netdev->dev;

    gnrc_netdev->pid = thread_getpid();

    gnrc_netapi_opt_t *opt;
    int res;
    msg_t msg, reply, msg_queue[GNRC_LWMAC_IPC_MSG_QUEUE_SIZE];

    LOG_INFO("[LWMAC] Starting LWMAC\n");

    /* setup the MAC layers message queue */
    msg_init_queue(msg_queue, GNRC_LWMAC_IPC_MSG_QUEUE_SIZE);

    /* register the event callback with the device driver */
    dev->event_callback = _event_cb;
    dev->context = (void *) gnrc_netdev;

    /* register the device to the network stack*/
    gnrc_netif_add(thread_getpid());

    /* initialize low-level driver */
    dev->driver->init(dev);

    dev->driver->set(dev, NETOPT_RX_START_IRQ, &enable, sizeof(enable));
    dev->driver->set(dev, NETOPT_TX_START_IRQ, &enable, sizeof(enable));
    dev->driver->set(dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));

    /* Get own address from netdev */
    gnrc_netdev->l2_addr_len = dev->driver->get(dev, NETOPT_ADDRESS_LONG,
                                                &gnrc_netdev->l2_addr,
                                                IEEE802154_LONG_ADDRESS_LEN);
    assert(gnrc_netdev->l2_addr_len > 0);

    netif->mac.tx.bcast_seqnr = gnrc_netdev->l2_addr[0];
#endif

    /* RTT is used for scheduling wakeup */
    rtt_init();

    /* Store pid globally, so that IRQ can use it to send msg */
    lwmac_pid = netif->pid;

    /* Enable RX- and TX-started interrupts  */
    netopt_enable_t enable = NETOPT_ENABLE;
    netif->dev->driver->set(netif->dev, NETOPT_RX_START_IRQ, &enable, sizeof(enable));
    netif->dev->driver->set(netif->dev, NETOPT_TX_START_IRQ, &enable, sizeof(enable));
    netif->dev->driver->set(netif->dev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));

    uint16_t src_len = IEEE802154_LONG_ADDRESS_LEN;
    netif->dev->driver->set(netif->dev, NETOPT_SRC_LEN, &src_len, sizeof(src_len));

    /* Initialize broadcast sequence number. This at least differs from board
     * to board */
    netif->mac.tx.bcast_seqnr = netif->l2addr[0];

    /* Reset all timeouts just to be sure */
    gnrc_lwmac_reset_timeouts(netif);

    /* Start duty cycling */
    lwmac_set_state(netif, GNRC_LWMAC_START);

#if (GNRC_LWMAC_ENABLE_DUTYCYLE_RECORD == 1)
    /* Start duty cycle recording */
    netif->mac.lwmac.system_start_time_ticks = rtt_get_counter();
    netif->mac.lwmac.last_radio_on_time_ticks = netif->mac.lwmac.system_start_time_ticks;
    netif->mac.lwmac.awake_duration_sum_ticks = 0;
    netif->mac.lwmac.lwmac_info |= GNRC_LWMAC_RADIO_IS_ON;
#endif

    /* start the event loop */
    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            /* Transceiver raised an interrupt */
            case NETDEV_MSG_TYPE_EVENT: {
                DEBUG("[LWMAC] GNRC_NETDEV_MSG_TYPE_EVENT received\n");
                dev->driver->isr(dev);
                break;
            }

            /* TX: Queue for sending */
            case GNRC_NETAPI_MSG_TYPE_SND: {
                /* TODO: how to announce failure to upper layers? */
                LOG_DEBUG("[LWMAC] GNRC_NETAPI_MSG_TYPE_SND received\n");
                gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) msg.content.ptr;

                if (!gnrc_mac_queue_tx_packet(&netif->mac.tx, 0, pkt)) {
                    gnrc_pktbuf_release(pkt);
                    LOG_WARNING("WARNING: [LWMAC] TX queue full, drop packet\n");
                }

                lwmac_schedule_update(netif);
                break;
            }

            /* NETAPI set/get. Can't this be refactored away from here? */
            case GNRC_NETAPI_MSG_TYPE_SET: {
                LOG_DEBUG("[LWMAC] GNRC_NETAPI_MSG_TYPE_SET received\n");
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;

                /* Depending on option forward to NETDEV or handle here */
                switch (opt->opt) {
                    /* Handle state change requests */
                    case NETOPT_STATE: {
                        netopt_state_t *state = (netopt_state_t *) opt->data;
                        res = opt->data_len;
                        switch (*state) {
                            case NETOPT_STATE_OFF: {
                                lwmac_set_state(netif, GNRC_LWMAC_STOP);
                                break;
                            }
                            case NETOPT_STATE_IDLE: {
                                lwmac_set_state(netif, GNRC_LWMAC_START);
                                break;
                            }
                            case NETOPT_STATE_RESET: {
                                lwmac_set_state(netif, GNRC_LWMAC_RESET);
                                break;
                            }
                            default:
                                res = -EINVAL;
                                LOG_ERROR("ERROR: [LWMAC] NETAPI tries to set unsupported"
                                               " state %u\n",*state);
                        }
                        lwmac_schedule_update(netif);
                        break;
                    }
                    /* Forward to netdev by default*/
                    default:
                        /* set option for device driver */
                        res = netif->ops->set(netif, opt);
                        LOG_DEBUG("[LWMAC] Response of netif->ops->set(): %i\n", res);
                }

                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            }

            case GNRC_NETAPI_MSG_TYPE_GET: {
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                LOG_DEBUG("[LWMAC] GNRC_NETAPI_MSG_TYPE_GET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* get option from device driver */
                res = netif->ops->get(netif, opt);
                LOG_DEBUG("[LWMAC] Response of netif->ops->get(): %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            }

            /* RTT raised an interrupt */
            case GNRC_LWMAC_EVENT_RTT_TYPE: {
                if (gnrc_netdev_lwmac_get_dutycycle_active(netif)) {
                    rtt_handler(msg.content.value, netif);
                    lwmac_schedule_update(netif);
                }
                else {
                    LOG_DEBUG("[LWMAC] Ignoring late RTT event while duty-cycling is off\n");
                }
                break;
            }

            /* An LWMAC timeout occurred */
            case GNRC_LWMAC_EVENT_TIMEOUT_TYPE: {
                gnrc_lwmac_timeout_make_expire((gnrc_lwmac_timeout_t *) msg.content.ptr);
                lwmac_schedule_update(netif);
                break;
            }

            default: {
                if (netif->ops->msg_handler) {
                    DEBUG("gnrc_netif2: delegate message of type 0x%04x to "
                          "netif->ops->msg_handler()\n", msg.type);
                    netif->ops->msg_handler(netif, &msg);
                }
#if ENABLE_DEBUG
                else {
                    DEBUG("gnrc_netif2: unknown message type 0x%04x"
                          "(no message handler defined)\n", msg.type);
                }
#endif
                break;
            }
        }

        /* Execute main state machine because something just happend*/
        while (gnrc_netdev_lwmac_get_reschedule(netif)) {
            lwmac_update(netif);
        }
    }

    LOG_ERROR("ERROR: [LWMAC] terminated\n");

    /* never reached */
    return NULL;
}

#if 0
kernel_pid_t gnrc_lwmac_init(char *stack, int stacksize, char priority,
                             const char *name, gnrc_netdev_t *dev)
{
    kernel_pid_t res;

    /* check if given netdev device is defined and the driver is set */
    if (dev == NULL || dev->dev == NULL) {
        LOG_ERROR("ERROR: [LWMAC] No netdev supplied or driver not set\n");
        return -ENODEV;
    }

    /* create new LWMAC thread */
    res = thread_create(stack, stacksize, priority, THREAD_CREATE_STACKTEST,
                        _lwmac_thread, (void *)dev, name);
    if (res <= 0) {
        LOG_ERROR("ERROR: [LWMAC] Couldn't create thread\n");
        return -EINVAL;
    }

    return res;
}
#endif

gnrc_netif2_t *gnrc_lwmac_init(char *stack, int stacksize, char priority,
                                  const char *name, netdev_t *netdev,
                                  const gnrc_netif2_ops_t *ops)
{
    gnrc_netif2_t *netif = NULL;
    int res;

    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        if (_netifs[i].dev == netdev) {
            return &_netifs[i];
        }
        if ((netif == NULL) && (_netifs[i].ops == NULL)) {
            netif = &_netifs[i];
        }
    }
    assert(netif != NULL);
    rmutex_init(&netif->mutex);
    netif->ops = ops;
    assert(netif->dev == NULL);
    netif->dev = netdev;
    res = thread_create(stack, stacksize, priority, THREAD_CREATE_STACKTEST,
    		            _lwmac_thread, (void *)netif, name);
    (void)res;
    assert(res > 0);
    return netif;
}
