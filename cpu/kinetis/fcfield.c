/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis
 * @{
 *
 * @file
 * @brief       Default FCF code for Freescale Kinetis MCUs
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>

/* fcfield table */
__attribute__((weak, used, section(".fcfield")))
const uint8_t flash_configuration_field[] = {
    0xff,    /* backdoor comparison key 3., offset: 0x0 */
    0xff,    /* backdoor comparison key 2., offset: 0x1 */
    0xff,    /* backdoor comparison key 1., offset: 0x2 */
    0xff,    /* backdoor comparison key 0., offset: 0x3 */
    0xff,    /* backdoor comparison key 7., offset: 0x4 */
    0xff,    /* backdoor comparison key 6., offset: 0x5 */
    0xff,    /* backdoor comparison key 5., offset: 0x6 */
    0xff,    /* backdoor comparison key 4., offset: 0x7 */
    0xff,    /* non-volatile p-flash protection 1 - low register, offset: 0x8 */
    0xff,    /* non-volatile p-flash protection 1 - high register, offset: 0x9 */
    0xff,    /* non-volatile p-flash protection 0 - low register, offset: 0xa */
    0xff,    /* non-volatile p-flash protection 0 - high register, offset: 0xb */
    0xfe,    /* non-volatile flash security register, offset: 0xc */
#if KINETIS_SERIES_K
    0xff,    /* FOPT non-volatile flash option register, offset: 0xd */
#else
    /* Clear bits 6,7 to boot directly from flash on parts which have a ROM
     * bootloader. May also require configuration in OpenOCD `kinetis fopt 0x3d` */
    0x3d,    /* FOPT non-volatile flash option register, offset: 0xd */
#endif
    0xff,    /* non-volatile eeram protection register, offset: 0xe */
    0xff,    /* non-volatile d-flash protection register, offset: 0xf */
};
