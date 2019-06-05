/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_credman (D)TLS Credential Manager
 * @ingroup     net
 * @brief       Credentials management module for (D)TLS
 *
 * @{
 *
 * @file
 * @brief       (D)TLS credentials management module definitions
 *
 * @note        This module DOES NOT copy the credentials into the system. It
 *              just holds the pointers to the credentials given by the user.
 *              The user must make sure that these pointers are valid during the
 *              lifetime of the application.
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#ifndef NET_CREDMAN_H
#define NET_CREDMAN_H

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum number of credentials in system buffer
 */
#ifndef CREDMAN_MAX_CREDENTIALS
#define CREDMAN_MAX_CREDENTIALS  (2)
#endif

/**
 * @brief Buffer to the credential
 */
typedef struct {
    void *s;                /**< Pointer to the buffer */
    size_t len;             /**< Length of s */
} credman_buffer_t;

/**
 * @brief PSK parameters
 */
typedef struct {
    credman_buffer_t key;   /**< Key buffer */
    credman_buffer_t id;    /**< Id buffer */
    credman_buffer_t hint;  /**< Hint buffer */
} psk_params_t;

/**
 * @brief ECDSA public keys
 */
typedef struct {
    const void *x;          /**< X part of the public key */
    const void *y;          /**< Y part of the public key */
} ecdsa_public_key_t;

/**
 * @brief ECDSA parameters
 */
typedef struct {
    const void * private_key;           /**< Pointer to the private key */
    ecdsa_public_key_t public_key;      /**< Public key */
    ecdsa_public_key_t *client_keys;    /**< Array of clients public keys */
    size_t client_keys_size;            /**< Size of clients_keys */
} ecdsa_params_t;

/**
 * @brief Tag of the credential. Must be bigger than 0.
 */
typedef unsigned credman_tag_t;

/**
 * @brief Used to identify credentials for application libraries.
 */
#define CREDMAN_TAG_GLOBAL  (999)

/**
 * @brief Used to signal empty/no tag
 */
#define CREDMAN_TAG_EMPTY   (0)

/**
 * @brief Credential types
 */
typedef enum {
    CREDMAN_TYPE_EMPTY  = 0,
    CREDMAN_TYPE_PSK    = 1,
    CREDMAN_TYPE_ECDSA  = 2,
} credman_type_t;

/**
 * @brief Credential informations
 */
typedef struct {
    credman_type_t type;        /**< Type of the credential */
    credman_tag_t tag;          /**< Tag of the credential */
    union {
        psk_params_t *psk;      /**< PSK credential parameters */
        ecdsa_params_t *ecdsa;  /**< ECDSA credential parameters */
    } params;                   /**< Credential parameters */
} credman_credential_t;

/**
 * @brief Return values
 */
enum {
    CREDMAN_OK          = 0,
    CREDMAN_EXIST       = -1,
    CREDMAN_NO_SPACE    = -2,
    CREDMAN_NOT_FOUND   = -3,
    CREDMAN_ERROR       = -4,
};

/**
 * @brief Initialize system credentials pool. Called from autoinit.
 */
void credman_init(void);

/**
 * @brief Add a credential to system
 *
 * @param[in] credential    Credential to add.
 *
 * @return CREDMAN_OK on success
 * @return CREDMAN_EXIST if credential of @p tag and @p type already exist
 * @return CREDMAN_NO_SPACE if system buffer full
 * @return CREDMAN_ERROR on other errors
 */
int credman_add_credential(const credman_credential_t *credential);

/**
 * @brief Get a credential from system
 *
 * @param[out] credential   Found credential
 * @param[in] tag           Tag of credential to get
 * @param[in] type          Type of credential to get
 *
 * @return CREDMAN_OK on success
 * @return CREDMAN_NOT_FOUND if no credential with @p tag and @p type found
 */
int credman_get_credential(credman_credential_t *credential,
                           credman_tag_t tag, credman_type_t type);

/**
 * @brief Delete a credential from the system
 *
 * @param[in] tag           Tag of the credential
 * @param[in] type          Type of the credential
 *
 * @return CREDMAN_OK on success
 * @return CREDMAN_NOT_FOUND if no credential with @p tag and @p type found
 */
int credman_delete_credential(credman_tag_t tag, credman_type_t type);

/**
 * @brief Get number of credentials added to system
 *
 * Maximum number of allowed credentials is defined by CREDMAN_MAX_CREDENTIALS
 *
 * @return number of credentials added to system
 */
int credman_get_used_count(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_CREDMAN_H */
/** @} */
