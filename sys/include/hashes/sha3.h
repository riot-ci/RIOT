/*-
 * Copyright 2017-2018 Mathias Tausig
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


/**
 * @ingroup     sys_hashes
 * @{
 *
 * @file
 * @brief       Header definitions for the SHA-3 hash function
 *
 * @author      Mathias Tausig
 */

#ifndef HASHES_SHA3_H
#define HASHES_SHA3_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHA3_256_DIGEST_LENGTH 32
#define SHA3_384_DIGEST_LENGTH 48
#define SHA3_512_DIGEST_LENGTH 64

/**
 * @brief Context for operations on a sponge with keccak permutation
 */
typedef struct {
    /** State of the KEccak sponge **/
    unsigned char state[200];
    /** Current position within the state **/
    unsigned int i;
    /** The suffix used for padding **/
    unsigned char delimitedSuffix;
    /** The bitrate of the sponge **/
    unsigned int rate;
    /** The capacity in bits of the sponge **/
    unsigned int capacity;
    /** The rate in bytes of the sponge **/
    unsigned int rateInBytes;
} keccak_state_t;

/**
 * @brief Initialise a sponge based on a keccak-1600 permutation
 *
 * @param ctx                  context handle to initialise
 * @param[in] rate             the desired rate of the sponge
 * @param[in] capacity         the desired capcity of the sponge
 * @param[in] delimitedSuffix  suffix to be appended to the message after the absorbation phase
 */
void Keccak_init(keccak_state_t *ctx, unsigned int rate, unsigned int capacity, unsigned char delimitedSuffix);

/**
 * @brief Absorbs data into a sponge. Can be called multiple times
 *
 * @param ctx               context handle of the sponge
 * @param[in] input         pointer to the data to be absorbed
 * @param[in] inputByteLen  length of the input data in bytes
 */
void Keccak_update(keccak_state_t *ctx, const unsigned char *input, unsigned long long int inputByteLen);

/**
 * @brief Squeeze data from a sponge
 *
 * @param ctx                context handle of the sponge
 * @param[out] output        the squeezed data
 * @param[in] outputByteLen  size of the data to be squeezed.
 */
void Keccak_final(keccak_state_t *ctx, unsigned char *output, unsigned long long int outputByteLen);

/**
 * @brief SHA-3-256 initialization.  Begins a SHA-3-256 operation.
 *
 * @param ctx  keccak_state_t handle to initialise
 */
void sha3_256_init(keccak_state_t *ctx);

/**
 * @brief Add bytes into the hash
 *
 * @param ctx      context handle to use
 * @param[in] data Input data
 * @param[in] len  Length of @p data
 */
void sha3_update(keccak_state_t *ctx, const void *data, size_t len);

/**
 * @brief SHA-3-256 finalization.  Pads the input data and exports the hash value
 *
 * @param ctx    context handle to use
 * @param digest resulting digest, this is the hash of all the bytes
 */
void sha3_256_final(keccak_state_t *ctx, void *digest);

/**
 * @brief SHA-3-384 initialization.  Begins a SHA-3-256 operation.
 *
 * @param ctx  keccak_state_t handle to initialise
 */
void sha3_384_init(keccak_state_t *ctx);

/**
 * @brief SHA-3-384 finalization.  Pads the input data and exports the hash value
 *
 * @param ctx    context handle to use
 * @param digest resulting digest, this is the hash of all the bytes
 */
void sha3_384_final(keccak_state_t *ctx, void *digest);

/**
 * @brief SHA-3-512 initialization.  Begins a SHA-3-256 operation.
 *
 * @param ctx  keccak_state_t handle to initialise
 */
void sha3_512_init(keccak_state_t *ctx);

/**
 * @brief SHA-3-512 finalization.  Pads the input data and exports the hash value
 *
 * @param ctx    context handle to use
 * @param digest resulting digest, this is the hash of all the bytes
 */

void sha3_512_final(keccak_state_t *ctx, void *digest);

/**
 * @brief A wrapper function to simplify the generation of a hash, this is
 * usefull for generating SHA-3-256 from one buffer
 *
 * @param[in] data     pointer to the buffer to generate hash from
 * @param[in] len      length of the buffer
 * @param[out] digest  pointer to an array for the result, length must
 *                     be SHA3_256_DIGEST_LENGTH
 */
void sha3_256(void *digest, const void *data, size_t len);

/**
 * @brief A wrapper function to simplify the generation of a hash, this is
 * usefull for generating SHA-3-384 from one buffer
 *
 * @param[in] data     pointer to the buffer to generate hash from
 * @param[in] len      length of the buffer
 * @param[out] digest  pointer to an array for the result, length must
 *                     be SHA3_384_DIGEST_LENGTH
 */
void sha3_384(void *digest, const void *data, size_t len);

/**
 * @brief A wrapper function to simplify the generation of a hash, this is
 * usefull for generating SHA-3-512 from one buffer
 *
 * @param[in] data     pointer to the buffer to generate hash from
 * @param[in] len      length of the buffer
 * @param[out] digest  pointer to an array for the result, length must
 *                     be SHA3_512_DIGEST_LENGTH
 */
void sha3_512(void *digest, const void *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif