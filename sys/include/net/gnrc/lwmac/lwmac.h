/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_lwmac Simplest possible MAC layer
 * @ingroup     net_gnrc
 * @brief       Lightweight MAC protocol that allows for duty cycling to save
 *              energy.
 * @{
 *
 * @file
 * @brief       Interface definition for the LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef GNRC_LWMAC_H
#define GNRC_LWMAC_H

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Time between consecutive wakeups. This parameter governs power consumption,
 *        latency and throughput!
 */
#ifndef LWMAC_WAKEUP_INTERVAL_US
#define LWMAC_WAKEUP_INTERVAL_US        (200U * US_PER_MS)
#endif

/**
 * @brief The Maximum WR (preamble) duration time. The idea was to send WRs/preambles
 *        for a slightly longer period than LWMAC_WAKEUP_INTERVAL_US in order to make
 *        sure that at least one WR will be received by the destination.
 */
#ifndef LWMAC_PREAMBLE_DURATION_US
#define LWMAC_PREAMBLE_DURATION_US      ((13 * LWMAC_WAKEUP_INTERVAL_US) / 10)
#endif

/**
 * @brief The Maximum random backoff time before actually sending each WR. This random
 *        backoff duration is temporally (maybe removed in the future) introduced to increase the
 *        probability of detection of WRs from other senders (i.e., two or more nearby senders
 *        are sending WRs at the same time), thus to avoid WR collisions.
 */
#ifndef LWMAC_RANDOM_BEFORE_WR_US
#define LWMAC_RANDOM_BEFORE_WR_US       (1U * US_PER_MS)
#endif

/**
 * @brief Timeout to send the next WR in case no WA has been received during that
 *        time. It is referenced to the beginning of both WRs, but due to internal
 *        overhead, the exact spacing is slightly higher.
 *        The minimum possible value depends on the time it takes to completely
 *        send a WR with the given hardware (including processor) and data rate.
 */
#ifndef LWMAC_TIME_BETWEEN_WR_US
#define LWMAC_TIME_BETWEEN_WR_US        (7U * US_PER_MS)
#endif

/**
 * @brief Time to idle between two successive BROADCAST packets, referenced to the
 *        start of the packet. The same limitation as for time-between-WR apply here.
 */
#ifndef LWMAC_TIME_BETWEEN_BROADCAST_US
#define LWMAC_TIME_BETWEEN_BROADCAST_US (LWMAC_TIME_BETWEEN_WR_US)
#endif

/**
 * @brief Receiver needs to support RX_START event in order to use time-between-WR
 *        as a sensible default here. Otherwise the duration of WRs as well as longest
 *        possible data broadcasts need to be taken into account.
 */
#ifndef LWMAC_WAKEUP_DURATION_US
#define LWMAC_WAKEUP_DURATION_US        (LWMAC_TIME_BETWEEN_WR_US * 2)
#endif

/**
 * @brief Start sending earlier then known phase. Therefore advance to beginning edge
 *        of destinations wakeup phase over time.
 *
 * Note:  RTT tick is ~30us @ 32 kHz timer.
 *        There is a certain overhead from dispatching driver call until WR
 *        will be really sent that may depend on hardware and driver
 *        implementation.
 */
#ifndef LWMAC_WR_BEFORE_PHASE_US
#define LWMAC_WR_BEFORE_PHASE_US        ((13 * US_PER_MS) / 10)
#endif

/**
 * @brief WR preparation overhead before it can be sent (higher with debugging output).
 *        LwMAC will wakeup earlier to prepare for synced WR sending. When preparation
 *        is done, it will busy wait (block the whole system) until the WR has been
 *        dispatched to the driver.
 */
#ifndef LWMAC_WR_PREPARATION_US
#define LWMAC_WR_PREPARATION_US         ((2U * US_PER_MS) + LWMAC_WR_BEFORE_PHASE_US)
#endif

/**
 * @brief How long to wait after a WA for data to come in. It's enough to catch the
 *        beginning of the packet if the transceiver supports RX_STARTED event (this
 *        can be important for big packets).
 */
#ifndef LWMAC_DATA_DELAY_US
#define LWMAC_DATA_DELAY_US             (10U * US_PER_MS)
#endif

/**
 * @brief How long BROADCAST packets will be sent to make sure every participant has
 *        received at least one packet.
 */
#ifndef LWMAC_BROADCAST_DURATION_US
#define LWMAC_BROADCAST_DURATION_US     ((LWMAC_WAKEUP_INTERVAL_US * 11) / 10)
#endif

/**
 * @brief Max link layer address length in bytes.
 */
#ifndef LWMAC_MAX_L2_ADDR_LEN
#define LWMAC_MAX_L2_ADDR_LEN           (8U)
#endif

/**
 * @brief CSMA retries for DATA packet after WR->WA was successful. Too many retries
 *        may timeout the receiver, refer LWMAC_DATA_DELAY_US.
 */
#ifndef LWMAC_DATA_CSMA_RETRIES
#define LWMAC_DATA_CSMA_RETRIES         (3U)
#endif

/**
 * @brief TX transmission retries for DATA packet in case of no response from the receiver.
 */
#ifndef LWMAC_DATA_TX_RETRIES
#define LWMAC_DATA_TX_RETRIES           (3U)
#endif

/**
 * @brief MAX burst transmission packet number in one shot.
 */
#ifndef LWMAC_MAX_TX_BURST_PKT_NUM
#define LWMAC_MAX_TX_BURST_PKT_NUM      (LWMAC_WAKEUP_INTERVAL_US / LWMAC_WAKEUP_DURATION_US)
#endif

/**
 * @brief MAX unnecessary Listen period extensions allowed when the device is in wakeup period.
 */
#ifndef LWMAC_MAX_RX_EXTENSION_NUM
#define LWMAC_MAX_RX_EXTENSION_NUM      (LWMAC_MAX_TX_BURST_PKT_NUM)
#endif

/**
 * @brief CSMA retries for BROADCAST packet, too many may lead to running out of
 *        destinations wakeup period.
 */
#ifndef LWMAC_BROADCAST_CSMA_RETRIES
#define LWMAC_BROADCAST_CSMA_RETRIES    (3U)
#endif

/**
 * @brief Set the default message queue size for LWMAC layer
 */
#ifndef LWMAC_IPC_MSG_QUEUE_SIZE
#define LWMAC_IPC_MSG_QUEUE_SIZE        (8U)
#endif

/**
 * @brief The default largest number of parallel timeouts in Lwmac
 */
#ifndef LWMAC_TIMEOUT_COUNT
#define LWMAC_TIMEOUT_COUNT             (3U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* GNRC_LWMAC_H */
/** @} */
