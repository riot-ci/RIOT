/**************************************************************************//**
 * @file efr32mg1p_af_pins.h
 * @brief EFR32MG1P_AF_PINS register and bit field definitions
 * @version 5.4.0
 ******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.@n
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.@n
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Laboratories, Inc.
 * has no obligation to support this Software. Silicon Laboratories, Inc. is
 * providing the Software "AS IS", with no express or implied warranties of any
 * kind, including, but not limited to, any implied warranties of
 * merchantability or fitness for any particular purpose or warranties against
 * infringement of any proprietary rights of a third party.
 *
 * Silicon Laboratories, Inc. will not be liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this Software.
 *
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__ICCARM__)
#pragma system_include       /* Treat file as system include file. */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang system_header  /* Treat file as system include file. */
#endif

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @addtogroup EFR32MG1P_Alternate_Function Alternate Function
 * @{
 * @defgroup EFR32MG1P_AF_Pins Alternate Function Pins
 * @{
 *****************************************************************************/

#define AF_CMU_CLK0_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 15 : (i) == 2 ? 6 : (i) == 3 ? 11 : (i) == 4 ? 9 : (i) == 5 ? 14 : (i) == 6 ? 2 : (i) == 7 ? 7 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                         /**< Pin number for AF_CMU_CLK0 location number i */
#define AF_CMU_CLK1_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 14 : (i) == 2 ? 7 : (i) == 3 ? 10 : (i) == 4 ? 10 : (i) == 5 ? 15 : (i) == 6 ? 3 : (i) == 7 ? 6 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                        /**< Pin number for AF_CMU_CLK1 location number i */
#define AF_PRS_CH0_PIN(i)          ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 6 : (i) == 7 ? 7 : (i) == 8 ? 6 : (i) == 9 ? 7 : (i) == 10 ? 8 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 :  -1)                                                                                                                                                                                                                                                                                                            /**< Pin number for AF_PRS_CH0 location number i */
#define AF_PRS_CH1_PIN(i)          ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 6 : (i) == 6 ? 7 : (i) == 7 ? 0 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                            /**< Pin number for AF_PRS_CH1 location number i */
#define AF_PRS_CH2_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 6 : (i) == 5 ? 7 : (i) == 6 ? 0 : (i) == 7 ? 1 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                            /**< Pin number for AF_PRS_CH2 location number i */
#define AF_PRS_CH3_PIN(i)          ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 6 : (i) == 4 ? 7 : (i) == 5 ? 0 : (i) == 6 ? 1 : (i) == 7 ? 2 : (i) == 8 ? 9 : (i) == 9 ? 10 : (i) == 10 ? 11 : (i) == 11 ? 12 : (i) == 12 ? 13 : (i) == 13 ? 14 : (i) == 14 ? 15 :  -1)                                                                                                                                                                                                                                                                                        /**< Pin number for AF_PRS_CH3 location number i */
#define AF_PRS_CH4_PIN(i)          ((i) == 0 ? 9 : (i) == 1 ? 10 : (i) == 2 ? 11 : (i) == 3 ? 12 : (i) == 4 ? 13 : (i) == 5 ? 14 : (i) == 6 ? 15 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_PRS_CH4 location number i */
#define AF_PRS_CH5_PIN(i)          ((i) == 0 ? 10 : (i) == 1 ? 11 : (i) == 2 ? 12 : (i) == 3 ? 13 : (i) == 4 ? 14 : (i) == 5 ? 15 : (i) == 6 ? 9 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_PRS_CH5 location number i */
#define AF_PRS_CH6_PIN(i)          ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 : (i) == 14 ? 12 : (i) == 15 ? 13 : (i) == 16 ? 14 : (i) == 17 ? 15 :  -1)                                                                                                                                                                                                                                   /**< Pin number for AF_PRS_CH6 location number i */
#define AF_PRS_CH7_PIN(i)          ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 0 :  -1)                                                                                                                                                                                                                                                                                                                                                         /**< Pin number for AF_PRS_CH7 location number i */
#define AF_PRS_CH8_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 12 : (i) == 6 ? 13 : (i) == 7 ? 14 : (i) == 8 ? 15 : (i) == 9 ? 0 : (i) == 10 ? 1 :  -1)                                                                                                                                                                                                                                                                                                                                                         /**< Pin number for AF_PRS_CH8 location number i */
#define AF_PRS_CH9_PIN(i)          ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 14 : (i) == 7 ? 15 : (i) == 8 ? 0 : (i) == 9 ? 1 : (i) == 10 ? 2 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 :  -1)                                                                                                                                                                                                                                                       /**< Pin number for AF_PRS_CH9 location number i */
#define AF_PRS_CH10_PIN(i)         ((i) == 0 ? 6 : (i) == 1 ? 7 : (i) == 2 ? 8 : (i) == 3 ? 9 : (i) == 4 ? 10 : (i) == 5 ? 11 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Pin number for AF_PRS_CH10 location number i */
#define AF_PRS_CH11_PIN(i)         ((i) == 0 ? 7 : (i) == 1 ? 8 : (i) == 2 ? 9 : (i) == 3 ? 10 : (i) == 4 ? 11 : (i) == 5 ? 6 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                        /**< Pin number for AF_PRS_CH11 location number i */
#define AF_TIMER0_CC0_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_TIMER0_CC0 location number i */
#define AF_TIMER0_CC1_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_TIMER0_CC1 location number i */
#define AF_TIMER0_CC2_PIN(i)       ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 12 : (i) == 6 ? 13 : (i) == 7 ? 14 : (i) == 8 ? 15 : (i) == 9 ? 6 : (i) == 10 ? 7 : (i) == 11 ? 8 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 9 : (i) == 16 ? 10 : (i) == 17 ? 11 : (i) == 18 ? 12 : (i) == 19 ? 13 : (i) == 20 ? 14 : (i) == 21 ? 15 : (i) == 22 ? 0 : (i) == 23 ? 1 : (i) == 24 ? 2 : (i) == 25 ? 3 : (i) == 26 ? 4 : (i) == 27 ? 5 : (i) == 28 ? 6 : (i) == 29 ? 7 : (i) == 30 ? 0 : (i) == 31 ? 1 :  -1) /**< Pin number for AF_TIMER0_CC2 location number i */
#define AF_TIMER0_CC3_PIN(i)       (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER0_CC3 location number i */
#define AF_TIMER0_CDTI0_PIN(i)     ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 14 : (i) == 7 ? 15 : (i) == 8 ? 6 : (i) == 9 ? 7 : (i) == 10 ? 8 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 12 : (i) == 18 ? 13 : (i) == 19 ? 14 : (i) == 20 ? 15 : (i) == 21 ? 0 : (i) == 22 ? 1 : (i) == 23 ? 2 : (i) == 24 ? 3 : (i) == 25 ? 4 : (i) == 26 ? 5 : (i) == 27 ? 6 : (i) == 28 ? 7 : (i) == 29 ? 0 : (i) == 30 ? 1 : (i) == 31 ? 2 :  -1) /**< Pin number for AF_TIMER0_CDTI0 location number i */
#define AF_TIMER0_CDTI1_PIN(i)     ((i) == 0 ? 4 : (i) == 1 ? 5 : (i) == 2 ? 11 : (i) == 3 ? 12 : (i) == 4 ? 13 : (i) == 5 ? 14 : (i) == 6 ? 15 : (i) == 7 ? 6 : (i) == 8 ? 7 : (i) == 9 ? 8 : (i) == 10 ? 9 : (i) == 11 ? 10 : (i) == 12 ? 11 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 12 : (i) == 17 ? 13 : (i) == 18 ? 14 : (i) == 19 ? 15 : (i) == 20 ? 0 : (i) == 21 ? 1 : (i) == 22 ? 2 : (i) == 23 ? 3 : (i) == 24 ? 4 : (i) == 25 ? 5 : (i) == 26 ? 6 : (i) == 27 ? 7 : (i) == 28 ? 0 : (i) == 29 ? 1 : (i) == 30 ? 2 : (i) == 31 ? 3 :  -1) /**< Pin number for AF_TIMER0_CDTI1 location number i */
#define AF_TIMER0_CDTI2_PIN(i)     ((i) == 0 ? 5 : (i) == 1 ? 11 : (i) == 2 ? 12 : (i) == 3 ? 13 : (i) == 4 ? 14 : (i) == 5 ? 15 : (i) == 6 ? 6 : (i) == 7 ? 7 : (i) == 8 ? 8 : (i) == 9 ? 9 : (i) == 10 ? 10 : (i) == 11 ? 11 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 12 : (i) == 16 ? 13 : (i) == 17 ? 14 : (i) == 18 ? 15 : (i) == 19 ? 0 : (i) == 20 ? 1 : (i) == 21 ? 2 : (i) == 22 ? 3 : (i) == 23 ? 4 : (i) == 24 ? 5 : (i) == 25 ? 6 : (i) == 26 ? 7 : (i) == 27 ? 0 : (i) == 28 ? 1 : (i) == 29 ? 2 : (i) == 30 ? 3 : (i) == 31 ? 4 :  -1) /**< Pin number for AF_TIMER0_CDTI2 location number i */
#define AF_TIMER0_CDTI3_PIN(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER0_CDTI3 location number i */
#define AF_TIMER1_CC0_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_TIMER1_CC0 location number i */
#define AF_TIMER1_CC1_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_TIMER1_CC1 location number i */
#define AF_TIMER1_CC2_PIN(i)       ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 12 : (i) == 6 ? 13 : (i) == 7 ? 14 : (i) == 8 ? 15 : (i) == 9 ? 6 : (i) == 10 ? 7 : (i) == 11 ? 8 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 9 : (i) == 16 ? 10 : (i) == 17 ? 11 : (i) == 18 ? 12 : (i) == 19 ? 13 : (i) == 20 ? 14 : (i) == 21 ? 15 : (i) == 22 ? 0 : (i) == 23 ? 1 : (i) == 24 ? 2 : (i) == 25 ? 3 : (i) == 26 ? 4 : (i) == 27 ? 5 : (i) == 28 ? 6 : (i) == 29 ? 7 : (i) == 30 ? 0 : (i) == 31 ? 1 :  -1) /**< Pin number for AF_TIMER1_CC2 location number i */
#define AF_TIMER1_CC3_PIN(i)       ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 14 : (i) == 7 ? 15 : (i) == 8 ? 6 : (i) == 9 ? 7 : (i) == 10 ? 8 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 12 : (i) == 18 ? 13 : (i) == 19 ? 14 : (i) == 20 ? 15 : (i) == 21 ? 0 : (i) == 22 ? 1 : (i) == 23 ? 2 : (i) == 24 ? 3 : (i) == 25 ? 4 : (i) == 26 ? 5 : (i) == 27 ? 6 : (i) == 28 ? 7 : (i) == 29 ? 0 : (i) == 30 ? 1 : (i) == 31 ? 2 :  -1) /**< Pin number for AF_TIMER1_CC3 location number i */
#define AF_TIMER1_CDTI0_PIN(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER1_CDTI0 location number i */
#define AF_TIMER1_CDTI1_PIN(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER1_CDTI1 location number i */
#define AF_TIMER1_CDTI2_PIN(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER1_CDTI2 location number i */
#define AF_TIMER1_CDTI3_PIN(i)     (-1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_TIMER1_CDTI3 location number i */
#define AF_USART0_TX_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_USART0_TX location number i */
#define AF_USART0_RX_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_USART0_RX location number i */
#define AF_USART0_CLK_PIN(i)       ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 12 : (i) == 6 ? 13 : (i) == 7 ? 14 : (i) == 8 ? 15 : (i) == 9 ? 6 : (i) == 10 ? 7 : (i) == 11 ? 8 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 9 : (i) == 16 ? 10 : (i) == 17 ? 11 : (i) == 18 ? 12 : (i) == 19 ? 13 : (i) == 20 ? 14 : (i) == 21 ? 15 : (i) == 22 ? 0 : (i) == 23 ? 1 : (i) == 24 ? 2 : (i) == 25 ? 3 : (i) == 26 ? 4 : (i) == 27 ? 5 : (i) == 28 ? 6 : (i) == 29 ? 7 : (i) == 30 ? 0 : (i) == 31 ? 1 :  -1) /**< Pin number for AF_USART0_CLK location number i */
#define AF_USART0_CS_PIN(i)        ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 14 : (i) == 7 ? 15 : (i) == 8 ? 6 : (i) == 9 ? 7 : (i) == 10 ? 8 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 12 : (i) == 18 ? 13 : (i) == 19 ? 14 : (i) == 20 ? 15 : (i) == 21 ? 0 : (i) == 22 ? 1 : (i) == 23 ? 2 : (i) == 24 ? 3 : (i) == 25 ? 4 : (i) == 26 ? 5 : (i) == 27 ? 6 : (i) == 28 ? 7 : (i) == 29 ? 0 : (i) == 30 ? 1 : (i) == 31 ? 2 :  -1) /**< Pin number for AF_USART0_CS location number i */
#define AF_USART0_CTS_PIN(i)       ((i) == 0 ? 4 : (i) == 1 ? 5 : (i) == 2 ? 11 : (i) == 3 ? 12 : (i) == 4 ? 13 : (i) == 5 ? 14 : (i) == 6 ? 15 : (i) == 7 ? 6 : (i) == 8 ? 7 : (i) == 9 ? 8 : (i) == 10 ? 9 : (i) == 11 ? 10 : (i) == 12 ? 11 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 12 : (i) == 17 ? 13 : (i) == 18 ? 14 : (i) == 19 ? 15 : (i) == 20 ? 0 : (i) == 21 ? 1 : (i) == 22 ? 2 : (i) == 23 ? 3 : (i) == 24 ? 4 : (i) == 25 ? 5 : (i) == 26 ? 6 : (i) == 27 ? 7 : (i) == 28 ? 0 : (i) == 29 ? 1 : (i) == 30 ? 2 : (i) == 31 ? 3 :  -1) /**< Pin number for AF_USART0_CTS location number i */
#define AF_USART0_RTS_PIN(i)       ((i) == 0 ? 5 : (i) == 1 ? 11 : (i) == 2 ? 12 : (i) == 3 ? 13 : (i) == 4 ? 14 : (i) == 5 ? 15 : (i) == 6 ? 6 : (i) == 7 ? 7 : (i) == 8 ? 8 : (i) == 9 ? 9 : (i) == 10 ? 10 : (i) == 11 ? 11 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 12 : (i) == 16 ? 13 : (i) == 17 ? 14 : (i) == 18 ? 15 : (i) == 19 ? 0 : (i) == 20 ? 1 : (i) == 21 ? 2 : (i) == 22 ? 3 : (i) == 23 ? 4 : (i) == 24 ? 5 : (i) == 25 ? 6 : (i) == 26 ? 7 : (i) == 27 ? 0 : (i) == 28 ? 1 : (i) == 29 ? 2 : (i) == 30 ? 3 : (i) == 31 ? 4 :  -1) /**< Pin number for AF_USART0_RTS location number i */
#define AF_USART1_TX_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_USART1_TX location number i */
#define AF_USART1_RX_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_USART1_RX location number i */
#define AF_USART1_CLK_PIN(i)       ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 12 : (i) == 6 ? 13 : (i) == 7 ? 14 : (i) == 8 ? 15 : (i) == 9 ? 6 : (i) == 10 ? 7 : (i) == 11 ? 8 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 9 : (i) == 16 ? 10 : (i) == 17 ? 11 : (i) == 18 ? 12 : (i) == 19 ? 13 : (i) == 20 ? 14 : (i) == 21 ? 15 : (i) == 22 ? 0 : (i) == 23 ? 1 : (i) == 24 ? 2 : (i) == 25 ? 3 : (i) == 26 ? 4 : (i) == 27 ? 5 : (i) == 28 ? 6 : (i) == 29 ? 7 : (i) == 30 ? 0 : (i) == 31 ? 1 :  -1) /**< Pin number for AF_USART1_CLK location number i */
#define AF_USART1_CS_PIN(i)        ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 14 : (i) == 7 ? 15 : (i) == 8 ? 6 : (i) == 9 ? 7 : (i) == 10 ? 8 : (i) == 11 ? 9 : (i) == 12 ? 10 : (i) == 13 ? 11 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 12 : (i) == 18 ? 13 : (i) == 19 ? 14 : (i) == 20 ? 15 : (i) == 21 ? 0 : (i) == 22 ? 1 : (i) == 23 ? 2 : (i) == 24 ? 3 : (i) == 25 ? 4 : (i) == 26 ? 5 : (i) == 27 ? 6 : (i) == 28 ? 7 : (i) == 29 ? 0 : (i) == 30 ? 1 : (i) == 31 ? 2 :  -1) /**< Pin number for AF_USART1_CS location number i */
#define AF_USART1_CTS_PIN(i)       ((i) == 0 ? 4 : (i) == 1 ? 5 : (i) == 2 ? 11 : (i) == 3 ? 12 : (i) == 4 ? 13 : (i) == 5 ? 14 : (i) == 6 ? 15 : (i) == 7 ? 6 : (i) == 8 ? 7 : (i) == 9 ? 8 : (i) == 10 ? 9 : (i) == 11 ? 10 : (i) == 12 ? 11 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 12 : (i) == 17 ? 13 : (i) == 18 ? 14 : (i) == 19 ? 15 : (i) == 20 ? 0 : (i) == 21 ? 1 : (i) == 22 ? 2 : (i) == 23 ? 3 : (i) == 24 ? 4 : (i) == 25 ? 5 : (i) == 26 ? 6 : (i) == 27 ? 7 : (i) == 28 ? 0 : (i) == 29 ? 1 : (i) == 30 ? 2 : (i) == 31 ? 3 :  -1) /**< Pin number for AF_USART1_CTS location number i */
#define AF_USART1_RTS_PIN(i)       ((i) == 0 ? 5 : (i) == 1 ? 11 : (i) == 2 ? 12 : (i) == 3 ? 13 : (i) == 4 ? 14 : (i) == 5 ? 15 : (i) == 6 ? 6 : (i) == 7 ? 7 : (i) == 8 ? 8 : (i) == 9 ? 9 : (i) == 10 ? 10 : (i) == 11 ? 11 : (i) == 12 ? 9 : (i) == 13 ? 10 : (i) == 14 ? 11 : (i) == 15 ? 12 : (i) == 16 ? 13 : (i) == 17 ? 14 : (i) == 18 ? 15 : (i) == 19 ? 0 : (i) == 20 ? 1 : (i) == 21 ? 2 : (i) == 22 ? 3 : (i) == 23 ? 4 : (i) == 24 ? 5 : (i) == 25 ? 6 : (i) == 26 ? 7 : (i) == 27 ? 0 : (i) == 28 ? 1 : (i) == 29 ? 2 : (i) == 30 ? 3 : (i) == 31 ? 4 :  -1) /**< Pin number for AF_USART1_RTS location number i */
#define AF_LEUART0_TX_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_LEUART0_TX location number i */
#define AF_LEUART0_RX_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_LEUART0_RX location number i */
#define AF_LETIMER0_OUT0_PIN(i)    ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_LETIMER0_OUT0 location number i */
#define AF_LETIMER0_OUT1_PIN(i)    ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_LETIMER0_OUT1 location number i */
#define AF_PCNT0_S0IN_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_PCNT0_S0IN location number i */
#define AF_PCNT0_S1IN_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_PCNT0_S1IN location number i */
#define AF_I2C0_SDA_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_I2C0_SDA location number i */
#define AF_I2C0_SCL_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 5 : (i) == 5 ? 11 : (i) == 6 ? 12 : (i) == 7 ? 13 : (i) == 8 ? 14 : (i) == 9 ? 15 : (i) == 10 ? 6 : (i) == 11 ? 7 : (i) == 12 ? 8 : (i) == 13 ? 9 : (i) == 14 ? 10 : (i) == 15 ? 11 : (i) == 16 ? 9 : (i) == 17 ? 10 : (i) == 18 ? 11 : (i) == 19 ? 12 : (i) == 20 ? 13 : (i) == 21 ? 14 : (i) == 22 ? 15 : (i) == 23 ? 0 : (i) == 24 ? 1 : (i) == 25 ? 2 : (i) == 26 ? 3 : (i) == 27 ? 4 : (i) == 28 ? 5 : (i) == 29 ? 6 : (i) == 30 ? 7 : (i) == 31 ? 0 :  -1) /**< Pin number for AF_I2C0_SCL location number i */
#define AF_ACMP0_OUT_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_ACMP0_OUT location number i */
#define AF_ACMP1_OUT_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 4 : (i) == 5 ? 5 : (i) == 6 ? 11 : (i) == 7 ? 12 : (i) == 8 ? 13 : (i) == 9 ? 14 : (i) == 10 ? 15 : (i) == 11 ? 6 : (i) == 12 ? 7 : (i) == 13 ? 8 : (i) == 14 ? 9 : (i) == 15 ? 10 : (i) == 16 ? 11 : (i) == 17 ? 9 : (i) == 18 ? 10 : (i) == 19 ? 11 : (i) == 20 ? 12 : (i) == 21 ? 13 : (i) == 22 ? 14 : (i) == 23 ? 15 : (i) == 24 ? 0 : (i) == 25 ? 1 : (i) == 26 ? 2 : (i) == 27 ? 3 : (i) == 28 ? 4 : (i) == 29 ? 5 : (i) == 30 ? 6 : (i) == 31 ? 7 :  -1) /**< Pin number for AF_ACMP1_OUT location number i */
#define AF_DBG_TDI_PIN(i)          ((i) == 0 ? 3 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_DBG_TDI location number i */
#define AF_DBG_TDO_PIN(i)          ((i) == 0 ? 2 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_DBG_TDO location number i */
#define AF_DBG_SWV_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 13 : (i) == 2 ? 15 : (i) == 3 ? 11 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_DBG_SWV location number i */
#define AF_DBG_SWDIOTMS_PIN(i)     ((i) == 0 ? 1 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_DBG_SWDIOTMS location number i */
#define AF_DBG_SWCLKTCK_PIN(i)     ((i) == 0 ? 0 :  -1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     /**< Pin number for AF_DBG_SWCLKTCK location number i */

/** @} */
/** @} End of group EFR32MG1P_AF_Pins */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

