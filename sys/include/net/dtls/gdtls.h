/*
 * Copyright (C) 2017 Raul Fuentes <raul.fuentes_samaniego@inria.fr>
 *               2017 Inria
 * All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup net_gdtls  DTLS
 * @ingroup  net
 * @ High-level interface to DTLS callbacks.
 *
 *
 * gdtls provides an example of how developing a high/level interface for
 * handling DTLS callbacks via RIOT's sock networking API.
 *
 * This is based on the documentation of pkg/tinydtls/dtls.h
 * @{
 *
 * @file
 * @brief       gdtls definition
 *
 * @author      Raul Fuentes <raul.fuentes_samaniego@inria.fr>
 *
 */

#ifndef _NET_DTLS_H
#define _NET_DTLS_H

# include "net/dtls/dtls_keys.h"

#include "net/sock/udp.h"
#include "net/ipv6/addr.h"
#include "net/af.h"

/* TinyDTLS */
#include "dtls.h"
#include "dtls_debug.h"
#include "tinydtls.h"


#ifdef __cplusplus
extern "C" {
#endif

/** Structure to pass to tinyDTLs for handling (DTLS) peers */
typedef struct {
  sock_udp_t *sock;      /**< The sock created between local and remote */
  sock_udp_ep_t *local;  /**< This peer (used by the client) */
  sock_udp_ep_t *remote; /**< The remote peer */
  int * dtls_connected;  /**< Status of the DTLS channel between the peers */
} dtls_remote_peer_t;

/** To pass between IPC messages for handling Upper layer data to send */
typedef struct {
  uint8_t *buffer;      /**< Ptr to the data to send  */
  size_t  *buffer_size; /**< Ptr to the size of the data */
} dtls_data_app_t;

#ifndef DTLS_DEFAULT_PORT
/** @brief DTLS default port  */
#define DTLS_DEFAULT_PORT    20220
#endif

/* NOTE: Overwrite the threads of upper modules (CoAP, TFTP, etc.) */
/** @brief Stack size for module thread */
#define DTLS_STACKSIZE    (3*THREAD_STACKSIZE_MAIN + THREAD_EXTRA_STACKSIZE_PRINTF)


/**
 *  @brief To avoid infinite loop in the DTLS client side.
 *
 *  Number of DTLS Record *  DTLS_DEFAULT_MAX_RETRANSMIT + Extra
 *  NOTE: DTLS Records are expected to be fragmented (Varies according
 *        to compression). Also, DTLS HelloVerify Request is send by tinyDTLS
 *        too often.
 */
# define GDTLS_MAX_RETRANSMISIONS    18 * DTLS_DEFAULT_MAX_RETRANSMIT + 3

/**
 * @brief Size fore the IPC queue  for the module thread
 *
 * Because of the variable number of DTLS messages to send in the different flights. It's more safe to icnrease the number of threads.
 */
#define DTLS_READER_QUEUE_SIZE    (16U)


/**
 * @brief Custom IPC type msgs for GDTLS.
 * @{
 */
#define DTLS_MSG_CLIENT_HNDSHK    0x4001
#define DTLS_MSG_SERVER_STOP      0x4002
#define DTLS_MSG_CLIENT_START     0x4003
#define DTLS_MSG_CLIENT_DATA      0x4004
#define DTLS_MSG_SERVER_RESTART   0x4005

#define DTLS_MSG_CLIENT_START_CHANNEL    0x5001
#define DTLS_MSG_CLIENT_STOP_CHANNEL     0x5002
#define DTLS_MSG_CLIENT_SEND_DATA        0x5003
#define DTLS_MSG_CLIENT_ANSWER           0x5000
/** @} */

/**
 * @brief Flags for handling listening states of GCOAP and GDTLS.
 * @{
 */
#define DTLS_FLAGS_SERVER_LISTEN           0x01
#define DTLS_FLAGS_CLIENT_LISTEN_ONLY      0x02
#define DTLS_FLAGS_SERVER_CLIENT_LISTEN    0x03
#define DTLS_FLAGS_CLIENT_NON_LISTEN       0xFF
/** @} */

/** @brief dtls_remote_peer_t default initialization. */
#define DTLS_REMOTE_PEER_NULL    {.sock=NULL, .local=NULL, .remote=NULL }
/** @brief dtls_data_app_t default initialization. */
#define DTLS_DATA_APP_NULL       {.buffer=NULL, .buffer_size= NULL}

/**
 * @brief Send a DTLS Applicaiton data record
 *
 * @param[in] ctx      DTLS context
 * @param[in] dst      DTLS session between DTLS peers
 * @param[in] buf      Data to send
 * @param[in] len      Size of the data to send
 *
 * @return   >= 0 on success
 * @return   < 0 on error
 */
int try_send(struct dtls_context_t *ctx, session_t *dst, uint8 *buf, size_t len);

/**
 * @brief Handles the reception of the DTLS flights.
 *
 * Handles all the packets arriving at the node and identifies those that are
 * DTLS records. Also, it determines if said DTLS record is coming from a new
 * peer or a currently established peer.
 *
 * @param[in] ctx         DTLS context
 * @param[in] packet      Packet receieved
 * @param[in] size        Size of the Packet receieved
 */
void dtls_handle_read_sock(dtls_context_t *ctx, uint8_t *packet,
                                                                  size_t size);


/** @brief initialize tinyDTLS and set the syslog for it */
void gnrc_dtls_init(void);

#ifdef DTLS_PSK


/**
 * This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session.
 */
int _client_peer_get_psk_info(struct dtls_context_t *ctx,
                        const session_t *session,
                        dtls_credentials_type_t type,
                        const unsigned char *id, size_t id_len,
                        unsigned char *result, size_t result_length);


int _server_peer_get_psk_info(struct dtls_context_t *ctx,
                       const session_t *session,
                       dtls_credentials_type_t type,
                       const unsigned char *id, size_t id_len,
                       unsigned char *result, size_t result_length);
#endif /* DTLS_PSK */

/**
 * @brief Handles the DTLS communication with the other peer.
 */
int _send_to_peer(struct dtls_context_t *ctx,
                        session_t *session, uint8 *buf, size_t len);

#ifdef DTLS_ECC

/* NOTE: The client and server sides are identically for ECC */

int _peer_get_ecdsa_key(struct dtls_context_t *ctx,
                                      const session_t *session,
                                      const dtls_ecdsa_key_t **result);

int _peer_verify_ecdsa_key(struct dtls_context_t *ctx,
                           const session_t *session,
                           const unsigned char *other_pub_x,
                           const unsigned char *other_pub_y,
                           size_t key_size);

#endif

/**
* @brief Reception of a DTLS Applicaiton data record.
*/
int _read_from_peer(struct dtls_context_t *ctx,
                                  session_t *session, uint8 *data, size_t len);

/**
 * @brief TinyDTLS callback for detecting the state of the DTLS channel.
 *
 * Ecxlsuive of the TinyDTLS Client (For now).
 *
 * @note Once renegotiation is included this is also affected.
 */
int _client_events(struct dtls_context_t *ctx,
                   session_t *session ,
                   dtls_alert_level_t level ,
                  unsigned short code);

#ifdef __cplusplus
 }
#endif

#endif /* _NET_DTLS_H */
/** @} */
