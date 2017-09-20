/**
 * \file
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _SAM3XA_PIOD_INSTANCE_
#define _SAM3XA_PIOD_INSTANCE_

/* ========== Register definition for PIOD peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PIOD_PER             (0x400E1400U) /**< \brief (PIOD) PIO Enable Register */
#define REG_PIOD_PDR             (0x400E1404U) /**< \brief (PIOD) PIO Disable Register */
#define REG_PIOD_PSR             (0x400E1408U) /**< \brief (PIOD) PIO Status Register */
#define REG_PIOD_OER             (0x400E1410U) /**< \brief (PIOD) Output Enable Register */
#define REG_PIOD_ODR             (0x400E1414U) /**< \brief (PIOD) Output Disable Register */
#define REG_PIOD_OSR             (0x400E1418U) /**< \brief (PIOD) Output Status Register */
#define REG_PIOD_IFER            (0x400E1420U) /**< \brief (PIOD) Glitch Input Filter Enable Register */
#define REG_PIOD_IFDR            (0x400E1424U) /**< \brief (PIOD) Glitch Input Filter Disable Register */
#define REG_PIOD_IFSR            (0x400E1428U) /**< \brief (PIOD) Glitch Input Filter Status Register */
#define REG_PIOD_SODR            (0x400E1430U) /**< \brief (PIOD) Set Output Data Register */
#define REG_PIOD_CODR            (0x400E1434U) /**< \brief (PIOD) Clear Output Data Register */
#define REG_PIOD_ODSR            (0x400E1438U) /**< \brief (PIOD) Output Data Status Register */
#define REG_PIOD_PDSR            (0x400E143CU) /**< \brief (PIOD) Pin Data Status Register */
#define REG_PIOD_IER             (0x400E1440U) /**< \brief (PIOD) Interrupt Enable Register */
#define REG_PIOD_IDR             (0x400E1444U) /**< \brief (PIOD) Interrupt Disable Register */
#define REG_PIOD_IMR             (0x400E1448U) /**< \brief (PIOD) Interrupt Mask Register */
#define REG_PIOD_ISR             (0x400E144CU) /**< \brief (PIOD) Interrupt Status Register */
#define REG_PIOD_MDER            (0x400E1450U) /**< \brief (PIOD) Multi-driver Enable Register */
#define REG_PIOD_MDDR            (0x400E1454U) /**< \brief (PIOD) Multi-driver Disable Register */
#define REG_PIOD_MDSR            (0x400E1458U) /**< \brief (PIOD) Multi-driver Status Register */
#define REG_PIOD_PUDR            (0x400E1460U) /**< \brief (PIOD) Pull-up Disable Register */
#define REG_PIOD_PUER            (0x400E1464U) /**< \brief (PIOD) Pull-up Enable Register */
#define REG_PIOD_PUSR            (0x400E1468U) /**< \brief (PIOD) Pad Pull-up Status Register */
#define REG_PIOD_ABSR            (0x400E1470U) /**< \brief (PIOD) Peripheral AB Select Register */
#define REG_PIOD_SCIFSR          (0x400E1480U) /**< \brief (PIOD) System Clock Glitch Input Filter Select Register */
#define REG_PIOD_DIFSR           (0x400E1484U) /**< \brief (PIOD) Debouncing Input Filter Select Register */
#define REG_PIOD_IFDGSR          (0x400E1488U) /**< \brief (PIOD) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOD_SCDR            (0x400E148CU) /**< \brief (PIOD) Slow Clock Divider Debouncing Register */
#define REG_PIOD_OWER            (0x400E14A0U) /**< \brief (PIOD) Output Write Enable */
#define REG_PIOD_OWDR            (0x400E14A4U) /**< \brief (PIOD) Output Write Disable */
#define REG_PIOD_OWSR            (0x400E14A8U) /**< \brief (PIOD) Output Write Status Register */
#define REG_PIOD_AIMER           (0x400E14B0U) /**< \brief (PIOD) Additional Interrupt Modes Enable Register */
#define REG_PIOD_AIMDR           (0x400E14B4U) /**< \brief (PIOD) Additional Interrupt Modes Disables Register */
#define REG_PIOD_AIMMR           (0x400E14B8U) /**< \brief (PIOD) Additional Interrupt Modes Mask Register */
#define REG_PIOD_ESR             (0x400E14C0U) /**< \brief (PIOD) Edge Select Register */
#define REG_PIOD_LSR             (0x400E14C4U) /**< \brief (PIOD) Level Select Register */
#define REG_PIOD_ELSR            (0x400E14C8U) /**< \brief (PIOD) Edge/Level Status Register */
#define REG_PIOD_FELLSR          (0x400E14D0U) /**< \brief (PIOD) Falling Edge/Low Level Select Register */
#define REG_PIOD_REHLSR          (0x400E14D4U) /**< \brief (PIOD) Rising Edge/ High Level Select Register */
#define REG_PIOD_FRLHSR          (0x400E14D8U) /**< \brief (PIOD) Fall/Rise - Low/High Status Register */
#define REG_PIOD_LOCKSR          (0x400E14E0U) /**< \brief (PIOD) Lock Status */
#define REG_PIOD_WPMR            (0x400E14E4U) /**< \brief (PIOD) Write Protect Mode Register */
#define REG_PIOD_WPSR            (0x400E14E8U) /**< \brief (PIOD) Write Protect Status Register */
#else
#define REG_PIOD_PER    (*(WoReg*)0x400E1400U) /**< \brief (PIOD) PIO Enable Register */
#define REG_PIOD_PDR    (*(WoReg*)0x400E1404U) /**< \brief (PIOD) PIO Disable Register */
#define REG_PIOD_PSR    (*(RoReg*)0x400E1408U) /**< \brief (PIOD) PIO Status Register */
#define REG_PIOD_OER    (*(WoReg*)0x400E1410U) /**< \brief (PIOD) Output Enable Register */
#define REG_PIOD_ODR    (*(WoReg*)0x400E1414U) /**< \brief (PIOD) Output Disable Register */
#define REG_PIOD_OSR    (*(RoReg*)0x400E1418U) /**< \brief (PIOD) Output Status Register */
#define REG_PIOD_IFER   (*(WoReg*)0x400E1420U) /**< \brief (PIOD) Glitch Input Filter Enable Register */
#define REG_PIOD_IFDR   (*(WoReg*)0x400E1424U) /**< \brief (PIOD) Glitch Input Filter Disable Register */
#define REG_PIOD_IFSR   (*(RoReg*)0x400E1428U) /**< \brief (PIOD) Glitch Input Filter Status Register */
#define REG_PIOD_SODR   (*(WoReg*)0x400E1430U) /**< \brief (PIOD) Set Output Data Register */
#define REG_PIOD_CODR   (*(WoReg*)0x400E1434U) /**< \brief (PIOD) Clear Output Data Register */
#define REG_PIOD_ODSR   (*(RwReg*)0x400E1438U) /**< \brief (PIOD) Output Data Status Register */
#define REG_PIOD_PDSR   (*(RoReg*)0x400E143CU) /**< \brief (PIOD) Pin Data Status Register */
#define REG_PIOD_IER    (*(WoReg*)0x400E1440U) /**< \brief (PIOD) Interrupt Enable Register */
#define REG_PIOD_IDR    (*(WoReg*)0x400E1444U) /**< \brief (PIOD) Interrupt Disable Register */
#define REG_PIOD_IMR    (*(RoReg*)0x400E1448U) /**< \brief (PIOD) Interrupt Mask Register */
#define REG_PIOD_ISR    (*(RoReg*)0x400E144CU) /**< \brief (PIOD) Interrupt Status Register */
#define REG_PIOD_MDER   (*(WoReg*)0x400E1450U) /**< \brief (PIOD) Multi-driver Enable Register */
#define REG_PIOD_MDDR   (*(WoReg*)0x400E1454U) /**< \brief (PIOD) Multi-driver Disable Register */
#define REG_PIOD_MDSR   (*(RoReg*)0x400E1458U) /**< \brief (PIOD) Multi-driver Status Register */
#define REG_PIOD_PUDR   (*(WoReg*)0x400E1460U) /**< \brief (PIOD) Pull-up Disable Register */
#define REG_PIOD_PUER   (*(WoReg*)0x400E1464U) /**< \brief (PIOD) Pull-up Enable Register */
#define REG_PIOD_PUSR   (*(RoReg*)0x400E1468U) /**< \brief (PIOD) Pad Pull-up Status Register */
#define REG_PIOD_ABSR   (*(RwReg*)0x400E1470U) /**< \brief (PIOD) Peripheral AB Select Register */
#define REG_PIOD_SCIFSR (*(WoReg*)0x400E1480U) /**< \brief (PIOD) System Clock Glitch Input Filter Select Register */
#define REG_PIOD_DIFSR  (*(WoReg*)0x400E1484U) /**< \brief (PIOD) Debouncing Input Filter Select Register */
#define REG_PIOD_IFDGSR (*(RoReg*)0x400E1488U) /**< \brief (PIOD) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOD_SCDR   (*(RwReg*)0x400E148CU) /**< \brief (PIOD) Slow Clock Divider Debouncing Register */
#define REG_PIOD_OWER   (*(WoReg*)0x400E14A0U) /**< \brief (PIOD) Output Write Enable */
#define REG_PIOD_OWDR   (*(WoReg*)0x400E14A4U) /**< \brief (PIOD) Output Write Disable */
#define REG_PIOD_OWSR   (*(RoReg*)0x400E14A8U) /**< \brief (PIOD) Output Write Status Register */
#define REG_PIOD_AIMER  (*(WoReg*)0x400E14B0U) /**< \brief (PIOD) Additional Interrupt Modes Enable Register */
#define REG_PIOD_AIMDR  (*(WoReg*)0x400E14B4U) /**< \brief (PIOD) Additional Interrupt Modes Disables Register */
#define REG_PIOD_AIMMR  (*(RoReg*)0x400E14B8U) /**< \brief (PIOD) Additional Interrupt Modes Mask Register */
#define REG_PIOD_ESR    (*(WoReg*)0x400E14C0U) /**< \brief (PIOD) Edge Select Register */
#define REG_PIOD_LSR    (*(WoReg*)0x400E14C4U) /**< \brief (PIOD) Level Select Register */
#define REG_PIOD_ELSR   (*(RoReg*)0x400E14C8U) /**< \brief (PIOD) Edge/Level Status Register */
#define REG_PIOD_FELLSR (*(WoReg*)0x400E14D0U) /**< \brief (PIOD) Falling Edge/Low Level Select Register */
#define REG_PIOD_REHLSR (*(WoReg*)0x400E14D4U) /**< \brief (PIOD) Rising Edge/ High Level Select Register */
#define REG_PIOD_FRLHSR (*(RoReg*)0x400E14D8U) /**< \brief (PIOD) Fall/Rise - Low/High Status Register */
#define REG_PIOD_LOCKSR (*(RoReg*)0x400E14E0U) /**< \brief (PIOD) Lock Status */
#define REG_PIOD_WPMR   (*(RwReg*)0x400E14E4U) /**< \brief (PIOD) Write Protect Mode Register */
#define REG_PIOD_WPSR   (*(RoReg*)0x400E14E8U) /**< \brief (PIOD) Write Protect Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3XA_PIOD_INSTANCE_ */
