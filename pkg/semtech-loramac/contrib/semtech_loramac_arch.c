/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     pkg_semtech-loramac
 * @file
 * @brief       Compatibility system functions used in LoRaMAC
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "semtech-loramac/board.h"
#include "random.h"

void srand1(uint32_t seed)
{
    random_init(seed);
}

int32_t randr(int32_t min, int32_t max)
{
    return random_uint32_range(min, max + 1);
}

void memcpy1(uint8_t *dst, const uint8_t *src, uint16_t size)
{
    while (size--) {
        *dst++ = *src++;
    }
}

void memcpyr(uint8_t *dst, const uint8_t *src, uint16_t size)
{
    dst = dst + (size - 1);
    while (size--) {
        *dst-- = *src++;
    }
}

void memset1(uint8_t *dst, uint8_t value, uint16_t size)
{
    while (size--) {
        *dst++ = value;
    }
}
