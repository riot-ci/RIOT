/*
 * Copyright (C) 2018 RWTH Aachen, Josua Arndt, Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  cpu_atmega256rfr2
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
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"

#include "atmega_regs_common.h"

#ifdef MODULE_AT86RF2XX
    #include "at86rf2xx_netdev.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/* Define CPU ID with CPU and transceiver IDs
 *
 * <1 Byte>
 * <JTAG Manufacturer ID>
 *
 * <3 Byte>
 * <Device Signature Part Numb>
 *
 * <1 Byte> \\ only first byte contains data
 * <JEDEC Manufacture ID 0 byte>
 *
 * <1 Byte>
 * <VERSION_NUM - Device Identification Register (Version Number)>
 *
 * <1 Byte>
 * < PART_NUM - Device Identification Register (Part Number) >
 *
 * < 1 Byte>
 * <Random Number> \\ MAc is initialized
 *
 * CPU ID: 1f:02:a8:1e:1f:3:94:<xx>
 *
 * The random part is initialized because HWaddr, Long HWaddr and
 * inet6 addr are derived from the CPUID.
 * So to initialize different addresses to different nodes the
 * random part is introduced.
 */

void cpuid_get(void *id)
{
    /* use random number generator to get last byte for the ID */
    uint8_t random;

#ifdef MODULE_AT86RF2XX
    at86rf2xx_get_random_num( &random, 1);
#else
    random = 0x11;
#endif

    uint8_t addr[] = {
        random,
        PART_NUM,
        VERSION_NUM,
        MAN_ID_0,
        SIGNATURE_0,
        SIGNATURE_1,
        SIGNATURE_2,
        MAN_ID_0
    };

    memcpy( id, addr, 8);
}
