/**
 * \file
 *
 * \brief Instance description for AC
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef _SAMR30_AC_INSTANCE_
#define _SAMR30_AC_INSTANCE_

/* ========== Register definition for AC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_AC_CTRLA               (0x43001000U) /**< \brief (AC) Control A */
#define REG_AC_CTRLB               (0x43001001U) /**< \brief (AC) Control B */
#define REG_AC_EVCTRL              (0x43001002U) /**< \brief (AC) Event Control */
#define REG_AC_INTENCLR            (0x43001004U) /**< \brief (AC) Interrupt Enable Clear */
#define REG_AC_INTENSET            (0x43001005U) /**< \brief (AC) Interrupt Enable Set */
#define REG_AC_INTFLAG             (0x43001006U) /**< \brief (AC) Interrupt Flag Status and Clear */
#define REG_AC_STATUSA             (0x43001007U) /**< \brief (AC) Status A */
#define REG_AC_STATUSB             (0x43001008U) /**< \brief (AC) Status B */
#define REG_AC_DBGCTRL             (0x43001009U) /**< \brief (AC) Debug Control */
#define REG_AC_WINCTRL             (0x4300100AU) /**< \brief (AC) Window Control */
#define REG_AC_SCALER0             (0x4300100CU) /**< \brief (AC) Scaler 0 */
#define REG_AC_SCALER1             (0x4300100DU) /**< \brief (AC) Scaler 1 */
#define REG_AC_COMPCTRL0           (0x43001010U) /**< \brief (AC) Comparator Control 0 */
#define REG_AC_COMPCTRL1           (0x43001014U) /**< \brief (AC) Comparator Control 1 */
#define REG_AC_SYNCBUSY            (0x43001020U) /**< \brief (AC) Synchronization Busy */
#else
#define REG_AC_CTRLA               (*(RwReg8 *)0x43001000U) /**< \brief (AC) Control A */
#define REG_AC_CTRLB               (*(WoReg8 *)0x43001001U) /**< \brief (AC) Control B */
#define REG_AC_EVCTRL              (*(RwReg16*)0x43001002U) /**< \brief (AC) Event Control */
#define REG_AC_INTENCLR            (*(RwReg8 *)0x43001004U) /**< \brief (AC) Interrupt Enable Clear */
#define REG_AC_INTENSET            (*(RwReg8 *)0x43001005U) /**< \brief (AC) Interrupt Enable Set */
#define REG_AC_INTFLAG             (*(RwReg8 *)0x43001006U) /**< \brief (AC) Interrupt Flag Status and Clear */
#define REG_AC_STATUSA             (*(RoReg8 *)0x43001007U) /**< \brief (AC) Status A */
#define REG_AC_STATUSB             (*(RoReg8 *)0x43001008U) /**< \brief (AC) Status B */
#define REG_AC_DBGCTRL             (*(RwReg8 *)0x43001009U) /**< \brief (AC) Debug Control */
#define REG_AC_WINCTRL             (*(RwReg8 *)0x4300100AU) /**< \brief (AC) Window Control */
#define REG_AC_SCALER0             (*(RwReg8 *)0x4300100CU) /**< \brief (AC) Scaler 0 */
#define REG_AC_SCALER1             (*(RwReg8 *)0x4300100DU) /**< \brief (AC) Scaler 1 */
#define REG_AC_COMPCTRL0           (*(RwReg  *)0x43001010U) /**< \brief (AC) Comparator Control 0 */
#define REG_AC_COMPCTRL1           (*(RwReg  *)0x43001014U) /**< \brief (AC) Comparator Control 1 */
#define REG_AC_SYNCBUSY            (*(RoReg  *)0x43001020U) /**< \brief (AC) Synchronization Busy */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for AC peripheral ========== */
#define AC_COMPCTRL_MUXNEG_OPAMP    7        // OPAMP selection for MUXNEG
#define AC_GCLK_ID                  31       // Index of Generic Clock
#define AC_NUM_CMP                  2        // Number of comparators
#define AC_PAIRS                    1        // Number of pairs of comparators

#endif /* _SAMR30_AC_INSTANCE_ */
