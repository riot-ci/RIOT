/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_lwmac Simplest possible MAC layer
 * @ingroup     net_gnrc
 * @brief       Lightweight MAC protocol that allows for duty cycling to save
 *              energy.
 *
 * LWMAC implementation
 * ====================
 *
 * # Radio duty cycling
 * LWMAC adopts the radio duty-cycle scheme to conserve power. Namely, in each
 * cycle period (MAC superframe), a node device wakes up for a short period of
 * time (called listen period or wake-up period) for receiving possible incoming
 * packets from other devices. Outside the listen period, the node device turns
 * off its radio to conserve power.
 *
 * # Phase-lock scheme
 * LWMAC adopts the phase-lock scheme to further reduce power consumption. Each
 * node device in LWMAC will try to record/track its Tx-neighbor's wake-up phase.
 * This is called phase-lock. After phase-locking, the sender node will (likely)
 * spend less preamble packets (also called WR packet, i.e., wake-up-request, in
 * LWMAC) for initiating a hand-shaking procedure for transmitting a data packet,
 * compared to the first time it talks to the receiver.
 *
 * # Burst transmission
 * LWMAC adopts pending-bit technique to enhance its throughput. Namely, in case
 * of having multi packets for the receiver, a sender uses the pending-bit flag
 * embedded in the MAC header to instruct this situation, and the buffered packets
 * will be transmitted in a continuous sequence, back to back, to the receiver in
 * one shot.
 *
 * # Auto wake-up extension
 * LWMAC adopts auto wake-up extension scheme based on timeout (like T-MAC). In short,
 * when a packet is successfully received at the receiver side, the receiver will
 * reset the wake-up timeout to extend its wake-up period for receiving more potential
 * incoming packets. This is to be compatible with the pending-bit technique to allow
 * the receiver to absorb more packets when needed, thus boosts the throughput.
 *
 * # Simple retransmission scheme
 * LWMAC adopts a simple retransmission scheme to enhance link reliability. The data
 * packet will only be dropped in case the retransmission counter gets larger than
 * LWMAC_MAX_DATA_TX_RETRIES.
 *
 * # Automatic phase backoff scheme
 * LWMAC adopts an automatic phase backoff scheme to reduce WR (preamble) collision
 * probability. In multi-hop scenarios, let's say, nodes A <---B <----C (which is
 * common in multi-hop data collection networks), in which B has packets for A, and
 * C has packets for B. In case A and B's wake-up phases are too close (overlapping).
 * Then, especially in high traffic conditions, B and C may initiate transmissions
 * at the same time (B sends to A, and C sends to B), a link of either will be
 * definitely interfered, leading to collisions and link throughput reduction. To
 * this end, by using the automatic phase backoff scheme, if a sender finds its
 * receiver's phase is too close to its own phase, it will run a backoff scheme to
 * randomly reselect a new wake-up phase for itself.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the LWMAC protocol
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef NET_GNRC_LWMAC_LWMAC_H
#define NET_GNRC_LWMAC_LWMAC_H

#include "kernel_types.h"
#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Time between consecutive wake-ups.
 *
 * This parameter governs power consumption, latency and throughput!
 * In LWMAC, devices adopt duty-cycle scheme to conserve power. That is,
 * time is divided into repeated cycles (or, superframes), and in each
 * cycle, a node only wakes up for a period of time for receiving potential
 * incoming packets for itself. This macro defines the wake-up interval, or,
 * in other words, defines the cycle duration used in LWMAC. If the wake-up interval
 * is short, nodes will wake up more frequently, which also increases
 * the chances for receiving packets from neighbors (i.e., leads to higher
 * throughput), but also results in higher power consumption.
 * In LWMAC, by default, we regard the wake-up period as the beginning of a cycle.
 */
#ifndef LWMAC_WAKEUP_INTERVAL_US
#define LWMAC_WAKEUP_INTERVAL_US        (100LU * US_PER_MS)
#endif

