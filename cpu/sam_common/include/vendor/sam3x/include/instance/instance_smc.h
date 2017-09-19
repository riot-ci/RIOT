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

#ifndef _SAM3XA_SMC_INSTANCE_
#define _SAM3XA_SMC_INSTANCE_

/* ========== Register definition for SMC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SMC_CFG               (0x400E0000U) /**< \brief (SMC) SMC NFC Configuration Register */
#define REG_SMC_CTRL              (0x400E0004U) /**< \brief (SMC) SMC NFC Control Register */
#define REG_SMC_SR                (0x400E0008U) /**< \brief (SMC) SMC NFC Status Register */
#define REG_SMC_IER               (0x400E000CU) /**< \brief (SMC) SMC NFC Interrupt Enable Register */
#define REG_SMC_IDR               (0x400E0010U) /**< \brief (SMC) SMC NFC Interrupt Disable Register */
#define REG_SMC_IMR               (0x400E0014U) /**< \brief (SMC) SMC NFC Interrupt Mask Register */
#define REG_SMC_ADDR              (0x400E0018U) /**< \brief (SMC) SMC NFC Address Cycle Zero Register */
#define REG_SMC_BANK              (0x400E001CU) /**< \brief (SMC) SMC Bank Address Register */
#define REG_SMC_ECC_CTRL          (0x400E0020U) /**< \brief (SMC) SMC ECC Control Register */
#define REG_SMC_ECC_MD            (0x400E0024U) /**< \brief (SMC) SMC ECC Mode Register */
#define REG_SMC_ECC_SR1           (0x400E0028U) /**< \brief (SMC) SMC ECC Status 1 Register */
#define REG_SMC_ECC_PR0           (0x400E002CU) /**< \brief (SMC) SMC ECC Parity 0 Register */
#define REG_SMC_ECC_PR1           (0x400E0030U) /**< \brief (SMC) SMC ECC parity 1 Register */
#define REG_SMC_ECC_SR2           (0x400E0034U) /**< \brief (SMC) SMC ECC status 2 Register */
#define REG_SMC_ECC_PR2           (0x400E0038U) /**< \brief (SMC) SMC ECC parity 2 Register */
#define REG_SMC_ECC_PR3           (0x400E003CU) /**< \brief (SMC) SMC ECC parity 3 Register */
#define REG_SMC_ECC_PR4           (0x400E0040U) /**< \brief (SMC) SMC ECC parity 4 Register */
#define REG_SMC_ECC_PR5           (0x400E0044U) /**< \brief (SMC) SMC ECC parity 5 Register */
#define REG_SMC_ECC_PR6           (0x400E0048U) /**< \brief (SMC) SMC ECC parity 6 Register */
#define REG_SMC_ECC_PR7           (0x400E004CU) /**< \brief (SMC) SMC ECC parity 7 Register */
#define REG_SMC_ECC_PR8           (0x400E0050U) /**< \brief (SMC) SMC ECC parity 8 Register */
#define REG_SMC_ECC_PR9           (0x400E0054U) /**< \brief (SMC) SMC ECC parity 9 Register */
#define REG_SMC_ECC_PR10          (0x400E0058U) /**< \brief (SMC) SMC ECC parity 10 Register */
#define REG_SMC_ECC_PR11          (0x400E005CU) /**< \brief (SMC) SMC ECC parity 11 Register */
#define REG_SMC_ECC_PR12          (0x400E0060U) /**< \brief (SMC) SMC ECC parity 12 Register */
#define REG_SMC_ECC_PR13          (0x400E0064U) /**< \brief (SMC) SMC ECC parity 13 Register */
#define REG_SMC_ECC_PR14          (0x400E0068U) /**< \brief (SMC) SMC ECC parity 14 Register */
#define REG_SMC_ECC_PR15          (0x400E006CU) /**< \brief (SMC) SMC ECC parity 15 Register */
#define REG_SMC_SETUP0            (0x400E0070U) /**< \brief (SMC) SMC Setup Register (CS_number = 0) */
#define REG_SMC_PULSE0            (0x400E0074U) /**< \brief (SMC) SMC Pulse Register (CS_number = 0) */
#define REG_SMC_CYCLE0            (0x400E0078U) /**< \brief (SMC) SMC Cycle Register (CS_number = 0) */
#define REG_SMC_TIMINGS0          (0x400E007CU) /**< \brief (SMC) SMC Timings Register (CS_number = 0) */
#define REG_SMC_MODE0             (0x400E0080U) /**< \brief (SMC) SMC Mode Register (CS_number = 0) */
#define REG_SMC_SETUP1            (0x400E0084U) /**< \brief (SMC) SMC Setup Register (CS_number = 1) */
#define REG_SMC_PULSE1            (0x400E0088U) /**< \brief (SMC) SMC Pulse Register (CS_number = 1) */
#define REG_SMC_CYCLE1            (0x400E008CU) /**< \brief (SMC) SMC Cycle Register (CS_number = 1) */
#define REG_SMC_TIMINGS1          (0x400E0090U) /**< \brief (SMC) SMC Timings Register (CS_number = 1) */
#define REG_SMC_MODE1             (0x400E0094U) /**< \brief (SMC) SMC Mode Register (CS_number = 1) */
#define REG_SMC_SETUP2            (0x400E0098U) /**< \brief (SMC) SMC Setup Register (CS_number = 2) */
#define REG_SMC_PULSE2            (0x400E009CU) /**< \brief (SMC) SMC Pulse Register (CS_number = 2) */
#define REG_SMC_CYCLE2            (0x400E00A0U) /**< \brief (SMC) SMC Cycle Register (CS_number = 2) */
#define REG_SMC_TIMINGS2          (0x400E00A4U) /**< \brief (SMC) SMC Timings Register (CS_number = 2) */
#define REG_SMC_MODE2             (0x400E00A8U) /**< \brief (SMC) SMC Mode Register (CS_number = 2) */
#define REG_SMC_SETUP3            (0x400E00ACU) /**< \brief (SMC) SMC Setup Register (CS_number = 3) */
#define REG_SMC_PULSE3            (0x400E00B0U) /**< \brief (SMC) SMC Pulse Register (CS_number = 3) */
#define REG_SMC_CYCLE3            (0x400E00B4U) /**< \brief (SMC) SMC Cycle Register (CS_number = 3) */
#define REG_SMC_TIMINGS3          (0x400E00B8U) /**< \brief (SMC) SMC Timings Register (CS_number = 3) */
#define REG_SMC_MODE3             (0x400E00BCU) /**< \brief (SMC) SMC Mode Register (CS_number = 3) */
#define REG_SMC_SETUP4            (0x400E00C0U) /**< \brief (SMC) SMC Setup Register (CS_number = 4) */
#define REG_SMC_PULSE4            (0x400E00C4U) /**< \brief (SMC) SMC Pulse Register (CS_number = 4) */
#define REG_SMC_CYCLE4            (0x400E00C8U) /**< \brief (SMC) SMC Cycle Register (CS_number = 4) */
#define REG_SMC_TIMINGS4          (0x400E00CCU) /**< \brief (SMC) SMC Timings Register (CS_number = 4) */
#define REG_SMC_MODE4             (0x400E00D0U) /**< \brief (SMC) SMC Mode Register (CS_number = 4) */
#define REG_SMC_SETUP5            (0x400E00D4U) /**< \brief (SMC) SMC Setup Register (CS_number = 5) */
#define REG_SMC_PULSE5            (0x400E00D8U) /**< \brief (SMC) SMC Pulse Register (CS_number = 5) */
#define REG_SMC_CYCLE5            (0x400E00DCU) /**< \brief (SMC) SMC Cycle Register (CS_number = 5) */
#define REG_SMC_TIMINGS5          (0x400E00E0U) /**< \brief (SMC) SMC Timings Register (CS_number = 5) */
#define REG_SMC_MODE5             (0x400E00E4U) /**< \brief (SMC) SMC Mode Register (CS_number = 5) */
#define REG_SMC_SETUP6            (0x400E00E8U) /**< \brief (SMC) SMC Setup Register (CS_number = 6) */
#define REG_SMC_PULSE6            (0x400E00ECU) /**< \brief (SMC) SMC Pulse Register (CS_number = 6) */
#define REG_SMC_CYCLE6            (0x400E00F0U) /**< \brief (SMC) SMC Cycle Register (CS_number = 6) */
#define REG_SMC_TIMINGS6          (0x400E00F4U) /**< \brief (SMC) SMC Timings Register (CS_number = 6) */
#define REG_SMC_MODE6             (0x400E00F8U) /**< \brief (SMC) SMC Mode Register (CS_number = 6) */
#define REG_SMC_SETUP7            (0x400E00FCU) /**< \brief (SMC) SMC Setup Register (CS_number = 7) */
#define REG_SMC_PULSE7            (0x400E0100U) /**< \brief (SMC) SMC Pulse Register (CS_number = 7) */
#define REG_SMC_CYCLE7            (0x400E0104U) /**< \brief (SMC) SMC Cycle Register (CS_number = 7) */
#define REG_SMC_TIMINGS7          (0x400E0108U) /**< \brief (SMC) SMC Timings Register (CS_number = 7) */
#define REG_SMC_MODE7             (0x400E010CU) /**< \brief (SMC) SMC Mode Register (CS_number = 7) */
#define REG_SMC_OCMS              (0x400E0110U) /**< \brief (SMC) SMC OCMS Register */
#define REG_SMC_KEY1              (0x400E0114U) /**< \brief (SMC) SMC OCMS KEY1 Register */
#define REG_SMC_KEY2              (0x400E0118U) /**< \brief (SMC) SMC OCMS KEY2 Register */
#define REG_SMC_WPCR              (0x400E01E4U) /**< \brief (SMC) Write Protection Control Register */
#define REG_SMC_WPSR              (0x400E01E8U) /**< \brief (SMC) Write Protection Status Register */
#else
#define REG_SMC_CFG      (*(RwReg*)0x400E0000U) /**< \brief (SMC) SMC NFC Configuration Register */
#define REG_SMC_CTRL     (*(WoReg*)0x400E0004U) /**< \brief (SMC) SMC NFC Control Register */
#define REG_SMC_SR       (*(RoReg*)0x400E0008U) /**< \brief (SMC) SMC NFC Status Register */
#define REG_SMC_IER      (*(WoReg*)0x400E000CU) /**< \brief (SMC) SMC NFC Interrupt Enable Register */
#define REG_SMC_IDR      (*(WoReg*)0x400E0010U) /**< \brief (SMC) SMC NFC Interrupt Disable Register */
#define REG_SMC_IMR      (*(RoReg*)0x400E0014U) /**< \brief (SMC) SMC NFC Interrupt Mask Register */
#define REG_SMC_ADDR     (*(RwReg*)0x400E0018U) /**< \brief (SMC) SMC NFC Address Cycle Zero Register */
#define REG_SMC_BANK     (*(RwReg*)0x400E001CU) /**< \brief (SMC) SMC Bank Address Register */
#define REG_SMC_ECC_CTRL (*(WoReg*)0x400E0020U) /**< \brief (SMC) SMC ECC Control Register */
#define REG_SMC_ECC_MD   (*(RwReg*)0x400E0024U) /**< \brief (SMC) SMC ECC Mode Register */
#define REG_SMC_ECC_SR1  (*(RoReg*)0x400E0028U) /**< \brief (SMC) SMC ECC Status 1 Register */
#define REG_SMC_ECC_PR0  (*(RoReg*)0x400E002CU) /**< \brief (SMC) SMC ECC Parity 0 Register */
#define REG_SMC_ECC_PR1  (*(RoReg*)0x400E0030U) /**< \brief (SMC) SMC ECC parity 1 Register */
#define REG_SMC_ECC_SR2  (*(RoReg*)0x400E0034U) /**< \brief (SMC) SMC ECC status 2 Register */
#define REG_SMC_ECC_PR2  (*(RoReg*)0x400E0038U) /**< \brief (SMC) SMC ECC parity 2 Register */
#define REG_SMC_ECC_PR3  (*(RoReg*)0x400E003CU) /**< \brief (SMC) SMC ECC parity 3 Register */
#define REG_SMC_ECC_PR4  (*(RoReg*)0x400E0040U) /**< \brief (SMC) SMC ECC parity 4 Register */
#define REG_SMC_ECC_PR5  (*(RoReg*)0x400E0044U) /**< \brief (SMC) SMC ECC parity 5 Register */
#define REG_SMC_ECC_PR6  (*(RoReg*)0x400E0048U) /**< \brief (SMC) SMC ECC parity 6 Register */
#define REG_SMC_ECC_PR7  (*(RoReg*)0x400E004CU) /**< \brief (SMC) SMC ECC parity 7 Register */
#define REG_SMC_ECC_PR8  (*(RoReg*)0x400E0050U) /**< \brief (SMC) SMC ECC parity 8 Register */
#define REG_SMC_ECC_PR9  (*(RoReg*)0x400E0054U) /**< \brief (SMC) SMC ECC parity 9 Register */
#define REG_SMC_ECC_PR10 (*(RoReg*)0x400E0058U) /**< \brief (SMC) SMC ECC parity 10 Register */
#define REG_SMC_ECC_PR11 (*(RoReg*)0x400E005CU) /**< \brief (SMC) SMC ECC parity 11 Register */
#define REG_SMC_ECC_PR12 (*(RoReg*)0x400E0060U) /**< \brief (SMC) SMC ECC parity 12 Register */
#define REG_SMC_ECC_PR13 (*(RoReg*)0x400E0064U) /**< \brief (SMC) SMC ECC parity 13 Register */
#define REG_SMC_ECC_PR14 (*(RoReg*)0x400E0068U) /**< \brief (SMC) SMC ECC parity 14 Register */
#define REG_SMC_ECC_PR15 (*(RoReg*)0x400E006CU) /**< \brief (SMC) SMC ECC parity 15 Register */
#define REG_SMC_SETUP0   (*(RwReg*)0x400E0070U) /**< \brief (SMC) SMC Setup Register (CS_number = 0) */
#define REG_SMC_PULSE0   (*(RwReg*)0x400E0074U) /**< \brief (SMC) SMC Pulse Register (CS_number = 0) */
#define REG_SMC_CYCLE0   (*(RwReg*)0x400E0078U) /**< \brief (SMC) SMC Cycle Register (CS_number = 0) */
#define REG_SMC_TIMINGS0 (*(RwReg*)0x400E007CU) /**< \brief (SMC) SMC Timings Register (CS_number = 0) */
#define REG_SMC_MODE0    (*(RwReg*)0x400E0080U) /**< \brief (SMC) SMC Mode Register (CS_number = 0) */
#define REG_SMC_SETUP1   (*(RwReg*)0x400E0084U) /**< \brief (SMC) SMC Setup Register (CS_number = 1) */
#define REG_SMC_PULSE1   (*(RwReg*)0x400E0088U) /**< \brief (SMC) SMC Pulse Register (CS_number = 1) */
#define REG_SMC_CYCLE1   (*(RwReg*)0x400E008CU) /**< \brief (SMC) SMC Cycle Register (CS_number = 1) */
#define REG_SMC_TIMINGS1 (*(RwReg*)0x400E0090U) /**< \brief (SMC) SMC Timings Register (CS_number = 1) */
#define REG_SMC_MODE1    (*(RwReg*)0x400E0094U) /**< \brief (SMC) SMC Mode Register (CS_number = 1) */
#define REG_SMC_SETUP2   (*(RwReg*)0x400E0098U) /**< \brief (SMC) SMC Setup Register (CS_number = 2) */
#define REG_SMC_PULSE2   (*(RwReg*)0x400E009CU) /**< \brief (SMC) SMC Pulse Register (CS_number = 2) */
#define REG_SMC_CYCLE2   (*(RwReg*)0x400E00A0U) /**< \brief (SMC) SMC Cycle Register (CS_number = 2) */
#define REG_SMC_TIMINGS2 (*(RwReg*)0x400E00A4U) /**< \brief (SMC) SMC Timings Register (CS_number = 2) */
#define REG_SMC_MODE2    (*(RwReg*)0x400E00A8U) /**< \brief (SMC) SMC Mode Register (CS_number = 2) */
#define REG_SMC_SETUP3   (*(RwReg*)0x400E00ACU) /**< \brief (SMC) SMC Setup Register (CS_number = 3) */
#define REG_SMC_PULSE3   (*(RwReg*)0x400E00B0U) /**< \brief (SMC) SMC Pulse Register (CS_number = 3) */
#define REG_SMC_CYCLE3   (*(RwReg*)0x400E00B4U) /**< \brief (SMC) SMC Cycle Register (CS_number = 3) */
#define REG_SMC_TIMINGS3 (*(RwReg*)0x400E00B8U) /**< \brief (SMC) SMC Timings Register (CS_number = 3) */
#define REG_SMC_MODE3    (*(RwReg*)0x400E00BCU) /**< \brief (SMC) SMC Mode Register (CS_number = 3) */
#define REG_SMC_SETUP4   (*(RwReg*)0x400E00C0U) /**< \brief (SMC) SMC Setup Register (CS_number = 4) */
#define REG_SMC_PULSE4   (*(RwReg*)0x400E00C4U) /**< \brief (SMC) SMC Pulse Register (CS_number = 4) */
#define REG_SMC_CYCLE4   (*(RwReg*)0x400E00C8U) /**< \brief (SMC) SMC Cycle Register (CS_number = 4) */
#define REG_SMC_TIMINGS4 (*(RwReg*)0x400E00CCU) /**< \brief (SMC) SMC Timings Register (CS_number = 4) */
#define REG_SMC_MODE4    (*(RwReg*)0x400E00D0U) /**< \brief (SMC) SMC Mode Register (CS_number = 4) */
#define REG_SMC_SETUP5   (*(RwReg*)0x400E00D4U) /**< \brief (SMC) SMC Setup Register (CS_number = 5) */
#define REG_SMC_PULSE5   (*(RwReg*)0x400E00D8U) /**< \brief (SMC) SMC Pulse Register (CS_number = 5) */
#define REG_SMC_CYCLE5   (*(RwReg*)0x400E00DCU) /**< \brief (SMC) SMC Cycle Register (CS_number = 5) */
#define REG_SMC_TIMINGS5 (*(RwReg*)0x400E00E0U) /**< \brief (SMC) SMC Timings Register (CS_number = 5) */
#define REG_SMC_MODE5    (*(RwReg*)0x400E00E4U) /**< \brief (SMC) SMC Mode Register (CS_number = 5) */
#define REG_SMC_SETUP6   (*(RwReg*)0x400E00E8U) /**< \brief (SMC) SMC Setup Register (CS_number = 6) */
#define REG_SMC_PULSE6   (*(RwReg*)0x400E00ECU) /**< \brief (SMC) SMC Pulse Register (CS_number = 6) */
#define REG_SMC_CYCLE6   (*(RwReg*)0x400E00F0U) /**< \brief (SMC) SMC Cycle Register (CS_number = 6) */
#define REG_SMC_TIMINGS6 (*(RwReg*)0x400E00F4U) /**< \brief (SMC) SMC Timings Register (CS_number = 6) */
#define REG_SMC_MODE6    (*(RwReg*)0x400E00F8U) /**< \brief (SMC) SMC Mode Register (CS_number = 6) */
#define REG_SMC_SETUP7   (*(RwReg*)0x400E00FCU) /**< \brief (SMC) SMC Setup Register (CS_number = 7) */
#define REG_SMC_PULSE7   (*(RwReg*)0x400E0100U) /**< \brief (SMC) SMC Pulse Register (CS_number = 7) */
#define REG_SMC_CYCLE7   (*(RwReg*)0x400E0104U) /**< \brief (SMC) SMC Cycle Register (CS_number = 7) */
#define REG_SMC_TIMINGS7 (*(RwReg*)0x400E0108U) /**< \brief (SMC) SMC Timings Register (CS_number = 7) */
#define REG_SMC_MODE7    (*(RwReg*)0x400E010CU) /**< \brief (SMC) SMC Mode Register (CS_number = 7) */
#define REG_SMC_OCMS     (*(RwReg*)0x400E0110U) /**< \brief (SMC) SMC OCMS Register */
#define REG_SMC_KEY1     (*(WoReg*)0x400E0114U) /**< \brief (SMC) SMC OCMS KEY1 Register */
#define REG_SMC_KEY2     (*(WoReg*)0x400E0118U) /**< \brief (SMC) SMC OCMS KEY2 Register */
#define REG_SMC_WPCR     (*(WoReg*)0x400E01E4U) /**< \brief (SMC) Write Protection Control Register */
#define REG_SMC_WPSR     (*(RoReg*)0x400E01E8U) /**< \brief (SMC) Write Protection Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3XA_SMC_INSTANCE_ */
