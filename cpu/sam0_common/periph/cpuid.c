/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_cpuid
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"

#ifdef CPU_SAMD5X
#define WORD0               (*(volatile uint32_t *)0x008061FC)
#define WORD1               (*(volatile uint32_t *)0x00806010)
#define WORD2               (*(volatile uint32_t *)0x00806014)
#define WORD3               (*(volatile uint32_t *)0x00806018)
#else
#define WORD0               (*(volatile uint32_t *)0x0080A00C)
#define WORD1               (*(volatile uint32_t *)0x0080A040)
#define WORD2               (*(volatile uint32_t *)0x0080A044)
#define WORD3               (*(volatile uint32_t *)0x0080A048)
#endif

void cpuid_get(void *id)
{
    uint32_t addr[] = { WORD0, WORD1, WORD2, WORD3 };
    memcpy(id, &addr[0], CPUID_LEN);
}
