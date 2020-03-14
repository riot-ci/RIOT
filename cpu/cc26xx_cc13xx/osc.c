/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx MCU I/O register definitions
 *
 * @author          Jean Pirre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "cpu.h"

static void ddi_write_bitfield16(const uint32_t base, const uint32_t reg,
                                 uint32_t mask, uint32_t shift, uint16_t data)
{
    uint32_t addr = base + (reg << 1) + DDI_MASK16B;

    /* Adjust for target bit in high half of the word. */
    if (shift >= 16) {
        shift = shift - 16;
        addr += 4;
        mask = mask >> 16;
    }

    /* Shift data in to position. */
    uint32_t wr_data = data << shift;

    /* Write data. */
    *(volatile uint32_t *)addr = (mask << 16) | wr_data;
}

sclk_hf_src_t osc_get_sclk_hf_source(void)
{
    uint32_t clk_src = (DDI_0_OSC->STAT0 & DDI_0_OSC_STAT0_SCLK_HF_SRC_m) >>
                        DDI_0_OSC_STAT0_SCLK_HF_SRC_s;

    return clk_src;
}

void osc_set_sclk_hf_source(sclk_hf_src_t src)
{
    ddi_write_bitfield16(DDI0_OSC_BASE, offsetof(ddi0_osc_regs_t, STAT0),
                         DDI_0_OSC_STAT0_SCLK_HF_SRC_m,
                         DDI_0_OSC_STAT0_SCLK_HF_SRC_s, src);
}

int osc_sclk_hf_source_ready(void)
{
    uint32_t ready = (DDI_0_OSC->STAT0 & DDI_0_OSC_STAT0_PENDINGSCLKHFSWITCHING_m) >>
                      DDI_0_OSC_STAT0_PENDINGSCLKHFSWITCHING_s;

    return ready;
}

void osc_sclk_hf_source_switch(void)
{
    ROM_HAPI->HFSourceSafeSwitch();
}
