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
 * # RIOT image header for riotboot bootloader
 *
 * ## Overview
 *
 * riotboot is the name of a minimal bootloader application and infrastructure.
 * It consists of
 *
 * - the application "riotboot" in the bootloaders folder which serves as
 *   minimal bootloader
 *
 * - the module "riot_hdr" used to recognise RIOT firmware images
 *   which riotboot can boot.
 *
 * - the module "slot_util" used to manage the images (slots) with a
 *   RIOT header attached to them
 *
 * - a tool in dist/tools/riot_hdr for header generation.
 *
 * - several make targets to glue everything together
 *
 * ## Concept
 *
 * riotboot expects the flash to be separated in slots: slot 0 contains
 * the bootloader and slot 1 a firmware image.
 *
 * A RIOT image with a single slot looks like:
 *
 * |------------------------------- FLASH -------------------------------------|
 * |----- RIOTBOOT_LEN ----|----------- RIOTBOOT_SLOT_SIZE (slot 0) -----------|
 *                         |---- RIOT_HDR_SIZE ------|
 *  ---------------------------------------------------------------------------
 * |       bootloader      | riot_hdr_t + filler (0) |     RIOT firmware       |
 *  ---------------------------------------------------------------------------
 *
 * Please note that `RIOT_HDR_SIZE` depends on the architecture of the
 * MCU, since it needs to be aligned to either 256B or 512B. This is
 * fixed regardless of `sizeof(riot_hdr_t)`
 *
 * The bootloader will, on reset, verify the checksum of the first slot, then
 * boot it. If the slot doesn't have a valid checksum, no image will be
 * booted and the bootloader will enter "while(1);".
 *
 * The header contains
 *
 * - "RIOT" as magic number
 * - an application version
 * - a checksum
 *
 * The bootloader "riotboot" only cares about checksum.
 *
 * @file
 * @brief       RIOT images header and tools
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
void riot_hdr_print(riot_hdr_t *riot_hdr);

/**
 * @brief  Validate image header
 *
 * @param[in] riot_hdr  ptr to image header
 *
 */
int riot_hdr_validate(riot_hdr_t *riot_hdr);

/**
 * @brief  Calculate header checksum
 *
 * @param[in] riot_hdr  ptr to image header
 *
 */
uint32_t riot_hdr_checksum(riot_hdr_t *riot_hdr);

#ifdef __cplusplus
}
#endif

#endif /* RIOT_HDR_H */
