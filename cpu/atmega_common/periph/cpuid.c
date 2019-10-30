/*
 * Copyright (C) 2018 RWTH Aachen, Josua Arndt, Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */
#include <stdint.h>

#include "periph/cpuid.h"
#include "avr/boot.h"

/**
 * @brief CPU_ID build from MCU register
 *
 *  CPIUD is taken from MCU Control Register and Signature bytes.
 *  CPUID:  1e a8 02 1f 94 03 ff ff
 *  CPUID:  1e a8 02 1f 94 92 XX XX
 *  MEGA62/128/256_RFR2 [MANUAL] p.505
 *  MEGA62/128/256_RFR2 [MANUAL] p.138
 *  MEGA62/128/256_RFR2 [MANUAL] p.492
 *
 *  usr_sign_0/1 are configurable values on flash page 1.
 */
void cpuid_get(void *id)
{
    uint8_t *out = id;
    out[0] = boot_signature_byte_get(0x00);     /* Atmel manufacturer ID (0x1E) */
    out[1] = boot_signature_byte_get(0x02);     /* Part Number high byte */
    out[2] = boot_signature_byte_get(0x04);     /* Part Number low byte */
    out[3] = boot_signature_byte_get(0x01);     /* internal RC calibration byte */
/* last two bytes can be set to flash page 1. for differentiation of different boards */
    out[4] = boot_signature_byte_get(0x0100);   /* user signature 0 */
    out[5] = boot_signature_byte_get(0x0102);   /* user signature 1 */
}
