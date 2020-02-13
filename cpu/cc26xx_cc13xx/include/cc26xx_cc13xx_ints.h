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

#ifndef CC26XX_CC13XX_INTS_H
#define CC26XX_CC13XX_INTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set the ISR handler.
 *
 * @{
 */
void cc26xx_cc13xx_set_isr_rfc_cpe0_handler(void (* fp)(void));
void cc26xx_cc13xx_set_isr_rfc_cpe1_handler(void (* fp)(void));
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* CC26XX_CC13XX_INTS_H */

/*@}*/
