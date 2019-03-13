/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 * @{
 *
 * @file
 * @brief       nanocoap private API
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef NET_NANOCOAP_PRIVATE_H
#define NET_NANOCOAP_PRIVATE_H

#include <assert.h>
#include <string.h>

#include "net/nanocoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Checks if a CoAP resource path matches a given URI
 *
 * @param[in] resource CoAP resource to check
 * @param[in] uri String containing the URI to compare
 *
 * @return 0 if resource path matches the URI
 * @return <0 if the resource path is less than the URI
 * @return >0 if the URI is les than the resource path
 */
int coap_match_path(const coap_resource_t *resource, uint8_t *uri);

#ifdef __cplusplus
}
#endif

#endif /* NET_NANOCOAP_PRIVATE_H */
/** @} */
