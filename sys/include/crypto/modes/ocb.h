/*
 * Copyright (C) 2018 Mathias Tausig
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        ocb.h
 * @brief       Offset Codebook (OCB3) AEAD mode as specified in RFC 7253
 *
 * @author      Mathias Tausig <mathias@tausig.at>
 */

#ifndef CRYPTO_MODES_OCB_H
#define CRYPTO_MODES_OCB_H

#include "crypto/ciphers.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name OCB error codes
 * @{
 */
#define OCB_ERR_INVALID_NONCE_LENGTH        (-2)
#define OCB_ERR_INVALID_BLOCK_LENGTH        (-3)
#define OCB_ERR_INVALID_DATA_LENGTH         (-3)
#define OCB_ERR_INVALID_TAG_LENGTH          (-4)
#define OCB_ERR_INVALID_TAG                 (-5)

/** @} */


int cipher_encrypt_ocb(cipher_t *cipher, uint8_t *auth_data, uint32_t auth_data_len,
                       uint8_t tag_length, uint8_t *nonce, size_t nonce_len,
                       uint8_t *input, size_t input_len, uint8_t *output);

int cipher_decrypt_ocb(cipher_t *cipher, uint8_t *auth_data, uint32_t auth_data_len,
                       uint8_t tag_length, uint8_t *nonce, size_t nonce_len,
                       uint8_t *input, size_t input_len, uint8_t *output);

#endif /* CRYPTO_MODES_OCB_H */
/** @} */
