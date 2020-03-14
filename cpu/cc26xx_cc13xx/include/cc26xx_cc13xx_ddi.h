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
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_DDI_H
#define CC26XX_CC13XX_DDI_H

/** @brief   DDI master instruction offsets
  * @{
  */
/**
 * @brief   Offset for the direct access instruction
 */
#define DDI_DIR     0x00000000

/**
 * @brief   Offset for 'Set' instruction.
 */
#define DDI_SET     0x00000080

/**
 * @brief   Offset for 'Clear' instruction.
 */
#define DDI_CLR     0x00000100

/**
 * @brief   Offset for 4-bit masked access.
 *
 * Data bit[n] is written if mask bit[n] is set ('1').
 * Bits 7:4 are mask. Bits 3:0 are data.
 * Requires 'byte' write.
 */
#define DDI_MASK4B  0x00000200

/**
 * @brief   Offset for 8-bit masked access.
 *
 * Data bit[n] is written if mask bit[n] is set ('1').
 * Bits 15:8 are mask. Bits 7:0 are data.
 * Requires 'short' write.
 */
#define DDI_MASK8B  0x00000300

/**
 * @brief   Offset for 16-bit masked access.
 *
 * Data bit[n] is written if mask bit[n] is set ('1').
 * Bits 31:16 are mask. Bits 15:0 are data.
 * Requires 'long' write.
 */
#define DDI_MASK16B 0x00000400
/** @} */


#endif /* CC26XX_CC13XX_DDI_H */

/*@}*/
