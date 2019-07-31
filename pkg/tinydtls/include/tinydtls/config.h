/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    tinydtls_config Tinydtls configuration header
 * @ingroup     pkg_tinydtls config
 * @brief       Provides compile-time configuration for tinydtls
 *
 * @{
 *
 * @file
 * @brief       TinyDTLS configuration
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#ifndef TINYDTLS_CONFIG_H
#define TINYDTLS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The maximum number of DTLS context at the same time
 */
#ifndef DTLS_CONTEXT_MAX
#define DTLS_CONTEXT_MAX    (2)
#endif

/**
 * @brief The maximum number DTLS peers (i.e. sessions)
 */
#ifndef DTLS_PEER_MAX
#define DTLS_PEER_MAX       (1)
#endif

/**
 * @brief The maximum number of concurrent DTLS handshakes
 */
#ifndef DTLS_HANDSHAKE_MAX
#define DTLS_HANDSHAKE_MAX  (2)
#endif

/**
 * @brief The maximum number of concurrently used cipher keys
 */
#ifndef DTLS_SECURITY_MAX
#define DTLS_SECURITY_MAX   (DTLS_HANDSHAKE_MAX + DTLS_PEER_MAX)
#endif

/**
 * @brief The maximum number of hash functions that can be used in parallel
 */
#ifndef DTLS_HASH_MAX
#define DTLS_HASH_MAX       (3 * DTLS_PEER_MAX)
#endif

/**
 * @brief The number of retransmissions of any DTLS record
 *
 * The 802.15.4 ACK can provoke very fast re-transmissions with a value
 * higher than one. This is a temporary bad behavior for the RIOT MAC
 */
#ifndef DTLS_DEFAULT_MAX_RETRANSMIT
#define DTLS_DEFAULT_MAX_RETRANSMIT     (1)
#endif

#ifdef __cplusplus
}
#endif

#endif /* TINYDTLS_CONFIG_H */
/** @} */
