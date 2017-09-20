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

#ifndef _SAM3XA_TC0_INSTANCE_
#define _SAM3XA_TC0_INSTANCE_

/* ========== Register definition for TC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC0_CCR0           (0x40080000U) /**< \brief (TC0) Channel Control Register (channel = 0) */
#define REG_TC0_CMR0           (0x40080004U) /**< \brief (TC0) Channel Mode Register (channel = 0) */
#define REG_TC0_SMMR0          (0x40080008U) /**< \brief (TC0) Stepper Motor Mode Register (channel = 0) */
#define REG_TC0_CV0            (0x40080010U) /**< \brief (TC0) Counter Value (channel = 0) */
#define REG_TC0_RA0            (0x40080014U) /**< \brief (TC0) Register A (channel = 0) */
#define REG_TC0_RB0            (0x40080018U) /**< \brief (TC0) Register B (channel = 0) */
#define REG_TC0_RC0            (0x4008001CU) /**< \brief (TC0) Register C (channel = 0) */
#define REG_TC0_SR0            (0x40080020U) /**< \brief (TC0) Status Register (channel = 0) */
#define REG_TC0_IER0           (0x40080024U) /**< \brief (TC0) Interrupt Enable Register (channel = 0) */
#define REG_TC0_IDR0           (0x40080028U) /**< \brief (TC0) Interrupt Disable Register (channel = 0) */
#define REG_TC0_IMR0           (0x4008002CU) /**< \brief (TC0) Interrupt Mask Register (channel = 0) */
#define REG_TC0_CCR1           (0x40080040U) /**< \brief (TC0) Channel Control Register (channel = 1) */
#define REG_TC0_CMR1           (0x40080044U) /**< \brief (TC0) Channel Mode Register (channel = 1) */
#define REG_TC0_SMMR1          (0x40080048U) /**< \brief (TC0) Stepper Motor Mode Register (channel = 1) */
#define REG_TC0_CV1            (0x40080050U) /**< \brief (TC0) Counter Value (channel = 1) */
#define REG_TC0_RA1            (0x40080054U) /**< \brief (TC0) Register A (channel = 1) */
#define REG_TC0_RB1            (0x40080058U) /**< \brief (TC0) Register B (channel = 1) */
#define REG_TC0_RC1            (0x4008005CU) /**< \brief (TC0) Register C (channel = 1) */
#define REG_TC0_SR1            (0x40080060U) /**< \brief (TC0) Status Register (channel = 1) */
#define REG_TC0_IER1           (0x40080064U) /**< \brief (TC0) Interrupt Enable Register (channel = 1) */
#define REG_TC0_IDR1           (0x40080068U) /**< \brief (TC0) Interrupt Disable Register (channel = 1) */
#define REG_TC0_IMR1           (0x4008006CU) /**< \brief (TC0) Interrupt Mask Register (channel = 1) */
#define REG_TC0_CCR2           (0x40080080U) /**< \brief (TC0) Channel Control Register (channel = 2) */
#define REG_TC0_CMR2           (0x40080084U) /**< \brief (TC0) Channel Mode Register (channel = 2) */
#define REG_TC0_SMMR2          (0x40080088U) /**< \brief (TC0) Stepper Motor Mode Register (channel = 2) */
#define REG_TC0_CV2            (0x40080090U) /**< \brief (TC0) Counter Value (channel = 2) */
#define REG_TC0_RA2            (0x40080094U) /**< \brief (TC0) Register A (channel = 2) */
#define REG_TC0_RB2            (0x40080098U) /**< \brief (TC0) Register B (channel = 2) */
#define REG_TC0_RC2            (0x4008009CU) /**< \brief (TC0) Register C (channel = 2) */
#define REG_TC0_SR2            (0x400800A0U) /**< \brief (TC0) Status Register (channel = 2) */
#define REG_TC0_IER2           (0x400800A4U) /**< \brief (TC0) Interrupt Enable Register (channel = 2) */
#define REG_TC0_IDR2           (0x400800A8U) /**< \brief (TC0) Interrupt Disable Register (channel = 2) */
#define REG_TC0_IMR2           (0x400800ACU) /**< \brief (TC0) Interrupt Mask Register (channel = 2) */
#define REG_TC0_BCR            (0x400800C0U) /**< \brief (TC0) Block Control Register */
#define REG_TC0_BMR            (0x400800C4U) /**< \brief (TC0) Block Mode Register */
#define REG_TC0_QIER           (0x400800C8U) /**< \brief (TC0) QDEC Interrupt Enable Register */
#define REG_TC0_QIDR           (0x400800CCU) /**< \brief (TC0) QDEC Interrupt Disable Register */
#define REG_TC0_QIMR           (0x400800D0U) /**< \brief (TC0) QDEC Interrupt Mask Register */
#define REG_TC0_QISR           (0x400800D4U) /**< \brief (TC0) QDEC Interrupt Status Register */
#define REG_TC0_FMR            (0x400800D8U) /**< \brief (TC0) Fault Mode Register */
#define REG_TC0_WPMR           (0x400800E4U) /**< \brief (TC0) Write Protect Mode Register */
#else
#define REG_TC0_CCR0  (*(WoReg*)0x40080000U) /**< \brief (TC0) Channel Control Register (channel = 0) */
#define REG_TC0_CMR0  (*(RwReg*)0x40080004U) /**< \brief (TC0) Channel Mode Register (channel = 0) */
#define REG_TC0_SMMR0 (*(RwReg*)0x40080008U) /**< \brief (TC0) Stepper Motor Mode Register (channel = 0) */
#define REG_TC0_CV0   (*(RoReg*)0x40080010U) /**< \brief (TC0) Counter Value (channel = 0) */
#define REG_TC0_RA0   (*(RwReg*)0x40080014U) /**< \brief (TC0) Register A (channel = 0) */
#define REG_TC0_RB0   (*(RwReg*)0x40080018U) /**< \brief (TC0) Register B (channel = 0) */
#define REG_TC0_RC0   (*(RwReg*)0x4008001CU) /**< \brief (TC0) Register C (channel = 0) */
#define REG_TC0_SR0   (*(RoReg*)0x40080020U) /**< \brief (TC0) Status Register (channel = 0) */
#define REG_TC0_IER0  (*(WoReg*)0x40080024U) /**< \brief (TC0) Interrupt Enable Register (channel = 0) */
#define REG_TC0_IDR0  (*(WoReg*)0x40080028U) /**< \brief (TC0) Interrupt Disable Register (channel = 0) */
#define REG_TC0_IMR0  (*(RoReg*)0x4008002CU) /**< \brief (TC0) Interrupt Mask Register (channel = 0) */
#define REG_TC0_CCR1  (*(WoReg*)0x40080040U) /**< \brief (TC0) Channel Control Register (channel = 1) */
#define REG_TC0_CMR1  (*(RwReg*)0x40080044U) /**< \brief (TC0) Channel Mode Register (channel = 1) */
#define REG_TC0_SMMR1 (*(RwReg*)0x40080048U) /**< \brief (TC0) Stepper Motor Mode Register (channel = 1) */
#define REG_TC0_CV1   (*(RoReg*)0x40080050U) /**< \brief (TC0) Counter Value (channel = 1) */
#define REG_TC0_RA1   (*(RwReg*)0x40080054U) /**< \brief (TC0) Register A (channel = 1) */
#define REG_TC0_RB1   (*(RwReg*)0x40080058U) /**< \brief (TC0) Register B (channel = 1) */
#define REG_TC0_RC1   (*(RwReg*)0x4008005CU) /**< \brief (TC0) Register C (channel = 1) */
#define REG_TC0_SR1   (*(RoReg*)0x40080060U) /**< \brief (TC0) Status Register (channel = 1) */
#define REG_TC0_IER1  (*(WoReg*)0x40080064U) /**< \brief (TC0) Interrupt Enable Register (channel = 1) */
#define REG_TC0_IDR1  (*(WoReg*)0x40080068U) /**< \brief (TC0) Interrupt Disable Register (channel = 1) */
#define REG_TC0_IMR1  (*(RoReg*)0x4008006CU) /**< \brief (TC0) Interrupt Mask Register (channel = 1) */
#define REG_TC0_CCR2  (*(WoReg*)0x40080080U) /**< \brief (TC0) Channel Control Register (channel = 2) */
#define REG_TC0_CMR2  (*(RwReg*)0x40080084U) /**< \brief (TC0) Channel Mode Register (channel = 2) */
#define REG_TC0_SMMR2 (*(RwReg*)0x40080088U) /**< \brief (TC0) Stepper Motor Mode Register (channel = 2) */
#define REG_TC0_CV2   (*(RoReg*)0x40080090U) /**< \brief (TC0) Counter Value (channel = 2) */
#define REG_TC0_RA2   (*(RwReg*)0x40080094U) /**< \brief (TC0) Register A (channel = 2) */
#define REG_TC0_RB2   (*(RwReg*)0x40080098U) /**< \brief (TC0) Register B (channel = 2) */
#define REG_TC0_RC2   (*(RwReg*)0x4008009CU) /**< \brief (TC0) Register C (channel = 2) */
#define REG_TC0_SR2   (*(RoReg*)0x400800A0U) /**< \brief (TC0) Status Register (channel = 2) */
#define REG_TC0_IER2  (*(WoReg*)0x400800A4U) /**< \brief (TC0) Interrupt Enable Register (channel = 2) */
#define REG_TC0_IDR2  (*(WoReg*)0x400800A8U) /**< \brief (TC0) Interrupt Disable Register (channel = 2) */
#define REG_TC0_IMR2  (*(RoReg*)0x400800ACU) /**< \brief (TC0) Interrupt Mask Register (channel = 2) */
#define REG_TC0_BCR   (*(WoReg*)0x400800C0U) /**< \brief (TC0) Block Control Register */
#define REG_TC0_BMR   (*(RwReg*)0x400800C4U) /**< \brief (TC0) Block Mode Register */
#define REG_TC0_QIER  (*(WoReg*)0x400800C8U) /**< \brief (TC0) QDEC Interrupt Enable Register */
#define REG_TC0_QIDR  (*(WoReg*)0x400800CCU) /**< \brief (TC0) QDEC Interrupt Disable Register */
#define REG_TC0_QIMR  (*(RoReg*)0x400800D0U) /**< \brief (TC0) QDEC Interrupt Mask Register */
#define REG_TC0_QISR  (*(RoReg*)0x400800D4U) /**< \brief (TC0) QDEC Interrupt Status Register */
#define REG_TC0_FMR   (*(RwReg*)0x400800D8U) /**< \brief (TC0) Fault Mode Register */
#define REG_TC0_WPMR  (*(RwReg*)0x400800E4U) /**< \brief (TC0) Write Protect Mode Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3XA_TC0_INSTANCE_ */
