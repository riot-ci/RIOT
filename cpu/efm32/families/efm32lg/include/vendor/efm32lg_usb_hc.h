/***************************************************************************//**
 * @file
 * @brief EFM32LG_USB_HC register and bit field definitions
 * @version 5.8.3
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__ICCARM__)
#pragma system_include       /* Treat file as system include file. */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang system_header  /* Treat file as system include file. */
#endif

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @brief USB_HC EFM32LG USB HC
 ******************************************************************************/
typedef struct {
  __IOM uint32_t CHAR;          /**< Host Channel x Characteristics Register  */
  uint32_t       RESERVED0[1U]; /**< Reserved for future use **/
  __IOM uint32_t INT;           /**< Host Channel x Interrupt Register  */
  __IOM uint32_t INTMSK;        /**< Host Channel x Interrupt Mask Register  */
  __IOM uint32_t TSIZ;          /**< Host Channel x Transfer Size Register  */
  __IOM uint32_t DMAADDR;       /**< Host Channel x DMA Address Register  */
  uint32_t       RESERVED1[2U]; /**< Reserved future */
} USB_HC_TypeDef;

/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

