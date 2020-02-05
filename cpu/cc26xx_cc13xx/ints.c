/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx Interrupt handling functions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "cc26xx_cc13xx_ints.h"

/**
 * @brief   RF Core tnterrupt lines.
 */
typedef struct {
    void (* isr_rfc_cpe0)(void); /**< CPE interrupt line 0 */
    void (* isr_rfc_cpe1)(void); /**< CPE interrupt line 1 */
} interrupt_lines_t;

static interrupt_lines_t _interrupt_lines;

void cc26xx_cc13xx_set_isr_rfc_cpe0_handler(void (* fp)(void))
{
    _interrupt_lines.isr_rfc_cpe0 = fp;
}

void cc26xx_cc13xx_set_isr_rfc_cpe1_handler(void (* fp)(void))
{
    _interrupt_lines.isr_rfc_cpe1 = fp;
}

void isr_rfc_cpe0(void)
{
    _interrupt_lines.isr_rfc_cpe0();
}

void isr_rfc_cpe1(void)
{
    _interrupt_lines.isr_rfc_cpe1();
}
