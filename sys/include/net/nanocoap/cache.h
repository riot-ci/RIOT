/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_nanocoap_cache Nanocoap-Cache implementation
 * @ingroup     net_nanocoap
 * @brief       A cache implementation for nanocoap response messages
 *
 * @{
 *
 * @file
 * @brief       nanocoap-cache API
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#ifndef NET_NANOCOAP_CACHE_H
#define NET_NANOCOAP_CACHE_H

#include <assert.h>
#include <stdint.h>
#include "clist.h"
#include "net/nanocoap.h"
#include "hashes/sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The number of maximum cache entries.
 */
#ifndef NANOCOAP_CACHE_ENTRIES
#define NANOCOAP_CACHE_ENTRIES            (8)
#endif

/**
 * @brief The length of the cache key in bytes.
 */
#ifndef NANOCOAP_CACHE_KEY_LENGTH
#define NANOCOAP_CACHE_KEY_LENGTH         (8)
#endif

/**
 * @brief Size of the buffer to store responses in the cache.
 */
#ifndef NANOCOAP_CACHE_RESPONSE_SIZE
#define NANOCOAP_CACHE_RESPONSE_SIZE      (128)
#endif

/**
 * @brief   Cache container that holds a @p coap_pkt_t struct.
 */
typedef struct {
    /**
     * @brief needed for clist_t, must be the first struct member!
     */
    clist_node_t node;

    /**
     * @brief the calculated cache key, see nanocoap_cache_key_generate().
     */
    uint8_t cache_key[NANOCOAP_CACHE_KEY_LENGTH];

    /**
     * @brief packet representation of the response
     */
    coap_pkt_t response_pkt;

    /**
     * @brief buffer to hold the response message.
     */
    uint8_t response_buf[NANOCOAP_CACHE_RESPONSE_SIZE];

    size_t response_len; /**< length of the message in @p response */

    unsigned request_method; /**< the method of the initial request */

    /**
     * @brief absolute system time (us) that records last access to
     * this cache entry (insertion and lookup).
     */
    uint64_t access_time;

    /**
     * @brief absolute system time (us) until which this cache entry
     * is considered valid.
     */
    uint64_t max_age;
} nanocoap_cache_entry_t;

typedef int (*nanocoap_cache_replacement_strategy_t)(void);

/**
 * @brief   Initializes the internal state of the nanocoap cache.
 */
#if IS_USED(MODULE_NANOCOAP_CACHE)
void nanocoap_cache_init(void);
#else
static inline void nanocoap_cache_init(void)
{
    return;
}
#endif

/**
 * @brief   Returns the number of cached entries.
 *
 * @return  Number of cached entries
 */
size_t nanocoap_cache_used_count(void);

/**
 * @brief   Returns the number of unused cache entries.
 *
 * @return  Number of unused cache entries
 */
size_t nanocoap_cache_free_count(void);

/**
 * @brief   Determines if a response is cacheable and modifies the cache
 *          as reflected in RFC7252, Section 5.9.

 * @param[in] cache_key       The cache key of the request
 * @param[in] request_method  The method of the initial request
 * @param[in] resp            The response to operate on
 * @param[in] resp_len        The actual length of the response in @p resp
 *
 * @return  0 on successfully handling the response
 * @return  -1 on error
 */
int nanocoap_cache_process(const uint8_t *cache_key, unsigned request_method,
                           const coap_pkt_t *resp, size_t resp_len);
/**
 * @brief   Creates a new or gets an existing cache entry using the
 *          request packet.
 *
 * @param[in] req           The request to calculate the cache-key
 * @param[in] resp          The response to add to the cache
 * @param[in] resp_len      The actual length of the response message in @p resp
 *
 * @return  The previously existing or newly added cache entry on success
 * @return  NULL, if there is no space left
 */
nanocoap_cache_entry_t *nanocoap_cache_add_by_req(const coap_pkt_t *req,
                                                  const coap_pkt_t *resp,
                                                  size_t resp_len);

/**
 * @brief   Creates a new or gets an existing cache entry using the cache key.
 *
 * @param[in] cache_key       The cache key of the request
 * @param[in] request_method  The method of the initial request
 * @param[in] resp            The response to add to the cache
 * @param[in] resp_len        The actual length of the response in @p resp
 *
 * @return  The previously existing or newly added cache entry on success
 * @return  NULL, if there is no space left
 */
nanocoap_cache_entry_t *nanocoap_cache_add_by_key(const uint8_t *cache_key,
                                                  unsigned request_method,
                                                  const coap_pkt_t *resp,
                                                  size_t resp_len);

/**
 * @brief   Performs a cache lookup based on the @p req.
 *
 * @param[in] req           The request to calculate the cache-key
 *
 * @return  An existing cache entry on cache hit
 * @return  NULL on cache miss
 */
nanocoap_cache_entry_t *nanocoap_cache_request_lookup(const coap_pkt_t *req);

/**
 * @brief   Performs a cache lookup based on the cache key of a request.
 *
 * @param[in] cache_key       The cache key of a request
 *
 * @return  An existing cache entry on cache hit
 * @return  NULL on cache miss
 */
nanocoap_cache_entry_t *nanocoap_cache_key_lookup(const uint8_t *key);

/**
 * @brief   Deletes the provided cache entry @p ce.
 *
 * @param[in] ce            The cache entry to delete
 *
 * @return  0 on success
 * @return  -1 if entry is not available in the cache
 */
int nanocoap_cache_del(const nanocoap_cache_entry_t *ce);

/**
 * @brief   Generates a cache key based on the request @p req.
 *
 * @param[in] req           The request to generate the cache key from
 * @param[out] cache_key    The generated cache key
 */
void nanocoap_cache_key_generate(const coap_pkt_t *req, uint8_t *cache_key);

/**
 * @brief   Compares two cache keys.
 *
 * @param[in] cache_key1    The first cache key in the comparison
 * @param[in] cache_key2    The second cache key in the comparison
 *
 * @return  0 if cache keys are equal
 * @return  <0 or 0> (see memcmp()) for unequal cache keys
 */
ssize_t nanocoap_cache_key_compare(uint8_t *cache_key1, uint8_t *cache_key2);

#ifdef __cplusplus
}
#endif
#endif /* NET_NANOCOAP_CACHE_H */
/** @} */
