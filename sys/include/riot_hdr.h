/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_riot_hdr RIOT header helpers and tools
 * @ingroup     sys
 * @{
 *
 * The header contains
 *
 * - "RIOT" as magic number
 * - the application version
 * - the address where to find the RIOT firmware
 * - the checksum of the three previous fields
 *
 * @file
 * @brief       RIOT firmware header and tools
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#ifndef RIOT_HDR_H
#define RIOT_HDR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 *  @brief Number of header bytes that get checksummed
 */
#define RIOT_HDR_CHECKSUM_LEN      (12)

/**
 * @brief Structure to store image header - All members are little endian
 * @{
 */
typedef struct {
    uint32_t magic_number;         /**< header magic_number (always "RIOT")    */
    uint32_t version;              /**< Integer representing firmware version  */
    uint32_t start_addr;           /**< Start address in flash                 */
    uint32_t chksum;               /**< checksum of riot_hdr                   */
} riot_hdr_t;
/** @} */

/**
 * @brief  Print formatted riot_hdr_t to STDIO
 *
 * @param[in] riot_hdr  ptr to image header
 *
 */
void riot_hdr_print(const riot_hdr_t *riot_hdr);

/**
 * @brief  Validate image header
 *
 * @param[in] riot_hdr  ptr to image header
 *
 */
int riot_hdr_validate(const riot_hdr_t *riot_hdr);

/**
 * @brief  Calculate header checksum
 *
 * @param[in] riot_hdr  ptr to image header
 *
 */
uint32_t riot_hdr_checksum(const riot_hdr_t *riot_hdr);

#ifdef __cplusplus
}
#endif

#endif /* RIOT_HDR_H */