/**
 * @brief The Maximum WR (preamble) duration time.
 *
 * Since LWMAC adopts duty-cycle scheme, a node only wakes up for a short
 * period in each cycle. Thus, to probe where is the wake-up period of the
 * receiver, a sender sends WR (preamble) packets to notice the receiver for
 * communication. To ensure that the receiver will catch at least one WR
 * packet in one cycle, the sender repeatedly broadcasts a stream of WR packets
 * with the broadcast duration (preamble duration) slightly longer period than
 * LWMAC_WAKEUP_INTERVAL_US.
 */
#ifndef LWMAC_PREAMBLE_DURATION_US
#define LWMAC_PREAMBLE_DURATION_US      ((13LU * LWMAC_WAKEUP_INTERVAL_US) / 10)
#endif

/**
 * @brief The Maximum random backoff time before actually sending each WR.
 *
 * Currently, before actually sending a WR packet, a sender runs a random backoff with a
 * duration from 0 to LWMAC_RANDOM_BEFORE_WR_US. Then, after the random backoff,
 * the sender senses the channel to see if there is ongoing transmission.
 * If yes, the sender quits sending packet and delays the transmission attempt to the
 * next cycle. This random backoff duration is temporally (maybe removed in the future)
 * introduced to increase the probability of detection of WRs from other senders
 * (i.e., two or more nearby senders are sending WRs at the same time), thus to
 * avoid WR collisions.
 */
#ifndef LWMAC_RANDOM_BEFORE_WR_US
#define LWMAC_RANDOM_BEFORE_WR_US       (1U * US_PER_MS)
#endif

/**
 * @brief Timeout to send the next WR in case no WA has been received during that
 *        time.
 *
 * In LWMAC, when a sender initiates a transmission to a receiver, it starts with
 * sending a stream of repeated WR packets with LWMAC_TIME_BETWEEN_WR_US interval
 * between consecutive two WRs. After sending one WR (preamble), the sender turns
 * to the listen mode to receive the potential incoming WA (preamble-ACK) packet with
 * a timeout of LWMAC_TIME_BETWEEN_WR_US. If no WA is received during
 * LWMAC_TIME_BETWEEN_WR_US, the sender starts sending the next WR.
 * It is referenced to the beginning of both WRs, but due to internal
 * overhead, the exact spacing is slightly higher.
 * The minimum possible value depends on the time it takes to completely
 * send a WR with the given hardware (including processor) and data rate.
 */
#ifndef LWMAC_TIME_BETWEEN_WR_US
#define LWMAC_TIME_BETWEEN_WR_US        (5U * US_PER_MS)
#endif

/**
 * @brief How longer a node in LWMAC should keep awake and listen on the channel in one cycle.
 *
 * LWMAC adopts the duty-cycle scheme that a node only wakes up for a short
 * period of LWMAC_WAKEUP_DURATION_US in each cycle. In the rest of the cycle, the node
 * turns off the radio to conserve power. LWMAC_WAKEUP_DURATION_US is set to twice the
 * duration of LWMAC_TIME_BETWEEN_WR_US, to guarantee that the wake-up period is longer
 * enough that receiver will not miss the WR (preamble) packet.
 * Receiver needs to support RX_START event in order to use time-between-WR
 * as a sensible default here. Otherwise the duration of WRs as well as longest
 * possible data broadcasts need to be taken into account.
 */
#ifndef LWMAC_WAKEUP_DURATION_US
#define LWMAC_WAKEUP_DURATION_US        (LWMAC_TIME_BETWEEN_WR_US * 2)
#endif

/**
 * @brief How long BROADCAST packets will be sent to make sure every participant has
 *        received at least one packet.
 *
 * Since LWMAC adopts adopts duty-cycle scheme that a node only wakes up for a short
 * period in each cycle, thus, when a node wants to broadcast a packet and ensures that
 * all neighbors will catch one broadcast packet from it, it repeatedly broadcasts the
 * broadcast packet for one LWMAC_BROADCAST_DURATION_US duration which is slightly longer
 * than LWMAC_WAKEUP_INTERVAL_US.
 */
