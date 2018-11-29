/*
 * Copyright (C) 2016 Andrew Moon (dedicated to the public domain)
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @defgroup    sys_crypto_poly1305 poly1305
 * @brief       Poly1305 one-time message authentication code
 * @{
 *
 * @file
 * @brief       Poly1305 MAC interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */
#ifndef CRYPTO_POLY1305_H
#define CRYPTO_POLY1305_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define poly1305_block_size 16

/**
 * @brief Poly1305 context
 */
typedef struct {
    uint32_t r[5];                          /**< first key part         */
    uint32_t h[5];                          /**< second key part        */
    uint32_t pad[4];                        /**< pad                    */
    size_t leftover;                        /**< leftover of the buffer */
    uint8_t buffer[poly1305_block_size];    /**< tag buffer             */
    uint8_t final;                          /**< end of data indicator  */
} poly1305_ctx_t;

/**
 * @brief   Initialize a poly1305 context
 *
 * @param   ctx     Poly1305 context
 * @param   key     32 byte key
 */
void poly1305_init(poly1305_ctx_t *ctx, const uint8_t *key);

/**
 * @brief   Update the poly1305 context with a block of message
 *
 * @param   ctx     poly1305 context
 * @param   data    ptr to the message
 * @param   len     length of the message
 */
void poly1305_update(poly1305_ctx_t *ctx, const uint8_t *data, size_t len);

/**
 * @brief   Finish the poly1305 operation
 *
 * @param   ctx     poly1305 context
 * @param   mac     16 byte buffer for the tag
 */
void poly1305_finish(poly1305_ctx_t *ctx, uint8_t *mac);

/**
 * @brief   Calculate a single poly1305 tag
 *
 * @param   mac     16 byte buffer for the tag
 * @param   data    ptr to the message
 * @param   len     length of the message
 * @param   key     32 byte key
 */
void poly1305_auth(uint8_t *mac, const uint8_t *data, size_t len, const uint8_t *key);

int poly1305_verify(const uint8_t *mac1, const uint8_t *mac2);

#ifdef __cplusplus
}
#endif
#endif /* CRYPTO_POLY1305_H */
/** @} */
