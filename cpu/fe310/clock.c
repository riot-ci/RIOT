/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the clock initialization for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"

#include "vendor/prci_driver.h"

void clock_init(void)
{
    /* Ensure that we aren't running off the PLL before we mess with it. */
    if (PRCI_REG(PRCI_PLLCFG) & PLL_SEL(1)) {
        /* Make sure the HFROSC is running at its default setting */
        /* It is OK to change this even if we are running off of it.*/
        PRCI_REG(PRCI_HFROSCCFG) = (ROSC_DIV(4) | ROSC_TRIM(16) | ROSC_EN(1));

        while ((PRCI_REG(PRCI_HFROSCCFG) & ROSC_RDY(1)) == 0);

        PRCI_REG(PRCI_PLLCFG) &= ~PLL_SEL(1);
    }

    /* Bypass PLL */
    PRCI_REG(PRCI_PLLCFG) = PLL_REFSEL(1) | PLL_BYPASS(1);

#if USE_CLOCK_PLL
    /* Set output divisor */
    PRCI_REG(PRCI_PLLDIV) = PLL_FINAL_DIV(CLOCK_PLL_OUTDIV);

    /* Configure PLL */
    PRCI_REG(PRCI_PLLCFG) |= PLL_R(CLOCK_PLL_R) | PLL_F(CLOCK_PLL_F) | PLL_Q(CLOCK_PLL_Q);

    /* Disable PLL Bypass */
    PRCI_REG(PRCI_PLLCFG) &= ~PLL_BYPASS(1);

    /* Now it is safe to check for PLL Lock */
    while ((PRCI_REG(PRCI_PLLCFG) & PLL_LOCK(1)) == 0);
#endif

    /* Switch over to PLL Clock source */
    PRCI_REG(PRCI_PLLCFG) |= PLL_SEL(1);

    /* Turn off the HFROSC */
    PRCI_REG(PRCI_HFROSCCFG) &= ~ROSC_EN(1);
}