#ifndef LWMAC_BROADCAST_DURATION_US
#define LWMAC_BROADCAST_DURATION_US     ((LWMAC_WAKEUP_INTERVAL_US * 11) / 10)
#endif

/**
 * @brief Time to idle between two successive BROADCAST packets, referenced to the
 *        start of the packet.
 *
 * The same limitation as for time-between-WR apply here.
 * In LWMAC, when a sender initiates a broadcast, it starts with sending a stream of
 * repeated broadcast packets with LWMAC_TIME_BETWEEN_BROADCAST_US interval
 * between two consecutive broadcast packets. After sending one broadcast packet, the sender
 * turns to the listen mode with a timeout of LWMAC_TIME_BETWEEN_BROADCAST_US. When this
 * timeout expires, the sender sends the next broadcast packet until reaching the maximum
 * broadcast duration of LWMAC_BROADCAST_DURATION_US.
 */
#ifndef LWMAC_TIME_BETWEEN_BROADCAST_US
#define LWMAC_TIME_BETWEEN_BROADCAST_US (LWMAC_TIME_BETWEEN_WR_US)
#endif

/**
 * @brief WR preparation overhead before it can be sent (higher with debugging output).
 *
 * In LWMAC, when a sender wants to send a data packet to the receiver, it starts
 * sending the WR stream a little bit earlier (advance) to the beginning edge
 * of destination's wake-up phase over time. The idea is not to miss the wake-up
 * period of the receiver, otherwise will lead to a long WR procedure.
 */
#ifndef LWMAC_WR_PREPARATION_US
#define LWMAC_WR_PREPARATION_US         ((3U * US_PER_MS))
#endif

/**
 * @brief How long to wait after a WA for data to come in.
 *
 * When a node in LWMAC gets a WR during its wake-up period, it immediately
 * replies a WA packet to the sender for acknowledging the sender's transmission
 * request. After sending the WA, the receiver waits for the data packet from the
 * sender, with a timeout of LWMAC_DATA_DELAY_US duration. In case no data will be
 * received in this period, the receiver regards reception failed and  go back to
 * normal listen mode. However, in case the receiver receives other unintended packets,
 * like WR/WA packets from other neighbor communication pairs, the receiver resets
 * this timeout and continues to wait for the data packet, with the consideration that
 * the sender's data transmission might be delayed due to other ongoing transmissions
 * (the data packet is transmitted with CSMA/CA).
 * This data timeout is enough to catch the beginning of the packet if the transceiver
 * supports RX_STARTED event (this can be important for big packets).
 */
#ifndef LWMAC_DATA_DELAY_US
#define LWMAC_DATA_DELAY_US             (10U * US_PER_MS)
#endif

/**
 * @brief The maximum MAC address length used in LWMAC.
 */
#ifndef LWMAC_MAX_L2_ADDR_LEN
#define LWMAC_MAX_L2_ADDR_LEN           (8U)
#endif

/**
 * @brief CSMA retries for DATA packet after WR->WA was successful.
 *
 * After receiving the WA packet from the receiver, the sender starts sending the
 * data packet using CSMA/CA. This macro defines how many CSMA retries a sender
 * will be allowed to execute for sending its data, before the data is successfully
 * sent (gets data ACK from the receiver).
 */
#ifndef LWMAC_DATA_CSMA_RETRIES
#define LWMAC_DATA_CSMA_RETRIES         (3U)
#endif

/**
 * @brief Maximum TX transmission retries for DATA packet in case of no response from the receiver.
 *
 * When a data packet is scheduled for transmission, i.e., pushed into TX for sending,
 * LWMAC defines a maximum of LWMAC_MAX_DATA_TX_RETRIES retries for transmission of the
 * packet. That is, in case of transmission failure in TX due to no WA from the receiver,
 * the sender will not drop the packet, but keep it and retries to send the data packet
 * in the following cycles, until the sender reaches the maximum retries limit defined here.
 * Then, the packet will be dropped.
 */
