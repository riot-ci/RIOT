/**
 * @ingroup pkg_mbedtls
 * @file
 *
 * @brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

/**
 * @def MBEDTLS_SHA256_ALT
 *
 * MBEDTLS__MODULE_NAME__ALT: Uncomment a macro to let mbed TLS use your
 * alternate core implementation of a symmetric crypto, an arithmetic or hash
 * module (e.g. platform specific assembly optimized implementations). Keep
 * in mind that the function prototypes should remain the same.
 *
 * This replaces the whole module. If you only want to replace one of the
 * functions, use one of the MBEDTLS__FUNCTION_NAME__ALT flags.
 *
 * Example: In case you uncomment MBEDTLS_SHA256_ALT, mbed TLS will no longer
 * provide the "struct mbedtls_aes_context" definition and omit the base
 * function declarations and implementations. "aes_alt.h" will be included from
 * "aes.h" to include the new function definitions.
 *
 * Uncomment a macro to enable alternate implementation of the corresponding
 * module.
 *
 * @warning   MD2, MD4, MD5, ARC4, DES and SHA-1 are considered weak and their
 *            use constitutes a security risk. If possible, we recommend
 *            avoiding dependencies on them, and considering stronger message
 *            digests and ciphers instead.
 *
 */
#define MBEDTLS_SHA256_ALT 1

/**
 * @def MBEDTLS_ENTROPY_HARDWARE_ALT
 *
 * Uncomment this macro to let mbed TLS use your own implementation of a
 * hardware entropy collector.
 *
 * Your function must be called \c mbedtls_hardware_poll(), have the same
 * prototype as declared in entropy_poll.h, and accept NULL as first argument.
 *
 * Uncomment to use your own hardware entropy collector.
 */
#define MBEDTLS_ENTROPY_HARDWARE_ALT 1

/**
 * @def MBEDTLS_NO_PLATFORM_ENTROPY
 *
 * Do not use built-in platform entropy functions.
 * This is useful if your platform does not support
 * standards like the /dev/urandom or Windows CryptoAPI.
 *
 * Uncomment this macro to disable the built-in platform entropy functions.
 */
#define MBEDTLS_NO_PLATFORM_ENTROPY 1

/**
 * @def MBEDTLS_ENTROPY_FORCE_SHA256
 *
 * Force the entropy accumulator to use a SHA-256 accumulator instead of the
 * default SHA-512 based one (if both are available).
 *
 * Requires: MBEDTLS_SHA256_C
 *
 * On 32-bit systems SHA-256 can be much faster than SHA-512. Use this option
 * if you have performance concerns.
 *
 * This option is only useful if both MBEDTLS_SHA256_C and
 * MBEDTLS_SHA512_C are defined. Otherwise the available hash module is used.
 */
#define MBEDTLS_ENTROPY_FORCE_SHA256 1

/**
 * @def MBEDTLS_SELF_TEST
 *
 * Enable the checkup functions (*_self_test).
 */
#define MBEDTLS_SELF_TEST 1

/**
 * @def MBEDTLS_ENTROPY_C
 *
 * Enable the platform-specific entropy code.
 *
 * Module:  mbedtls/library/entropy.c
 * Caller:
 *
 * Requires: MBEDTLS_SHA512_C or MBEDTLS_SHA256_C
 *
 * This module provides a generic entropy pool
 */
#define MBEDTLS_ENTROPY_C

/**
 * @def MBEDTLS_SHA256_C
 *
 * Enable the SHA-224 and SHA-256 cryptographic hash algorithms.
 *
 * Module:  mbedtls/library/sha256.c
 * Caller:  mbedtls/library/entropy.c
 *          mbedtls/library/md.c
 *          mbedtls/library/ssl_cli.c
 *          mbedtls/library/ssl_srv.c
 *          mbedtls/library/ssl_tls.c
 *
 * This module adds support for SHA-224 and SHA-256.
 * This module is required for the SSL/TLS 1.2 PRF function.
 */
#define MBEDTLS_SHA256_C 1

/* Target and application specific configurations
 *
 * Allow user to override any previous default.
 *
 */
#if defined(MBEDTLS_USER_CONFIG_FILE)
#include MBEDTLS_USER_CONFIG_FILE
#endif

#if defined(MBEDTLS_PSA_CRYPTO_CONFIG)
#include "mbedtls/config_psa.h"
#endif

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
