/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_checksum_crc8     CRC-8
 * @ingroup     sys_checksum
 * @brief       CRC-8 checksum algorithms
 *
 * @{
 *
 * @file
 * @brief   CRC-8 definitions
 *
 * @author  Gunar Schorcht <gunar@schorcht.net>
 */
#ifndef CHECKSUM_CRC8_H
#define CHECKSUM_CRC8_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Calculate CRC-8
 *
 * @param[in] buf   Start of memory area to checksum
 * @param[in] len   Number of bytes in @p buf to calculate checksum for
 *
 * @return  Checksum of the specified memory area.
 */
uint8_t crc8(const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* CHECKSUM_CRC8_H */
/** @} */