#ifndef LWMAC_MAX_DATA_TX_RETRIES
#define LWMAC_MAX_DATA_TX_RETRIES       (3U)
#endif

/**
 * @brief MAX burst transmission packet number in one shot.
 *
 * LWMAC supports burst transmission based on the pending-bit technique, and this macro
 * here defines the largest number of packets allowed to be sent in one consecutive
 * sequence. In case a sender has multi packets for one receiver,the burst transmission
 * procedure is as follow: 1) the sender first uses WR stream to locate the receiver's
 * wake-up period (if the sender has already phase-locked the receiver's phase, normally
 * the sender only cost one WR to get the first WA from the receiver) and then sends its
 * first data. 2) After the transmission of the first data, the sender immediately sends
 * a WR to the receiver for starting the second round of transmission of the second data.
 * The receiver should also immediately reply WA for continue receiving data packets. In
 * case the sender doesn't receive WA during LWMAC_TIME_BETWEEN_WR_US, it regards the
 * consecutive (burst) transmission failed and quits TX procedure (the data will be queued
 * back to the transmission queue for normal transmission attempt in following cycles).
 * 3) In case the second transmission succeeds, the sender repeats step-2 to send all the
 * following pending packets. In short, in burst transmission mode, the sender doesn't
 * tolerate no-WA event. ALl the pending data packets should be sent with only one WR cost for
 * leading the transmission.
 */
#ifndef LWMAC_MAX_TX_BURST_PKT_NUM
#define LWMAC_MAX_TX_BURST_PKT_NUM      (LWMAC_WAKEUP_INTERVAL_US / LWMAC_WAKEUP_DURATION_US)
#endif

/**
 * @brief MAX bad Listen period extensions a node can tolerate.
 *
 * In LWMAC, to allow burst transmissions, when in the wake-up period and by default, a node
 * will extend its wake-up period to another LWMAC_WAKEUP_DURATION_US after each packet
 * reception (except for broadcast packet). However, in some cases, a receiver may
 * overhear other unintended packets, e.g., WR or WA packets for other nodes, these are
 * called bad extensions for the receiver. If a receiver reaches the maximum bad listen
 * extension limit defined here, it goto sleep mode with the consideration that the
 * channel is currently unavailable/busy.
 */
#ifndef LWMAC_MAX_RX_EXTENSION_NUM
#define LWMAC_MAX_RX_EXTENSION_NUM      (3U)
#endif

/**
 * @brief CSMA retries for BROADCAST packet.
 *
 * Currently, each broadcast packet is sent with CSMA/CA for collision avoidance.
 * Too many CSMA retries may lead to running out of destinations wake-up period.
 */
#ifndef LWMAC_BROADCAST_CSMA_RETRIES
#define LWMAC_BROADCAST_CSMA_RETRIES    (3U)
#endif

/**
 * @brief The default message queue size for LWMAC layer
 */
#ifndef LWMAC_IPC_MSG_QUEUE_SIZE
#define LWMAC_IPC_MSG_QUEUE_SIZE        (8U)
#endif


/**
 * @brief Initialize an instance of the LWMAC layer
 *
 * The initialization starts a new thread that connects to the given netdev
 * device and starts a link layer event loop.
 *
 * @param[in] stack         stack for the control thread
 * @param[in] stacksize     size of *stack*
 * @param[in] priority      priority for the thread housing the LWMAC instance
 * @param[in] name          name of the thread housing the LWMAC instance
 * @param[in] dev           netdev device, needs to be already initialized
 *
 * @return                  PID of LWMAC thread on success
 * @return                  -EINVAL if creation of thread fails
 * @return                  -ENODEV if *dev* is invalid
 */
kernel_pid_t gnrc_lwmac_init(char *stack, int stacksize, char priority,
                             const char *name, gnrc_netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_LWMAC_LWMAC_H */
/** @} */
