/*
 * Copyright 2008-2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_arm7_common
 * @{
 *
 * @file
 * @brief       Common ARM7 boot up code
 *
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include "thread.h"

#include "cpu.h"
#include "log.h"

static inline void
_init_data(void)
{
    extern unsigned int _etext;
    extern unsigned int _data;
    extern unsigned int _edata;
    extern unsigned int __bss_start;
    extern unsigned int __bss_end;

/* Support for LPRAM. */
#ifdef CPU_HAS_BACKUP_RAM
    extern unsigned int _sbackup_data_load[];
    extern unsigned int _sbackup_data[];
    extern unsigned int _ebackup_data[];
    extern unsigned int _sbackup_bss[];
    extern unsigned int _ebackup_bss[];
#endif /* CPU_HAS_BACKUP_RAM */

    register unsigned int *p1;
    register unsigned int *p2;
    register unsigned int *p3;

    // initialize data from flash
    // (linker script ensures that data is 32-bit aligned)
    p1 = &_etext;
    p2 = &_data;
    p3 = &_edata;

    while (p2 < p3) {
        *p2++ = *p1++;
    }

    // clear bss
    // (linker script ensures that bss is 32-bit aligned)
    p1 = &__bss_start;
    p2 = &__bss_end;

    while (p1 < p2) {
        *p1++ = 0;
    }

#ifdef CPU_HAS_BACKUP_RAM
    if (cpu_power_on_reset()) {

        /* load low-power data section. */
        for (p1 = _sbackup_data, p2 = _sbackup_data_load;
             p1 < _ebackup_data;
             p1++, p2++) {
            *p1 = *p2;
        }

        /* zero-out low-power bss. */
        for (p1 = _sbackup_bss; p1 < _ebackup_bss; p1++) {
            *p1 = 0;
        }
    }
#endif /* CPU_HAS_BACKUP_RAM */
}

void bootloader(void)
{
    extern void cpu_init(void);

    /* initialize bss and data */
    _init_data();

    /* cpu specific setup of clocks, peripherals */
    cpu_init();

#ifdef MODULE_NEWLIB
    extern void __libc_init_array(void);
    __libc_init_array();
#endif
}

/** @} */
