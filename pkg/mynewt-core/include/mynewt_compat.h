/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       Undefines macros of clang on mynewt-OS to use RIOT's macros
 *
 */

#ifndef MYNEWT_COMPAT_H
#define MYNEWT_H

#ifdef __cplusplus
extern "C" {
#endif

#undef htons
#undef htonl
#undef htonll
#undef ntohs
#undef ntohl
#undef ntohll

#ifdef __cplusplus
}
#endif

#endif /* MYNEWT_COMPAT_H */
