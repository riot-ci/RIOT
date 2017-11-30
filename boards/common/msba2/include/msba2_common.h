/*
 * Copyright 2014 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_msba2
 * @{
 *
 * @file
 * @brief       MSB-A2 Common Board Definitions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef MSBA2_COMMON_H
#define MSBA2_COMMON_H


#include <stdint.h>
#include "lpc2387.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Feed sequence for PLL register
 */
static inline void pllfeed(void)
{
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MSBA2_COMMON_H */
