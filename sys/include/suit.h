/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_suit SUIT manifest parser
 * @ingroup     sys
 * @brief       SUIT manifest parser
 *
 * This is a simple suit manifest parser for RIOT. The high level assumption is
 * that the raw manifest data is stored in a buffered location where raw values
 * or strings can be left during the lifetime of the suit_manifest_t struct.
 * This assumption is valid in the case where gcoap block1 is used to transfer
 * the manifest to the node and an intermediate buffer is necessary to validate
 * the manifest.
 *
 * The parser is based on draft version 1 of the specification.
 *
 * @see https://tools.ietf.org/html/draft-moran-suit-manifest-01
 *
 * @{
 *
 * @brief       Decoding functions for a SUIT manifest
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef SUIT_H
#define SUIT_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "cbor.h"
#include "uuid.h"

#define SUIT_MANIFEST_MIN_LENGTH            9
#define SUIT_MANIFEST_PAYLOADINFO_LENGTH    7

#define SUIT_MANIFEST_IDX_MANIFESTVERSION   0
#define SUIT_MANIFEST_IDX_TEXT              1
#define SUIT_MANIFEST_IDX_NONCE             2
#define SUIT_MANIFEST_IDX_TIMESTAMP         3
#define SUIT_MANIFEST_IDX_CONDITIONS        4
#define SUIT_MANIFEST_IDX_DIRECTIVES        5
#define SUIT_MANIFEST_IDX_ALIASES           6
#define SUIT_MANIFEST_IDX_DEPENDENCIES      7
#define SUIT_MANIFEST_IDX_EXTENSIONS        8
#define SUIT_MANIFEST_IDX_PAYLOADINFO       9

#define SUIT_PAYLOADINFO_IDX_FORMAT         0
#define SUIT_PAYLOADINFO_IDX_SIZE           1
#define SUIT_PAYLOADINFO_IDX_STORID         2
#define SUIT_PAYLOADINFO_IDX_URIS           3
#define SUIT_PAYLOADINFO_IDX_DIGESTALGO     4
#define SUIT_PAYLOADINFO_IDX_DIGESTS        5
#define SUIT_PAYLOADINFO_IDX_PAYLOAD        6

/**
 * @name suit parser error codes
 */
typedef enum {
    SUIT_OK                   = 0,  /**< Manifest parsed and validated */
    SUIT_ERR_INVALID_MANIFEST = -1, /**< Unexpected CBOR structure detected */
    SUIT_ERR_NOT_SUPPORTED    = -2, /**< Unsupported manifest features detected */
    SUIT_ERR_COND             = -3, /**< Conditionals evaluate to false */
} suit_error_t;

/**
 * @name SUIT conditionals
 */
enum {
    SUIT_COND_VENDOR_ID     = 1,
    SUIT_COND_CLASS_ID      = 2,
    SUIT_COND_DEV_ID        = 3,
    SUIT_COND_BEST_BEFORE   = 4,
};

/**
 * @name SUIT payload digest algorithms
 *
 * Unofficial list from
 * [suit-manifest-generator](https://github.com/ARMmbed/suit-manifest-generator)
 */
typedef enum {
    SUIT_DIGEST_NONE    = 0, /**< No digest algo supplied */
    SUIT_DIGEST_SHA256  = 1, /**< SHA256 */
    SUIT_DIGEST_SHA384  = 2, /**< SHA384 */
    SUIT_DIGEST_SHA512  = 3, /**< SHA512 */
} suit_digest_t;

/**
 * @name SUIT payload digest types
 *
 * Unofficial list from
 * [suit-manifest-generator](https://github.com/ARMmbed/suit-manifest-generator)
 */
typedef enum {
    SUIT_DIGEST_TYPE_RAW = 1,
    SUIT_DIGEST_TYPE_INSTALLED = 2,
    SUIT_DIGEST_TYPE_CIPHERTEXT = 3,
    SUIT_DIGEST_TYPE_PREIMAGE = 4
} suit_digest_type_t;

/**
 * @brief SUIT manifest struct
 */
typedef struct {
    uint32_t version;           /**< Manifest Version field */
    uint32_t sequence;          /**< Manifest timestamp */
    uint32_t size;              /**< Manifest payload size */
    const uint8_t *conditions;  /**< ptr to the conditionals in cbor form */
    size_t condition_len;       /**< length of the conditionals */
    const uint8_t *urls;        /**< ptr to the urls in cbor form */
    size_t url_len;             /**< length of the urls */
    const uint8_t *digests;     /**< ptr to the digests */
    size_t digest_len;          /**< length of the digest */
    uint8_t *identifier;        /**< Storage identifier */
    suit_digest_t algo;         /**< Digest algorithm used */
} suit_manifest_t;

/**
 * @brief Initialize the UUID's for conditionals
 */
void suit_uuid_init(void);

const uuid_t* suit_get_uuid_vendor(void);
const uuid_t* suit_get_uuid_class(void);
const uuid_t* suit_get_uuid_device(void);

/**
 * @brief Parse a buffer containing a cbor encoded manifest into a
 * suit_manifest_t struct
 *
 * @param   manifest    manifest to fill
 * @param   buf         Buffer to read from
 * @param   len         Size of the buffer
 * @param   ct          cn-cbor allocator context
 *
 * return               0 on success
 */
int suit_parse(suit_manifest_t *manifest, const uint8_t *buf,
               size_t len);

/**
 * @brief Copy the url from the manifest into the supplied buffer
 *
 * @param   manifest    manifest to retrieve the url from
 * @param   buf         buffer to write into
 * @param   len         size of the buffer
 *
 * @return              length of the url
 * @return              negative on error
 */
ssize_t suit_get_url(const suit_manifest_t *manifest, char *buf, size_t len);

uint32_t suit_get_version(const suit_manifest_t *manifest);

/**
 * @brief Retrieve the sequence number of the manifest
 *
 * @param   manifest    manifest to retrieve from
 *
 * @return              sequence number
 */
uint32_t suit_get_seq_no(const suit_manifest_t *manifest);

int suit_verify_conditions(suit_manifest_t *manifest, uint64_t curtime);

static inline uint32_t suit_payload_get_size(const suit_manifest_t *manifest)
{
    return manifest->size;
}
/**
 * Retrieve a pointer to the requested digest
 *
 */
int suit_payload_get_digest(const suit_manifest_t *manifest,
            suit_digest_type_t digest, uint8_t *buf, size_t *len);

/**
 * Check if a manifest is a valid update compared to the old manifest
 *
 * @param   old     Current manifest to compare
 * @param   new     New manifest to compare
 *
 * @return          True when the new manifest is a newer manifest
 */
static inline bool suit_isnew(const suit_manifest_t *old, const suit_manifest_t *cur)
{
    return (old->sequence < cur->sequence);
}


#ifdef __cplusplus
}
#endif
/** @} */
#endif /* SUIT_H */
