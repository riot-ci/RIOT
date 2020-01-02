/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_tcp TCP
 * @ingroup     net_gnrc
 * @brief       RIOT's TCP implementation for the GNRC network stack.
 *
 * @{
 *
 * @file
 * @brief       GNRC TCP API
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef NET_GNRC_TCP_H
#define NET_GNRC_TCP_H

#include <stdint.h>
#include "net/gnrc/pkt.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Address information for a single TCP connection endpoint.
 * @note Must be compatible to sock_tcp_ep_t.
 */
typedef struct {
    int family; /**< IP Address family. GNRC supports currently only IPv6. */

    union {
#ifdef MODULE_GNRC_IPV6
        uint8_t ipv6[sizeof(ipv6_addr_t)]; /**< IPv6 Address storage */
#endif
    } addr;

    uint16_t netif; /**< Network interface ID */
    uint16_t port;  /**< Port number (in host byte order) */
} gnrc_tcp_ep_t;


/**
 * @brief Initialize TCP connection endpoint.
 *
 * @param[in,out] ep               Endpoint to initialize.
 * @param[in]     address_family   Address family of @p address.
 * @param[in]     address          Address information for endpoint.
 * @param[in]     port             Port number for endpoint.
 *
 * @returns   0 on success.
 *            -EAFNOSUPPORT if @p address_family is not supported.
 *            -EINVAL if parsing of @p address failed.
 */
int gnrc_tcp_ep_init(gnrc_tcp_ep_t *ep, int address_family, char *address, uint16_t port);

/**
 * @brief Initialize TCP
 *
 * @returns   PID of TCP thread on success
 *            -1 if TCB is already running.
 *            -EINVAL, if priority is greater than or equal SCHED_PRIO_LEVELS
 *            -EOVERFLOW, if there are too many threads running.
 */
int gnrc_tcp_init(void);

/**
 * @brief Initialize Transmission Control Block (TCB)
 * @pre @p tcb must not be NULL.
 *
 * @param[in,out] tcb   TCB that should be initialized.
 */
void gnrc_tcp_tcb_init(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Opens a connection actively.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL
 * @pre @p remote must not be NULL.
 *
 * @note Blocks until a connection has been established or an error occurred.
 *
 * @param[in,out] tcb          TCB holding the connection information.
 * @param[in]     remote       Remote endpoint of the host to connect to.
 * @param[in]     local_port   If zero or PORT_UNSPEC, the connections source port
 *                             is randomly chosen. If local_port is non-zero
 *                             the local_port is used as source port.
 *
 * @returns   0 on success.
 *            -EAFNOSUPPORT if @p address_family is not supported.
 *            -EINVAL if @p address_family is not the same the address_family use by the TCB.
 *                    or @p target_addr is invalid.
 *            -EISCONN if TCB is already in use.
 *            -ENOMEM if the receive buffer for the TCB could not be allocated.
 *            -EADDRINUSE if @p local_port is already used by another connection.
 *            -ETIMEDOUT if the connection could not be opened.
 *            -ECONNREFUSED if the connection was reset by the peer.
 */
int gnrc_tcp_open_active(gnrc_tcp_tcb_t *tcb, const gnrc_tcp_ep_t *remote,
                         uint16_t local_port);

/**
 * @brief Opens a connection passively, by waiting for an incoming request.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL.
 * @pre @p local must not be NULL.
 * @pre port in @p local must not be zero.
 *
 * @note Blocks until a connection has been established (incoming connection request
 *       to @p local_port) or an error occurred.
 *
 * @param[in,out] tcb     TCB holding the connection information.
 * @param[in]     local   Endpoint specifying the port and address used to wait for
 *                        incomming connections.
 *
 * @returns   0 on success.
 *            -EAFNOSUPPORT if local_addr != NULL and @p address_family is not supported.
 *            -EINVAL if @p address_family is not the same the address_family used in TCB.
 *                    or the address in @p local is invalid.
 *            -EISCONN if TCB is already in use.
 *            -ENOMEM if the receive buffer for the TCB could not be allocated.
 *            Hint: Increase "GNRC_TCP_RCV_BUFFERS".
 */
int gnrc_tcp_open_passive(gnrc_tcp_tcb_t *tcb, const gnrc_tcp_ep_t *local);

/**
 * @brief Transmit data to connected peer.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL.
 * @pre @p data must not be NULL.
 *
 * @note Blocks until up to @p len bytes were transmitted or an error occurred.
 *
 * @param[in,out] tcb                        TCB holding the connection information.
 * @param[in]     data                       Pointer to the data that should be transmitted.
 * @param[in]     len                        Number of bytes that should be transmitted.
 * @param[in]     user_timeout_duration_us   If not zero and there was not data transmitted
 *                                           the function returns after user_timeout_duration_us.
 *                                           If zero, no timeout will be triggered.
 *
 * @returns   The number of successfully transmitted bytes.
 *            -ENOTCONN if connection is not established.
 *            -ECONNRESET if connection was reset by the peer.
 *            -ECONNABORTED if the connection was aborted.
 *            -ETIMEDOUT if @p user_timeout_duration_us expired.
 */
ssize_t gnrc_tcp_send(gnrc_tcp_tcb_t *tcb, const void *data, const size_t len,
                      const uint32_t user_timeout_duration_us);

/**
 * @brief Receive Data from the peer.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL.
 * @pre @p data must not be NULL.
 *
 * @note Function blocks if user_timeout_duration_us is not zero.
 *
 * @param[in,out] tcb                        TCB holding the connection information.
 * @param[out]    data                       Pointer to the buffer where the received data
 *                                           should be copied into.
 * @param[in]     max_len                    Maximum amount to bytes that should be read
 *                                           into @p data.
 * @param[in]     user_timeout_duration_us   Timeout for receive in microseconds.
 *                                           If zero and no data is available, the function
 *                                           returns immediately. If not zero the function
 *                                           blocks until data is available or
 *                                           @p user_timeout_duration_us microseconds passed.
 *
 * @returns   The number of bytes read into @p data.
 *            0, if the connection is closing and no further data can be read.
 *            -ENOTCONN if connection is not established.
 *            -EAGAIN if  user_timeout_duration_us is zero and no data is available.
 *            -ECONNRESET if connection was reset by the peer.
 *            -ECONNABORTED if the connection was aborted.
 *            -ETIMEDOUT if @p user_timeout_duration_us expired.
 */
ssize_t gnrc_tcp_recv(gnrc_tcp_tcb_t *tcb, void *data, const size_t max_len,
                      const uint32_t user_timeout_duration_us);

/**
 * @brief Close a TCP connection.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 */
void gnrc_tcp_close(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Abort a TCP connection.
 *
 * @pre gnrc_tcp_tcb_init() must have been successfully called.
 * @pre @p tcb must not be NULL.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 */
void gnrc_tcp_abort(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Calculate and set checksum in TCP header.
 *
 * @param[in] hdr          Gnrc_pktsnip that contains TCP header.
 * @param[in] pseudo_hdr   Gnrc_pktsnip that contains network layer header.
 *
 * @returns   0 on success.
 *            -EFAULT if @p hdr or pseudo_hdr were NULL
 *            -EBADMSG if @p hdr is not of type GNRC_NETTYPE_TCP
 *            -ENOENT if @p pseudo_hdr protocol is unsupported.
 */
int gnrc_tcp_calc_csum(const gnrc_pktsnip_t *hdr, const gnrc_pktsnip_t *pseudo_hdr);

/**
 * @brief Adds a TCP header to a given payload.
 *
 * @param[in] payload   Payload that follows the TCP header.
 * @param[in] src       Source port number.
 * @param[in] dst       Destination port number.
 *
 * @returns   Not NULL on success.
 *            NULL if TCP header was not allocated.
 */
gnrc_pktsnip_t *gnrc_tcp_hdr_build(gnrc_pktsnip_t *payload, uint16_t src, uint16_t dst);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_TCP_H */
/** @} */
