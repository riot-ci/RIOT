/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 AUX functions
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include <assert.h>
#include "cpu.h"

static const uint8_t _opmode_to_order[4] = { 1, 2, 0, 3 };
static const uint8_t _order_to_opmode[4] = { 2, 0, 1, 3 };

void aux_sysif_opmode_change(uint32_t target_opmode)
{
    assert((target_opmode == AUX_SYSIF_OPMODEREQ_REQ_PDLP) ||
           (target_opmode == AUX_SYSIF_OPMODEREQ_REQ_PDA)  ||
           (target_opmode == AUX_SYSIF_OPMODEREQ_REQ_LP)   ||
           (target_opmode == AUX_SYSIF_OPMODEREQ_REQ_A));

    uint32_t current_opmode;
    uint32_t current_order;
    uint32_t next_mode;

    do {
       current_opmode = AUX_SYSIF->OPMODEREQ;
       while (current_opmode != AUX_SYSIF->OPMODEACK) {}
       if (current_opmode != target_opmode) {
           current_order = _opmode_to_order[current_opmode];
           if (current_order < _opmode_to_order[target_opmode]) {
               next_mode = _order_to_opmode[current_order + 1];
           }
           else {
               next_mode = _order_to_opmode[current_order - 1];
           }
           AUX_SYSIF->OPMODEREQ = next_mode;
       }
    } while (current_opmode != target_opmode);
}
