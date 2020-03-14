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
 */

#ifndef CC26XX_CC13XX_OSC_H
#define CC26XX_CC13XX_OSC_H

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   SCLK_HF source.
 */
typedef enum {
    SCLK_HF_RCOSC = 0x0, /**< Internal RC oscillator. */
    SCLK_HF_XOSC = 0x1, /**< External crystal oscillator. */
} sclk_hf_src_t;

/**
 * @brief   Get SCLK_HF source.
 */
sclk_hf_src_t osc_get_sclk_hf_source(void);

/**
 * @brief   Set SCLK_HF source.
 *
 * @note This doesn't perform the actual switch after this you must use
 *       @ref osc_sclk_hf_source_switch
 *
 * @param[in] src Oscillator to use.
 */
void osc_set_sclk_hf_source(sclk_hf_src_t src);

/**
 * @brief   Indicates if SCLK_HF is ready to perform the source switch.
 *
 * @return 0 Not ready.
 * @return 1 SCLK_HF is ready to switch.
 */
int osc_sclk_hf_source_ready(void);

/**
 * @brief   Perform the SCLK_HF source switch
 *
 * @note    This calls code from the MCU ROM.
 */
void osc_sclk_hf_source_switch(void);

#ifdef __cplusplus
}
#endif

#endif /* CC26XX_CC13XX_OSC_H */

/*@}*/
