/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Test cases for the SHA256PRNG pseudo random number generator
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#ifndef TESTS_PRNG_SHA256PRNG_H
#define TESTS_PRNG_SHA256PRNG_H

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_prng_sha256prng(void);

/**
 * @brief   Generates tests for SHA256PRNG
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_prng_sha256prng_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_PRNG_SHA256PRNG_H */
/** @} */
