/*
 * Copyright (C) 2020 Sören Tempel <tempel@uni-bremen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Test application for the mpu_noexec_stack pseudo-module
 *
 * @author Sören Tempel <tempel@uni-bremen.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define JMPBUF_SIZE 3

int main(void)
{
    uint32_t buf[JMPBUF_SIZE];

    /* Fill the buffer with invalid instructions */
    memset(&buf, UINT32_MAX, sizeof(uint32_t) * JMPBUF_SIZE);

    puts("Attempting to jump to stack buffer ...\n");
    __asm__ volatile ("BX %0"
                      : /* no output operands */
                      : "r" ((uint8_t*)&buf + 1)); /* LSB must be set for thumb2 */

    return 0;
}
