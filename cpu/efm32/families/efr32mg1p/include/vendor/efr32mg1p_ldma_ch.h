/***************************************************************************//**
 * @file
 * @brief EFR32MG1P_LDMA_CH register and bit field definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @brief LDMA_CH LDMA CH Register
 * @ingroup EFR32MG1P_LDMA
 ******************************************************************************/
typedef struct {
  __IOM uint32_t REQSEL;        /**< Channel Peripheral Request Select Register  */
  __IOM uint32_t CFG;           /**< Channel Configuration Register  */
  __IOM uint32_t LOOP;          /**< Channel Loop Counter Register  */
  __IOM uint32_t CTRL;          /**< Channel Descriptor Control Word Register  */
  __IOM uint32_t SRC;           /**< Channel Descriptor Source Data Address Register  */
  __IOM uint32_t DST;           /**< Channel Descriptor Destination Data Address Register  */
  __IOM uint32_t LINK;          /**< Channel Descriptor Link Structure Address Register  */
  uint32_t       RESERVED0[5U]; /**< Reserved future */
} LDMA_CH_TypeDef;

/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

