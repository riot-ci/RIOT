/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2/CC13x2 VIMS functions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "cpu.h"

void vims_mode_set(uint32_t mode)
{
    uint32_t reg = VIMS->CTL;
    reg &= ~VIMS_CTL_MODE_m;
    reg |= (mode & VIMS_CTL_MODE_m);

    VIMS->CTL = reg;
}

void vims_configure(bool round_robin, bool prefetch)
{
    uint32_t reg = VIMS->CTL;
    reg &= ~(VIMS_CTL_PREF_EN | VIMS_CTL_ARB_CFG);

    if (round_robin) {
        reg |= VIMS_CTL_ARB_CFG;
    }

    if (prefetch) {
        reg |= VIMS_CTL_PREF_EN;
    }

    /* Set the Arbitration and prefetch mode. */
    VIMS->CTL = reg;
}
