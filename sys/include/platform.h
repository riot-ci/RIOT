/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_platform    Platform-independent access to platform details
 * @ingroup     sys
 *
 * This module provides platform-independent access to platform details.
 *
 * @{
 *
 * @file
 * @brief       Platform-independent access to platform details
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#include "platform_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Provide doxygen doc centrally, instead of in every platform_arch.h */
#ifdef DOXYGEN
/**
 * @brief   Size of a word in bits
 *
 * @details Depending on platform, this can have a value of 8, 16, or 32
 */
#define PLATFORM_WORD_BITS
#endif /* DOXYGEN */

#if (PLATFORM_WORD_BITS == 8) || defined(DOXYGEN)
/**
 * @brief   Size of a word in bytes
 *
 * @details Depending on platform, this can have a value or 1, 2, or 4.
 */
#define PLATFORM_WORD_BYTES     (1U)
/**
 * @brief   Word sized unsigned integer
 *
 * @details Synonym to `uint8_t`, `uint16_t` or `uint32_t` depending on platform
 */
typedef uint8_t     uword_t;
/**
 * @brief   Word sized signed integer
 *
 * @details Synonym to `int8_t`, `int16_t` or `int32_t` depending on platform
 */
typedef int8_t      sword_t;
#elif (PLATFORM_WORD_BITS == 16)
#define PLATFORM_WORD_BYTES     (2U)
typedef uint16_t    uword_t;
typedef int16_t     sword_t;
#elif (PLATFORM_WORD_BITS == 32)
#define PLATFORM_WORD_BYTES     (4U)
typedef uint32_t    uword_t;
typedef int32_t     sword_t;
#else
#error  "Unsupported word size (check PLATFORM_WORD_BITS in platform_arch.h)"
#endif

/**
 * @brief   Type qualifier to use to align data on word boundaries
 *
 * Use like this:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * char WORD_ALIGNED thread_stack[THREAD_STACKSIZE_DEFAULT];
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define WORD_ALIGNED __attribute__((aligned(PLATFORM_WORD_BYTES)))

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_H */
/** @} */
