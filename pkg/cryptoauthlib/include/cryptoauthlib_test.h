/*
 * Copyright (C) 2019 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_cryptoauthlib cryptoauthlib security crypto
 * @{
 *
 * @file
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#ifndef CRYPTOAUTHLIB_TEST_H
#define CRYPTOAUTHLIB_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Helper function to use the library's unittests
 * @brief   This function is defined in the cryptoauth library via patch.
 *          It is used to pass commands to run built-in unit tests of the library.
 */
int run_cmd(const char *command);


#ifdef __cplusplus
}
#endif

#endif /* CRYPTOAUTHLIB_TEST_H */
/** @} */
