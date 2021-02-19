/*
 * Copyright (C) 2014-2015 Martine S. Lenders <m.lenders@fu-berlin.de>
 * Copyright (C) 2014-2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_pktbuf
 * @brief   Internal definitions of the static implementation of
 *          @ref net_gnrc_pktbuf
 * @{
 *
 * @file
 * @brief   Definitions of types and their alignment for usage in tests
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef PKTBUF_STATIC_H
#define PKTBUF_STATIC_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GNRC_PKTBUF_STATIC_ALIGN_MASK   (sizeof(_unused_t) - 1)

typedef struct _unused {
    struct _unused *next;
    unsigned int size;
} _unused_t;

/* fits size to byte alignment */
static inline size_t _align(size_t size)
{
    return (size + GNRC_PKTBUF_STATIC_ALIGN_MASK) &
          ~(GNRC_PKTBUF_STATIC_ALIGN_MASK);
}

#ifdef __cplusplus
}
#endif

#endif /* PKTBUF_STATIC_H */
/** @} */
