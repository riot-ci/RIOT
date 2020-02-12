/**
 * \file
 *
 * \brief Peripheral I/O description for SAML21E17B
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAML21E17B_PIO_
#define _SAML21E17B_PIO_

#define PIN_PA00                            0  /**< \brief Pin Number for PA00 */
#define PORT_PA00              (_UL_(1) <<  0) /**< \brief PORT Mask  for PA00 */
#define PIN_PA01                            1  /**< \brief Pin Number for PA01 */
#define PORT_PA01              (_UL_(1) <<  1) /**< \brief PORT Mask  for PA01 */
#define PIN_PA02                            2  /**< \brief Pin Number for PA02 */
#define PORT_PA02              (_UL_(1) <<  2) /**< \brief PORT Mask  for PA02 */
#define PIN_PA03                            3  /**< \brief Pin Number for PA03 */
#define PORT_PA03              (_UL_(1) <<  3) /**< \brief PORT Mask  for PA03 */
#define PIN_PA04                            4  /**< \brief Pin Number for PA04 */
#define PORT_PA04              (_UL_(1) <<  4) /**< \brief PORT Mask  for PA04 */
#define PIN_PA05                            5  /**< \brief Pin Number for PA05 */
#define PORT_PA05              (_UL_(1) <<  5) /**< \brief PORT Mask  for PA05 */
#define PIN_PA06                            6  /**< \brief Pin Number for PA06 */
#define PORT_PA06              (_UL_(1) <<  6) /**< \brief PORT Mask  for PA06 */
#define PIN_PA07                            7  /**< \brief Pin Number for PA07 */
#define PORT_PA07              (_UL_(1) <<  7) /**< \brief PORT Mask  for PA07 */
#define PIN_PA08                            8  /**< \brief Pin Number for PA08 */
#define PORT_PA08              (_UL_(1) <<  8) /**< \brief PORT Mask  for PA08 */
#define PIN_PA09                            9  /**< \brief Pin Number for PA09 */
#define PORT_PA09              (_UL_(1) <<  9) /**< \brief PORT Mask  for PA09 */
#define PIN_PA10                           10  /**< \brief Pin Number for PA10 */
#define PORT_PA10              (_UL_(1) << 10) /**< \brief PORT Mask  for PA10 */
#define PIN_PA11                           11  /**< \brief Pin Number for PA11 */
#define PORT_PA11              (_UL_(1) << 11) /**< \brief PORT Mask  for PA11 */
#define PIN_PA14                           14  /**< \brief Pin Number for PA14 */
#define PORT_PA14              (_UL_(1) << 14) /**< \brief PORT Mask  for PA14 */
#define PIN_PA15                           15  /**< \brief Pin Number for PA15 */
#define PORT_PA15              (_UL_(1) << 15) /**< \brief PORT Mask  for PA15 */
#define PIN_PA16                           16  /**< \brief Pin Number for PA16 */
#define PORT_PA16              (_UL_(1) << 16) /**< \brief PORT Mask  for PA16 */
#define PIN_PA17                           17  /**< \brief Pin Number for PA17 */
#define PORT_PA17              (_UL_(1) << 17) /**< \brief PORT Mask  for PA17 */
#define PIN_PA18                           18  /**< \brief Pin Number for PA18 */
#define PORT_PA18              (_UL_(1) << 18) /**< \brief PORT Mask  for PA18 */
#define PIN_PA19                           19  /**< \brief Pin Number for PA19 */
#define PORT_PA19              (_UL_(1) << 19) /**< \brief PORT Mask  for PA19 */
#define PIN_PA22                           22  /**< \brief Pin Number for PA22 */
#define PORT_PA22              (_UL_(1) << 22) /**< \brief PORT Mask  for PA22 */
#define PIN_PA23                           23  /**< \brief Pin Number for PA23 */
#define PORT_PA23              (_UL_(1) << 23) /**< \brief PORT Mask  for PA23 */
#define PIN_PA24                           24  /**< \brief Pin Number for PA24 */
#define PORT_PA24              (_UL_(1) << 24) /**< \brief PORT Mask  for PA24 */
#define PIN_PA25                           25  /**< \brief Pin Number for PA25 */
#define PORT_PA25              (_UL_(1) << 25) /**< \brief PORT Mask  for PA25 */
#define PIN_PA27                           27  /**< \brief Pin Number for PA27 */
#define PORT_PA27              (_UL_(1) << 27) /**< \brief PORT Mask  for PA27 */
#define PIN_PA30                           30  /**< \brief Pin Number for PA30 */
#define PORT_PA30              (_UL_(1) << 30) /**< \brief PORT Mask  for PA30 */
#define PIN_PA31                           31  /**< \brief Pin Number for PA31 */
#define PORT_PA31              (_UL_(1) << 31) /**< \brief PORT Mask  for PA31 */
/* ========== PORT definition for RSTC peripheral ========== */
#define PIN_PA00A_RSTC_EXTWAKE0         _L_(0) /**< \brief RSTC signal: EXTWAKE0 on PA00 mux A */
#define MUX_PA00A_RSTC_EXTWAKE0         _L_(0)
#define PINMUX_PA00A_RSTC_EXTWAKE0  ((PIN_PA00A_RSTC_EXTWAKE0 << 16) | MUX_PA00A_RSTC_EXTWAKE0)
#define PORT_PA00A_RSTC_EXTWAKE0  (_UL_(1) <<  0)
#define PIN_PA01A_RSTC_EXTWAKE1         _L_(1) /**< \brief RSTC signal: EXTWAKE1 on PA01 mux A */
#define MUX_PA01A_RSTC_EXTWAKE1         _L_(0)
#define PINMUX_PA01A_RSTC_EXTWAKE1  ((PIN_PA01A_RSTC_EXTWAKE1 << 16) | MUX_PA01A_RSTC_EXTWAKE1)
#define PORT_PA01A_RSTC_EXTWAKE1  (_UL_(1) <<  1)
#define PIN_PA02A_RSTC_EXTWAKE2         _L_(2) /**< \brief RSTC signal: EXTWAKE2 on PA02 mux A */
#define MUX_PA02A_RSTC_EXTWAKE2         _L_(0)
#define PINMUX_PA02A_RSTC_EXTWAKE2  ((PIN_PA02A_RSTC_EXTWAKE2 << 16) | MUX_PA02A_RSTC_EXTWAKE2)
#define PORT_PA02A_RSTC_EXTWAKE2  (_UL_(1) <<  2)
#define PIN_PA03A_RSTC_EXTWAKE3         _L_(3) /**< \brief RSTC signal: EXTWAKE3 on PA03 mux A */
#define MUX_PA03A_RSTC_EXTWAKE3         _L_(0)
#define PINMUX_PA03A_RSTC_EXTWAKE3  ((PIN_PA03A_RSTC_EXTWAKE3 << 16) | MUX_PA03A_RSTC_EXTWAKE3)
#define PORT_PA03A_RSTC_EXTWAKE3  (_UL_(1) <<  3)
#define PIN_PA04A_RSTC_EXTWAKE4         _L_(4) /**< \brief RSTC signal: EXTWAKE4 on PA04 mux A */
#define MUX_PA04A_RSTC_EXTWAKE4         _L_(0)
#define PINMUX_PA04A_RSTC_EXTWAKE4  ((PIN_PA04A_RSTC_EXTWAKE4 << 16) | MUX_PA04A_RSTC_EXTWAKE4)
#define PORT_PA04A_RSTC_EXTWAKE4  (_UL_(1) <<  4)
#define PIN_PA05A_RSTC_EXTWAKE5         _L_(5) /**< \brief RSTC signal: EXTWAKE5 on PA05 mux A */
#define MUX_PA05A_RSTC_EXTWAKE5         _L_(0)
#define PINMUX_PA05A_RSTC_EXTWAKE5  ((PIN_PA05A_RSTC_EXTWAKE5 << 16) | MUX_PA05A_RSTC_EXTWAKE5)
#define PORT_PA05A_RSTC_EXTWAKE5  (_UL_(1) <<  5)
#define PIN_PA06A_RSTC_EXTWAKE6         _L_(6) /**< \brief RSTC signal: EXTWAKE6 on PA06 mux A */
#define MUX_PA06A_RSTC_EXTWAKE6         _L_(0)
#define PINMUX_PA06A_RSTC_EXTWAKE6  ((PIN_PA06A_RSTC_EXTWAKE6 << 16) | MUX_PA06A_RSTC_EXTWAKE6)
#define PORT_PA06A_RSTC_EXTWAKE6  (_UL_(1) <<  6)
#define PIN_PA07A_RSTC_EXTWAKE7         _L_(7) /**< \brief RSTC signal: EXTWAKE7 on PA07 mux A */
#define MUX_PA07A_RSTC_EXTWAKE7         _L_(0)
#define PINMUX_PA07A_RSTC_EXTWAKE7  ((PIN_PA07A_RSTC_EXTWAKE7 << 16) | MUX_PA07A_RSTC_EXTWAKE7)
#define PORT_PA07A_RSTC_EXTWAKE7  (_UL_(1) <<  7)
/* ========== PORT definition for GCLK peripheral ========== */
#define PIN_PA14H_GCLK_IO0             _L_(14) /**< \brief GCLK signal: IO0 on PA14 mux H */
#define MUX_PA14H_GCLK_IO0              _L_(7)
#define PINMUX_PA14H_GCLK_IO0      ((PIN_PA14H_GCLK_IO0 << 16) | MUX_PA14H_GCLK_IO0)
#define PORT_PA14H_GCLK_IO0    (_UL_(1) << 14)
#define PIN_PA27H_GCLK_IO0             _L_(27) /**< \brief GCLK signal: IO0 on PA27 mux H */
#define MUX_PA27H_GCLK_IO0              _L_(7)
#define PINMUX_PA27H_GCLK_IO0      ((PIN_PA27H_GCLK_IO0 << 16) | MUX_PA27H_GCLK_IO0)
#define PORT_PA27H_GCLK_IO0    (_UL_(1) << 27)
#define PIN_PA30H_GCLK_IO0             _L_(30) /**< \brief GCLK signal: IO0 on PA30 mux H */
#define MUX_PA30H_GCLK_IO0              _L_(7)
#define PINMUX_PA30H_GCLK_IO0      ((PIN_PA30H_GCLK_IO0 << 16) | MUX_PA30H_GCLK_IO0)
#define PORT_PA30H_GCLK_IO0    (_UL_(1) << 30)
#define PIN_PA15H_GCLK_IO1             _L_(15) /**< \brief GCLK signal: IO1 on PA15 mux H */
#define MUX_PA15H_GCLK_IO1              _L_(7)
#define PINMUX_PA15H_GCLK_IO1      ((PIN_PA15H_GCLK_IO1 << 16) | MUX_PA15H_GCLK_IO1)
#define PORT_PA15H_GCLK_IO1    (_UL_(1) << 15)
#define PIN_PA16H_GCLK_IO2             _L_(16) /**< \brief GCLK signal: IO2 on PA16 mux H */
#define MUX_PA16H_GCLK_IO2              _L_(7)
#define PINMUX_PA16H_GCLK_IO2      ((PIN_PA16H_GCLK_IO2 << 16) | MUX_PA16H_GCLK_IO2)
#define PORT_PA16H_GCLK_IO2    (_UL_(1) << 16)
#define PIN_PA17H_GCLK_IO3             _L_(17) /**< \brief GCLK signal: IO3 on PA17 mux H */
#define MUX_PA17H_GCLK_IO3              _L_(7)
#define PINMUX_PA17H_GCLK_IO3      ((PIN_PA17H_GCLK_IO3 << 16) | MUX_PA17H_GCLK_IO3)
#define PORT_PA17H_GCLK_IO3    (_UL_(1) << 17)
#define PIN_PA10H_GCLK_IO4             _L_(10) /**< \brief GCLK signal: IO4 on PA10 mux H */
#define MUX_PA10H_GCLK_IO4              _L_(7)
#define PINMUX_PA10H_GCLK_IO4      ((PIN_PA10H_GCLK_IO4 << 16) | MUX_PA10H_GCLK_IO4)
#define PORT_PA10H_GCLK_IO4    (_UL_(1) << 10)
#define PIN_PA11H_GCLK_IO5             _L_(11) /**< \brief GCLK signal: IO5 on PA11 mux H */
#define MUX_PA11H_GCLK_IO5              _L_(7)
#define PINMUX_PA11H_GCLK_IO5      ((PIN_PA11H_GCLK_IO5 << 16) | MUX_PA11H_GCLK_IO5)
#define PORT_PA11H_GCLK_IO5    (_UL_(1) << 11)
#define PIN_PA22H_GCLK_IO6             _L_(22) /**< \brief GCLK signal: IO6 on PA22 mux H */
#define MUX_PA22H_GCLK_IO6              _L_(7)
#define PINMUX_PA22H_GCLK_IO6      ((PIN_PA22H_GCLK_IO6 << 16) | MUX_PA22H_GCLK_IO6)
#define PORT_PA22H_GCLK_IO6    (_UL_(1) << 22)
#define PIN_PA23H_GCLK_IO7             _L_(23) /**< \brief GCLK signal: IO7 on PA23 mux H */
#define MUX_PA23H_GCLK_IO7              _L_(7)
#define PINMUX_PA23H_GCLK_IO7      ((PIN_PA23H_GCLK_IO7 << 16) | MUX_PA23H_GCLK_IO7)
#define PORT_PA23H_GCLK_IO7    (_UL_(1) << 23)
/* ========== PORT definition for EIC peripheral ========== */
#define PIN_PA16A_EIC_EXTINT0          _L_(16) /**< \brief EIC signal: EXTINT0 on PA16 mux A */
#define MUX_PA16A_EIC_EXTINT0           _L_(0)
#define PINMUX_PA16A_EIC_EXTINT0   ((PIN_PA16A_EIC_EXTINT0 << 16) | MUX_PA16A_EIC_EXTINT0)
#define PORT_PA16A_EIC_EXTINT0  (_UL_(1) << 16)
#define PIN_PA16A_EIC_EXTINT_NUM        _L_(0) /**< \brief EIC signal: PIN_PA16 External Interrupt Line */
#define PIN_PA00A_EIC_EXTINT0           _L_(0) /**< \brief EIC signal: EXTINT0 on PA00 mux A */
#define MUX_PA00A_EIC_EXTINT0           _L_(0)
#define PINMUX_PA00A_EIC_EXTINT0   ((PIN_PA00A_EIC_EXTINT0 << 16) | MUX_PA00A_EIC_EXTINT0)
#define PORT_PA00A_EIC_EXTINT0  (_UL_(1) <<  0)
#define PIN_PA00A_EIC_EXTINT_NUM        _L_(0) /**< \brief EIC signal: PIN_PA00 External Interrupt Line */
#define PIN_PA17A_EIC_EXTINT1          _L_(17) /**< \brief EIC signal: EXTINT1 on PA17 mux A */
#define MUX_PA17A_EIC_EXTINT1           _L_(0)
#define PINMUX_PA17A_EIC_EXTINT1   ((PIN_PA17A_EIC_EXTINT1 << 16) | MUX_PA17A_EIC_EXTINT1)
#define PORT_PA17A_EIC_EXTINT1  (_UL_(1) << 17)
#define PIN_PA17A_EIC_EXTINT_NUM        _L_(1) /**< \brief EIC signal: PIN_PA17 External Interrupt Line */
#define PIN_PA01A_EIC_EXTINT1           _L_(1) /**< \brief EIC signal: EXTINT1 on PA01 mux A */
#define MUX_PA01A_EIC_EXTINT1           _L_(0)
#define PINMUX_PA01A_EIC_EXTINT1   ((PIN_PA01A_EIC_EXTINT1 << 16) | MUX_PA01A_EIC_EXTINT1)
#define PORT_PA01A_EIC_EXTINT1  (_UL_(1) <<  1)
#define PIN_PA01A_EIC_EXTINT_NUM        _L_(1) /**< \brief EIC signal: PIN_PA01 External Interrupt Line */
#define PIN_PA02A_EIC_EXTINT2           _L_(2) /**< \brief EIC signal: EXTINT2 on PA02 mux A */
#define MUX_PA02A_EIC_EXTINT2           _L_(0)
#define PINMUX_PA02A_EIC_EXTINT2   ((PIN_PA02A_EIC_EXTINT2 << 16) | MUX_PA02A_EIC_EXTINT2)
#define PORT_PA02A_EIC_EXTINT2  (_UL_(1) <<  2)
#define PIN_PA02A_EIC_EXTINT_NUM        _L_(2) /**< \brief EIC signal: PIN_PA02 External Interrupt Line */
#define PIN_PA18A_EIC_EXTINT2          _L_(18) /**< \brief EIC signal: EXTINT2 on PA18 mux A */
#define MUX_PA18A_EIC_EXTINT2           _L_(0)
#define PINMUX_PA18A_EIC_EXTINT2   ((PIN_PA18A_EIC_EXTINT2 << 16) | MUX_PA18A_EIC_EXTINT2)
#define PORT_PA18A_EIC_EXTINT2  (_UL_(1) << 18)
#define PIN_PA18A_EIC_EXTINT_NUM        _L_(2) /**< \brief EIC signal: PIN_PA18 External Interrupt Line */
#define PIN_PA03A_EIC_EXTINT3           _L_(3) /**< \brief EIC signal: EXTINT3 on PA03 mux A */
#define MUX_PA03A_EIC_EXTINT3           _L_(0)
#define PINMUX_PA03A_EIC_EXTINT3   ((PIN_PA03A_EIC_EXTINT3 << 16) | MUX_PA03A_EIC_EXTINT3)
#define PORT_PA03A_EIC_EXTINT3  (_UL_(1) <<  3)
#define PIN_PA03A_EIC_EXTINT_NUM        _L_(3) /**< \brief EIC signal: PIN_PA03 External Interrupt Line */
#define PIN_PA19A_EIC_EXTINT3          _L_(19) /**< \brief EIC signal: EXTINT3 on PA19 mux A */
#define MUX_PA19A_EIC_EXTINT3           _L_(0)
#define PINMUX_PA19A_EIC_EXTINT3   ((PIN_PA19A_EIC_EXTINT3 << 16) | MUX_PA19A_EIC_EXTINT3)
#define PORT_PA19A_EIC_EXTINT3  (_UL_(1) << 19)
#define PIN_PA19A_EIC_EXTINT_NUM        _L_(3) /**< \brief EIC signal: PIN_PA19 External Interrupt Line */
#define PIN_PA04A_EIC_EXTINT4           _L_(4) /**< \brief EIC signal: EXTINT4 on PA04 mux A */
#define MUX_PA04A_EIC_EXTINT4           _L_(0)
#define PINMUX_PA04A_EIC_EXTINT4   ((PIN_PA04A_EIC_EXTINT4 << 16) | MUX_PA04A_EIC_EXTINT4)
#define PORT_PA04A_EIC_EXTINT4  (_UL_(1) <<  4)
#define PIN_PA04A_EIC_EXTINT_NUM        _L_(4) /**< \brief EIC signal: PIN_PA04 External Interrupt Line */
#define PIN_PA05A_EIC_EXTINT5           _L_(5) /**< \brief EIC signal: EXTINT5 on PA05 mux A */
#define MUX_PA05A_EIC_EXTINT5           _L_(0)
#define PINMUX_PA05A_EIC_EXTINT5   ((PIN_PA05A_EIC_EXTINT5 << 16) | MUX_PA05A_EIC_EXTINT5)
#define PORT_PA05A_EIC_EXTINT5  (_UL_(1) <<  5)
#define PIN_PA05A_EIC_EXTINT_NUM        _L_(5) /**< \brief EIC signal: PIN_PA05 External Interrupt Line */
#define PIN_PA06A_EIC_EXTINT6           _L_(6) /**< \brief EIC signal: EXTINT6 on PA06 mux A */
#define MUX_PA06A_EIC_EXTINT6           _L_(0)
#define PINMUX_PA06A_EIC_EXTINT6   ((PIN_PA06A_EIC_EXTINT6 << 16) | MUX_PA06A_EIC_EXTINT6)
#define PORT_PA06A_EIC_EXTINT6  (_UL_(1) <<  6)
#define PIN_PA06A_EIC_EXTINT_NUM        _L_(6) /**< \brief EIC signal: PIN_PA06 External Interrupt Line */
#define PIN_PA22A_EIC_EXTINT6          _L_(22) /**< \brief EIC signal: EXTINT6 on PA22 mux A */
#define MUX_PA22A_EIC_EXTINT6           _L_(0)
#define PINMUX_PA22A_EIC_EXTINT6   ((PIN_PA22A_EIC_EXTINT6 << 16) | MUX_PA22A_EIC_EXTINT6)
#define PORT_PA22A_EIC_EXTINT6  (_UL_(1) << 22)
#define PIN_PA22A_EIC_EXTINT_NUM        _L_(6) /**< \brief EIC signal: PIN_PA22 External Interrupt Line */
#define PIN_PA07A_EIC_EXTINT7           _L_(7) /**< \brief EIC signal: EXTINT7 on PA07 mux A */
#define MUX_PA07A_EIC_EXTINT7           _L_(0)
#define PINMUX_PA07A_EIC_EXTINT7   ((PIN_PA07A_EIC_EXTINT7 << 16) | MUX_PA07A_EIC_EXTINT7)
#define PORT_PA07A_EIC_EXTINT7  (_UL_(1) <<  7)
#define PIN_PA07A_EIC_EXTINT_NUM        _L_(7) /**< \brief EIC signal: PIN_PA07 External Interrupt Line */
#define PIN_PA23A_EIC_EXTINT7          _L_(23) /**< \brief EIC signal: EXTINT7 on PA23 mux A */
#define MUX_PA23A_EIC_EXTINT7           _L_(0)
#define PINMUX_PA23A_EIC_EXTINT7   ((PIN_PA23A_EIC_EXTINT7 << 16) | MUX_PA23A_EIC_EXTINT7)
#define PORT_PA23A_EIC_EXTINT7  (_UL_(1) << 23)
#define PIN_PA23A_EIC_EXTINT_NUM        _L_(7) /**< \brief EIC signal: PIN_PA23 External Interrupt Line */
#define PIN_PA09A_EIC_EXTINT9           _L_(9) /**< \brief EIC signal: EXTINT9 on PA09 mux A */
#define MUX_PA09A_EIC_EXTINT9           _L_(0)
#define PINMUX_PA09A_EIC_EXTINT9   ((PIN_PA09A_EIC_EXTINT9 << 16) | MUX_PA09A_EIC_EXTINT9)
#define PORT_PA09A_EIC_EXTINT9  (_UL_(1) <<  9)
#define PIN_PA09A_EIC_EXTINT_NUM        _L_(9) /**< \brief EIC signal: PIN_PA09 External Interrupt Line */
#define PIN_PA10A_EIC_EXTINT10         _L_(10) /**< \brief EIC signal: EXTINT10 on PA10 mux A */
#define MUX_PA10A_EIC_EXTINT10          _L_(0)
#define PINMUX_PA10A_EIC_EXTINT10  ((PIN_PA10A_EIC_EXTINT10 << 16) | MUX_PA10A_EIC_EXTINT10)
#define PORT_PA10A_EIC_EXTINT10  (_UL_(1) << 10)
#define PIN_PA10A_EIC_EXTINT_NUM       _L_(10) /**< \brief EIC signal: PIN_PA10 External Interrupt Line */
#define PIN_PA30A_EIC_EXTINT10         _L_(30) /**< \brief EIC signal: EXTINT10 on PA30 mux A */
#define MUX_PA30A_EIC_EXTINT10          _L_(0)
#define PINMUX_PA30A_EIC_EXTINT10  ((PIN_PA30A_EIC_EXTINT10 << 16) | MUX_PA30A_EIC_EXTINT10)
#define PORT_PA30A_EIC_EXTINT10  (_UL_(1) << 30)
#define PIN_PA30A_EIC_EXTINT_NUM       _L_(10) /**< \brief EIC signal: PIN_PA30 External Interrupt Line */
#define PIN_PA11A_EIC_EXTINT11         _L_(11) /**< \brief EIC signal: EXTINT11 on PA11 mux A */
#define MUX_PA11A_EIC_EXTINT11          _L_(0)
#define PINMUX_PA11A_EIC_EXTINT11  ((PIN_PA11A_EIC_EXTINT11 << 16) | MUX_PA11A_EIC_EXTINT11)
#define PORT_PA11A_EIC_EXTINT11  (_UL_(1) << 11)
#define PIN_PA11A_EIC_EXTINT_NUM       _L_(11) /**< \brief EIC signal: PIN_PA11 External Interrupt Line */
#define PIN_PA31A_EIC_EXTINT11         _L_(31) /**< \brief EIC signal: EXTINT11 on PA31 mux A */
#define MUX_PA31A_EIC_EXTINT11          _L_(0)
#define PINMUX_PA31A_EIC_EXTINT11  ((PIN_PA31A_EIC_EXTINT11 << 16) | MUX_PA31A_EIC_EXTINT11)
#define PORT_PA31A_EIC_EXTINT11  (_UL_(1) << 31)
#define PIN_PA31A_EIC_EXTINT_NUM       _L_(11) /**< \brief EIC signal: PIN_PA31 External Interrupt Line */
#define PIN_PA24A_EIC_EXTINT12         _L_(24) /**< \brief EIC signal: EXTINT12 on PA24 mux A */
#define MUX_PA24A_EIC_EXTINT12          _L_(0)
#define PINMUX_PA24A_EIC_EXTINT12  ((PIN_PA24A_EIC_EXTINT12 << 16) | MUX_PA24A_EIC_EXTINT12)
#define PORT_PA24A_EIC_EXTINT12  (_UL_(1) << 24)
#define PIN_PA24A_EIC_EXTINT_NUM       _L_(12) /**< \brief EIC signal: PIN_PA24 External Interrupt Line */
#define PIN_PA25A_EIC_EXTINT13         _L_(25) /**< \brief EIC signal: EXTINT13 on PA25 mux A */
#define MUX_PA25A_EIC_EXTINT13          _L_(0)
#define PINMUX_PA25A_EIC_EXTINT13  ((PIN_PA25A_EIC_EXTINT13 << 16) | MUX_PA25A_EIC_EXTINT13)
#define PORT_PA25A_EIC_EXTINT13  (_UL_(1) << 25)
#define PIN_PA25A_EIC_EXTINT_NUM       _L_(13) /**< \brief EIC signal: PIN_PA25 External Interrupt Line */
#define PIN_PA14A_EIC_EXTINT14         _L_(14) /**< \brief EIC signal: EXTINT14 on PA14 mux A */
#define MUX_PA14A_EIC_EXTINT14          _L_(0)
#define PINMUX_PA14A_EIC_EXTINT14  ((PIN_PA14A_EIC_EXTINT14 << 16) | MUX_PA14A_EIC_EXTINT14)
#define PORT_PA14A_EIC_EXTINT14  (_UL_(1) << 14)
#define PIN_PA14A_EIC_EXTINT_NUM       _L_(14) /**< \brief EIC signal: PIN_PA14 External Interrupt Line */
#define PIN_PA27A_EIC_EXTINT15         _L_(27) /**< \brief EIC signal: EXTINT15 on PA27 mux A */
#define MUX_PA27A_EIC_EXTINT15          _L_(0)
#define PINMUX_PA27A_EIC_EXTINT15  ((PIN_PA27A_EIC_EXTINT15 << 16) | MUX_PA27A_EIC_EXTINT15)
#define PORT_PA27A_EIC_EXTINT15  (_UL_(1) << 27)
#define PIN_PA27A_EIC_EXTINT_NUM       _L_(15) /**< \brief EIC signal: PIN_PA27 External Interrupt Line */
#define PIN_PA15A_EIC_EXTINT15         _L_(15) /**< \brief EIC signal: EXTINT15 on PA15 mux A */
#define MUX_PA15A_EIC_EXTINT15          _L_(0)
#define PINMUX_PA15A_EIC_EXTINT15  ((PIN_PA15A_EIC_EXTINT15 << 16) | MUX_PA15A_EIC_EXTINT15)
#define PORT_PA15A_EIC_EXTINT15  (_UL_(1) << 15)
#define PIN_PA15A_EIC_EXTINT_NUM       _L_(15) /**< \brief EIC signal: PIN_PA15 External Interrupt Line */
#define PIN_PA08A_EIC_NMI               _L_(8) /**< \brief EIC signal: NMI on PA08 mux A */
#define MUX_PA08A_EIC_NMI               _L_(0)
#define PINMUX_PA08A_EIC_NMI       ((PIN_PA08A_EIC_NMI << 16) | MUX_PA08A_EIC_NMI)
#define PORT_PA08A_EIC_NMI     (_UL_(1) <<  8)
/* ========== PORT definition for USB peripheral ========== */
#define PIN_PA24G_USB_DM               _L_(24) /**< \brief USB signal: DM on PA24 mux G */
#define MUX_PA24G_USB_DM                _L_(6)
#define PINMUX_PA24G_USB_DM        ((PIN_PA24G_USB_DM << 16) | MUX_PA24G_USB_DM)
#define PORT_PA24G_USB_DM      (_UL_(1) << 24)
#define PIN_PA25G_USB_DP               _L_(25) /**< \brief USB signal: DP on PA25 mux G */
#define MUX_PA25G_USB_DP                _L_(6)
#define PINMUX_PA25G_USB_DP        ((PIN_PA25G_USB_DP << 16) | MUX_PA25G_USB_DP)
#define PORT_PA25G_USB_DP      (_UL_(1) << 25)
#define PIN_PA23G_USB_SOF_1KHZ         _L_(23) /**< \brief USB signal: SOF_1KHZ on PA23 mux G */
#define MUX_PA23G_USB_SOF_1KHZ          _L_(6)
#define PINMUX_PA23G_USB_SOF_1KHZ  ((PIN_PA23G_USB_SOF_1KHZ << 16) | MUX_PA23G_USB_SOF_1KHZ)
#define PORT_PA23G_USB_SOF_1KHZ  (_UL_(1) << 23)
/* ========== PORT definition for SERCOM0 peripheral ========== */
#define PIN_PA04D_SERCOM0_PAD0          _L_(4) /**< \brief SERCOM0 signal: PAD0 on PA04 mux D */
#define MUX_PA04D_SERCOM0_PAD0          _L_(3)
#define PINMUX_PA04D_SERCOM0_PAD0  ((PIN_PA04D_SERCOM0_PAD0 << 16) | MUX_PA04D_SERCOM0_PAD0)
#define PORT_PA04D_SERCOM0_PAD0  (_UL_(1) <<  4)
#define PIN_PA08C_SERCOM0_PAD0          _L_(8) /**< \brief SERCOM0 signal: PAD0 on PA08 mux C */
#define MUX_PA08C_SERCOM0_PAD0          _L_(2)
#define PINMUX_PA08C_SERCOM0_PAD0  ((PIN_PA08C_SERCOM0_PAD0 << 16) | MUX_PA08C_SERCOM0_PAD0)
#define PORT_PA08C_SERCOM0_PAD0  (_UL_(1) <<  8)
#define PIN_PA05D_SERCOM0_PAD1          _L_(5) /**< \brief SERCOM0 signal: PAD1 on PA05 mux D */
#define MUX_PA05D_SERCOM0_PAD1          _L_(3)
#define PINMUX_PA05D_SERCOM0_PAD1  ((PIN_PA05D_SERCOM0_PAD1 << 16) | MUX_PA05D_SERCOM0_PAD1)
#define PORT_PA05D_SERCOM0_PAD1  (_UL_(1) <<  5)
#define PIN_PA09C_SERCOM0_PAD1          _L_(9) /**< \brief SERCOM0 signal: PAD1 on PA09 mux C */
#define MUX_PA09C_SERCOM0_PAD1          _L_(2)
#define PINMUX_PA09C_SERCOM0_PAD1  ((PIN_PA09C_SERCOM0_PAD1 << 16) | MUX_PA09C_SERCOM0_PAD1)
#define PORT_PA09C_SERCOM0_PAD1  (_UL_(1) <<  9)
#define PIN_PA06D_SERCOM0_PAD2          _L_(6) /**< \brief SERCOM0 signal: PAD2 on PA06 mux D */
#define MUX_PA06D_SERCOM0_PAD2          _L_(3)
#define PINMUX_PA06D_SERCOM0_PAD2  ((PIN_PA06D_SERCOM0_PAD2 << 16) | MUX_PA06D_SERCOM0_PAD2)
#define PORT_PA06D_SERCOM0_PAD2  (_UL_(1) <<  6)
#define PIN_PA10C_SERCOM0_PAD2         _L_(10) /**< \brief SERCOM0 signal: PAD2 on PA10 mux C */
#define MUX_PA10C_SERCOM0_PAD2          _L_(2)
#define PINMUX_PA10C_SERCOM0_PAD2  ((PIN_PA10C_SERCOM0_PAD2 << 16) | MUX_PA10C_SERCOM0_PAD2)
#define PORT_PA10C_SERCOM0_PAD2  (_UL_(1) << 10)
#define PIN_PA07D_SERCOM0_PAD3          _L_(7) /**< \brief SERCOM0 signal: PAD3 on PA07 mux D */
#define MUX_PA07D_SERCOM0_PAD3          _L_(3)
#define PINMUX_PA07D_SERCOM0_PAD3  ((PIN_PA07D_SERCOM0_PAD3 << 16) | MUX_PA07D_SERCOM0_PAD3)
#define PORT_PA07D_SERCOM0_PAD3  (_UL_(1) <<  7)
#define PIN_PA11C_SERCOM0_PAD3         _L_(11) /**< \brief SERCOM0 signal: PAD3 on PA11 mux C */
#define MUX_PA11C_SERCOM0_PAD3          _L_(2)
#define PINMUX_PA11C_SERCOM0_PAD3  ((PIN_PA11C_SERCOM0_PAD3 << 16) | MUX_PA11C_SERCOM0_PAD3)
#define PORT_PA11C_SERCOM0_PAD3  (_UL_(1) << 11)
/* ========== PORT definition for SERCOM1 peripheral ========== */
#define PIN_PA16C_SERCOM1_PAD0         _L_(16) /**< \brief SERCOM1 signal: PAD0 on PA16 mux C */
#define MUX_PA16C_SERCOM1_PAD0          _L_(2)
#define PINMUX_PA16C_SERCOM1_PAD0  ((PIN_PA16C_SERCOM1_PAD0 << 16) | MUX_PA16C_SERCOM1_PAD0)
#define PORT_PA16C_SERCOM1_PAD0  (_UL_(1) << 16)
#define PIN_PA00D_SERCOM1_PAD0          _L_(0) /**< \brief SERCOM1 signal: PAD0 on PA00 mux D */
#define MUX_PA00D_SERCOM1_PAD0          _L_(3)
#define PINMUX_PA00D_SERCOM1_PAD0  ((PIN_PA00D_SERCOM1_PAD0 << 16) | MUX_PA00D_SERCOM1_PAD0)
#define PORT_PA00D_SERCOM1_PAD0  (_UL_(1) <<  0)
#define PIN_PA17C_SERCOM1_PAD1         _L_(17) /**< \brief SERCOM1 signal: PAD1 on PA17 mux C */
#define MUX_PA17C_SERCOM1_PAD1          _L_(2)
#define PINMUX_PA17C_SERCOM1_PAD1  ((PIN_PA17C_SERCOM1_PAD1 << 16) | MUX_PA17C_SERCOM1_PAD1)
#define PORT_PA17C_SERCOM1_PAD1  (_UL_(1) << 17)
#define PIN_PA01D_SERCOM1_PAD1          _L_(1) /**< \brief SERCOM1 signal: PAD1 on PA01 mux D */
#define MUX_PA01D_SERCOM1_PAD1          _L_(3)
#define PINMUX_PA01D_SERCOM1_PAD1  ((PIN_PA01D_SERCOM1_PAD1 << 16) | MUX_PA01D_SERCOM1_PAD1)
#define PORT_PA01D_SERCOM1_PAD1  (_UL_(1) <<  1)
#define PIN_PA30D_SERCOM1_PAD2         _L_(30) /**< \brief SERCOM1 signal: PAD2 on PA30 mux D */
#define MUX_PA30D_SERCOM1_PAD2          _L_(3)
#define PINMUX_PA30D_SERCOM1_PAD2  ((PIN_PA30D_SERCOM1_PAD2 << 16) | MUX_PA30D_SERCOM1_PAD2)
#define PORT_PA30D_SERCOM1_PAD2  (_UL_(1) << 30)
#define PIN_PA18C_SERCOM1_PAD2         _L_(18) /**< \brief SERCOM1 signal: PAD2 on PA18 mux C */
#define MUX_PA18C_SERCOM1_PAD2          _L_(2)
#define PINMUX_PA18C_SERCOM1_PAD2  ((PIN_PA18C_SERCOM1_PAD2 << 16) | MUX_PA18C_SERCOM1_PAD2)
#define PORT_PA18C_SERCOM1_PAD2  (_UL_(1) << 18)
#define PIN_PA31D_SERCOM1_PAD3         _L_(31) /**< \brief SERCOM1 signal: PAD3 on PA31 mux D */
#define MUX_PA31D_SERCOM1_PAD3          _L_(3)
#define PINMUX_PA31D_SERCOM1_PAD3  ((PIN_PA31D_SERCOM1_PAD3 << 16) | MUX_PA31D_SERCOM1_PAD3)
#define PORT_PA31D_SERCOM1_PAD3  (_UL_(1) << 31)
#define PIN_PA19C_SERCOM1_PAD3         _L_(19) /**< \brief SERCOM1 signal: PAD3 on PA19 mux C */
#define MUX_PA19C_SERCOM1_PAD3          _L_(2)
#define PINMUX_PA19C_SERCOM1_PAD3  ((PIN_PA19C_SERCOM1_PAD3 << 16) | MUX_PA19C_SERCOM1_PAD3)
#define PORT_PA19C_SERCOM1_PAD3  (_UL_(1) << 19)
/* ========== PORT definition for SERCOM2 peripheral ========== */
#define PIN_PA08D_SERCOM2_PAD0          _L_(8) /**< \brief SERCOM2 signal: PAD0 on PA08 mux D */
#define MUX_PA08D_SERCOM2_PAD0          _L_(3)
#define PINMUX_PA08D_SERCOM2_PAD0  ((PIN_PA08D_SERCOM2_PAD0 << 16) | MUX_PA08D_SERCOM2_PAD0)
#define PORT_PA08D_SERCOM2_PAD0  (_UL_(1) <<  8)
#define PIN_PA09D_SERCOM2_PAD1          _L_(9) /**< \brief SERCOM2 signal: PAD1 on PA09 mux D */
#define MUX_PA09D_SERCOM2_PAD1          _L_(3)
#define PINMUX_PA09D_SERCOM2_PAD1  ((PIN_PA09D_SERCOM2_PAD1 << 16) | MUX_PA09D_SERCOM2_PAD1)
#define PORT_PA09D_SERCOM2_PAD1  (_UL_(1) <<  9)
#define PIN_PA10D_SERCOM2_PAD2         _L_(10) /**< \brief SERCOM2 signal: PAD2 on PA10 mux D */
#define MUX_PA10D_SERCOM2_PAD2          _L_(3)
#define PINMUX_PA10D_SERCOM2_PAD2  ((PIN_PA10D_SERCOM2_PAD2 << 16) | MUX_PA10D_SERCOM2_PAD2)
#define PORT_PA10D_SERCOM2_PAD2  (_UL_(1) << 10)
#define PIN_PA14C_SERCOM2_PAD2         _L_(14) /**< \brief SERCOM2 signal: PAD2 on PA14 mux C */
#define MUX_PA14C_SERCOM2_PAD2          _L_(2)
#define PINMUX_PA14C_SERCOM2_PAD2  ((PIN_PA14C_SERCOM2_PAD2 << 16) | MUX_PA14C_SERCOM2_PAD2)
#define PORT_PA14C_SERCOM2_PAD2  (_UL_(1) << 14)
#define PIN_PA11D_SERCOM2_PAD3         _L_(11) /**< \brief SERCOM2 signal: PAD3 on PA11 mux D */
#define MUX_PA11D_SERCOM2_PAD3          _L_(3)
#define PINMUX_PA11D_SERCOM2_PAD3  ((PIN_PA11D_SERCOM2_PAD3 << 16) | MUX_PA11D_SERCOM2_PAD3)
#define PORT_PA11D_SERCOM2_PAD3  (_UL_(1) << 11)
#define PIN_PA15C_SERCOM2_PAD3         _L_(15) /**< \brief SERCOM2 signal: PAD3 on PA15 mux C */
#define MUX_PA15C_SERCOM2_PAD3          _L_(2)
#define PINMUX_PA15C_SERCOM2_PAD3  ((PIN_PA15C_SERCOM2_PAD3 << 16) | MUX_PA15C_SERCOM2_PAD3)
#define PORT_PA15C_SERCOM2_PAD3  (_UL_(1) << 15)
/* ========== PORT definition for SERCOM3 peripheral ========== */
#define PIN_PA16D_SERCOM3_PAD0         _L_(16) /**< \brief SERCOM3 signal: PAD0 on PA16 mux D */
#define MUX_PA16D_SERCOM3_PAD0          _L_(3)
#define PINMUX_PA16D_SERCOM3_PAD0  ((PIN_PA16D_SERCOM3_PAD0 << 16) | MUX_PA16D_SERCOM3_PAD0)
#define PORT_PA16D_SERCOM3_PAD0  (_UL_(1) << 16)
#define PIN_PA22C_SERCOM3_PAD0         _L_(22) /**< \brief SERCOM3 signal: PAD0 on PA22 mux C */
#define MUX_PA22C_SERCOM3_PAD0          _L_(2)
#define PINMUX_PA22C_SERCOM3_PAD0  ((PIN_PA22C_SERCOM3_PAD0 << 16) | MUX_PA22C_SERCOM3_PAD0)
#define PORT_PA22C_SERCOM3_PAD0  (_UL_(1) << 22)
#define PIN_PA17D_SERCOM3_PAD1         _L_(17) /**< \brief SERCOM3 signal: PAD1 on PA17 mux D */
#define MUX_PA17D_SERCOM3_PAD1          _L_(3)
#define PINMUX_PA17D_SERCOM3_PAD1  ((PIN_PA17D_SERCOM3_PAD1 << 16) | MUX_PA17D_SERCOM3_PAD1)
#define PORT_PA17D_SERCOM3_PAD1  (_UL_(1) << 17)
#define PIN_PA23C_SERCOM3_PAD1         _L_(23) /**< \brief SERCOM3 signal: PAD1 on PA23 mux C */
#define MUX_PA23C_SERCOM3_PAD1          _L_(2)
#define PINMUX_PA23C_SERCOM3_PAD1  ((PIN_PA23C_SERCOM3_PAD1 << 16) | MUX_PA23C_SERCOM3_PAD1)
#define PORT_PA23C_SERCOM3_PAD1  (_UL_(1) << 23)
#define PIN_PA18D_SERCOM3_PAD2         _L_(18) /**< \brief SERCOM3 signal: PAD2 on PA18 mux D */
#define MUX_PA18D_SERCOM3_PAD2          _L_(3)
#define PINMUX_PA18D_SERCOM3_PAD2  ((PIN_PA18D_SERCOM3_PAD2 << 16) | MUX_PA18D_SERCOM3_PAD2)
#define PORT_PA18D_SERCOM3_PAD2  (_UL_(1) << 18)
#define PIN_PA24C_SERCOM3_PAD2         _L_(24) /**< \brief SERCOM3 signal: PAD2 on PA24 mux C */
#define MUX_PA24C_SERCOM3_PAD2          _L_(2)
#define PINMUX_PA24C_SERCOM3_PAD2  ((PIN_PA24C_SERCOM3_PAD2 << 16) | MUX_PA24C_SERCOM3_PAD2)
#define PORT_PA24C_SERCOM3_PAD2  (_UL_(1) << 24)
#define PIN_PA19D_SERCOM3_PAD3         _L_(19) /**< \brief SERCOM3 signal: PAD3 on PA19 mux D */
#define MUX_PA19D_SERCOM3_PAD3          _L_(3)
#define PINMUX_PA19D_SERCOM3_PAD3  ((PIN_PA19D_SERCOM3_PAD3 << 16) | MUX_PA19D_SERCOM3_PAD3)
#define PORT_PA19D_SERCOM3_PAD3  (_UL_(1) << 19)
#define PIN_PA25C_SERCOM3_PAD3         _L_(25) /**< \brief SERCOM3 signal: PAD3 on PA25 mux C */
#define MUX_PA25C_SERCOM3_PAD3          _L_(2)
#define PINMUX_PA25C_SERCOM3_PAD3  ((PIN_PA25C_SERCOM3_PAD3 << 16) | MUX_PA25C_SERCOM3_PAD3)
#define PORT_PA25C_SERCOM3_PAD3  (_UL_(1) << 25)
/* ========== PORT definition for SERCOM4 peripheral ========== */
#define PIN_PA14D_SERCOM4_PAD2         _L_(14) /**< \brief SERCOM4 signal: PAD2 on PA14 mux D */
#define MUX_PA14D_SERCOM4_PAD2          _L_(3)
#define PINMUX_PA14D_SERCOM4_PAD2  ((PIN_PA14D_SERCOM4_PAD2 << 16) | MUX_PA14D_SERCOM4_PAD2)
#define PORT_PA14D_SERCOM4_PAD2  (_UL_(1) << 14)
#define PIN_PA15D_SERCOM4_PAD3         _L_(15) /**< \brief SERCOM4 signal: PAD3 on PA15 mux D */
#define MUX_PA15D_SERCOM4_PAD3          _L_(3)
#define PINMUX_PA15D_SERCOM4_PAD3  ((PIN_PA15D_SERCOM4_PAD3 << 16) | MUX_PA15D_SERCOM4_PAD3)
#define PORT_PA15D_SERCOM4_PAD3  (_UL_(1) << 15)
/* ========== PORT definition for TCC0 peripheral ========== */
#define PIN_PA04E_TCC0_WO0              _L_(4) /**< \brief TCC0 signal: WO0 on PA04 mux E */
#define MUX_PA04E_TCC0_WO0              _L_(4)
#define PINMUX_PA04E_TCC0_WO0      ((PIN_PA04E_TCC0_WO0 << 16) | MUX_PA04E_TCC0_WO0)
#define PORT_PA04E_TCC0_WO0    (_UL_(1) <<  4)
#define PIN_PA08E_TCC0_WO0              _L_(8) /**< \brief TCC0 signal: WO0 on PA08 mux E */
#define MUX_PA08E_TCC0_WO0              _L_(4)
#define PINMUX_PA08E_TCC0_WO0      ((PIN_PA08E_TCC0_WO0 << 16) | MUX_PA08E_TCC0_WO0)
#define PORT_PA08E_TCC0_WO0    (_UL_(1) <<  8)
#define PIN_PA05E_TCC0_WO1              _L_(5) /**< \brief TCC0 signal: WO1 on PA05 mux E */
#define MUX_PA05E_TCC0_WO1              _L_(4)
#define PINMUX_PA05E_TCC0_WO1      ((PIN_PA05E_TCC0_WO1 << 16) | MUX_PA05E_TCC0_WO1)
#define PORT_PA05E_TCC0_WO1    (_UL_(1) <<  5)
#define PIN_PA09E_TCC0_WO1              _L_(9) /**< \brief TCC0 signal: WO1 on PA09 mux E */
#define MUX_PA09E_TCC0_WO1              _L_(4)
#define PINMUX_PA09E_TCC0_WO1      ((PIN_PA09E_TCC0_WO1 << 16) | MUX_PA09E_TCC0_WO1)
#define PORT_PA09E_TCC0_WO1    (_UL_(1) <<  9)
#define PIN_PA10F_TCC0_WO2             _L_(10) /**< \brief TCC0 signal: WO2 on PA10 mux F */
#define MUX_PA10F_TCC0_WO2              _L_(5)
#define PINMUX_PA10F_TCC0_WO2      ((PIN_PA10F_TCC0_WO2 << 16) | MUX_PA10F_TCC0_WO2)
#define PORT_PA10F_TCC0_WO2    (_UL_(1) << 10)
#define PIN_PA18F_TCC0_WO2             _L_(18) /**< \brief TCC0 signal: WO2 on PA18 mux F */
#define MUX_PA18F_TCC0_WO2              _L_(5)
#define PINMUX_PA18F_TCC0_WO2      ((PIN_PA18F_TCC0_WO2 << 16) | MUX_PA18F_TCC0_WO2)
#define PORT_PA18F_TCC0_WO2    (_UL_(1) << 18)
#define PIN_PA11F_TCC0_WO3             _L_(11) /**< \brief TCC0 signal: WO3 on PA11 mux F */
#define MUX_PA11F_TCC0_WO3              _L_(5)
#define PINMUX_PA11F_TCC0_WO3      ((PIN_PA11F_TCC0_WO3 << 16) | MUX_PA11F_TCC0_WO3)
#define PORT_PA11F_TCC0_WO3    (_UL_(1) << 11)
#define PIN_PA19F_TCC0_WO3             _L_(19) /**< \brief TCC0 signal: WO3 on PA19 mux F */
#define MUX_PA19F_TCC0_WO3              _L_(5)
#define PINMUX_PA19F_TCC0_WO3      ((PIN_PA19F_TCC0_WO3 << 16) | MUX_PA19F_TCC0_WO3)
#define PORT_PA19F_TCC0_WO3    (_UL_(1) << 19)
#define PIN_PA22F_TCC0_WO4             _L_(22) /**< \brief TCC0 signal: WO4 on PA22 mux F */
#define MUX_PA22F_TCC0_WO4              _L_(5)
#define PINMUX_PA22F_TCC0_WO4      ((PIN_PA22F_TCC0_WO4 << 16) | MUX_PA22F_TCC0_WO4)
#define PORT_PA22F_TCC0_WO4    (_UL_(1) << 22)
#define PIN_PA14F_TCC0_WO4             _L_(14) /**< \brief TCC0 signal: WO4 on PA14 mux F */
#define MUX_PA14F_TCC0_WO4              _L_(5)
#define PINMUX_PA14F_TCC0_WO4      ((PIN_PA14F_TCC0_WO4 << 16) | MUX_PA14F_TCC0_WO4)
#define PORT_PA14F_TCC0_WO4    (_UL_(1) << 14)
#define PIN_PA15F_TCC0_WO5             _L_(15) /**< \brief TCC0 signal: WO5 on PA15 mux F */
#define MUX_PA15F_TCC0_WO5              _L_(5)
#define PINMUX_PA15F_TCC0_WO5      ((PIN_PA15F_TCC0_WO5 << 16) | MUX_PA15F_TCC0_WO5)
#define PORT_PA15F_TCC0_WO5    (_UL_(1) << 15)
#define PIN_PA23F_TCC0_WO5             _L_(23) /**< \brief TCC0 signal: WO5 on PA23 mux F */
#define MUX_PA23F_TCC0_WO5              _L_(5)
#define PINMUX_PA23F_TCC0_WO5      ((PIN_PA23F_TCC0_WO5 << 16) | MUX_PA23F_TCC0_WO5)
#define PORT_PA23F_TCC0_WO5    (_UL_(1) << 23)
#define PIN_PA16F_TCC0_WO6             _L_(16) /**< \brief TCC0 signal: WO6 on PA16 mux F */
#define MUX_PA16F_TCC0_WO6              _L_(5)
#define PINMUX_PA16F_TCC0_WO6      ((PIN_PA16F_TCC0_WO6 << 16) | MUX_PA16F_TCC0_WO6)
#define PORT_PA16F_TCC0_WO6    (_UL_(1) << 16)
#define PIN_PA17F_TCC0_WO7             _L_(17) /**< \brief TCC0 signal: WO7 on PA17 mux F */
#define MUX_PA17F_TCC0_WO7              _L_(5)
#define PINMUX_PA17F_TCC0_WO7      ((PIN_PA17F_TCC0_WO7 << 16) | MUX_PA17F_TCC0_WO7)
#define PORT_PA17F_TCC0_WO7    (_UL_(1) << 17)
/* ========== PORT definition for TCC1 peripheral ========== */
#define PIN_PA06E_TCC1_WO0              _L_(6) /**< \brief TCC1 signal: WO0 on PA06 mux E */
#define MUX_PA06E_TCC1_WO0              _L_(4)
#define PINMUX_PA06E_TCC1_WO0      ((PIN_PA06E_TCC1_WO0 << 16) | MUX_PA06E_TCC1_WO0)
#define PORT_PA06E_TCC1_WO0    (_UL_(1) <<  6)
#define PIN_PA10E_TCC1_WO0             _L_(10) /**< \brief TCC1 signal: WO0 on PA10 mux E */
#define MUX_PA10E_TCC1_WO0              _L_(4)
#define PINMUX_PA10E_TCC1_WO0      ((PIN_PA10E_TCC1_WO0 << 16) | MUX_PA10E_TCC1_WO0)
#define PORT_PA10E_TCC1_WO0    (_UL_(1) << 10)
#define PIN_PA30E_TCC1_WO0             _L_(30) /**< \brief TCC1 signal: WO0 on PA30 mux E */
#define MUX_PA30E_TCC1_WO0              _L_(4)
#define PINMUX_PA30E_TCC1_WO0      ((PIN_PA30E_TCC1_WO0 << 16) | MUX_PA30E_TCC1_WO0)
#define PORT_PA30E_TCC1_WO0    (_UL_(1) << 30)
#define PIN_PA07E_TCC1_WO1              _L_(7) /**< \brief TCC1 signal: WO1 on PA07 mux E */
#define MUX_PA07E_TCC1_WO1              _L_(4)
#define PINMUX_PA07E_TCC1_WO1      ((PIN_PA07E_TCC1_WO1 << 16) | MUX_PA07E_TCC1_WO1)
#define PORT_PA07E_TCC1_WO1    (_UL_(1) <<  7)
#define PIN_PA11E_TCC1_WO1             _L_(11) /**< \brief TCC1 signal: WO1 on PA11 mux E */
#define MUX_PA11E_TCC1_WO1              _L_(4)
#define PINMUX_PA11E_TCC1_WO1      ((PIN_PA11E_TCC1_WO1 << 16) | MUX_PA11E_TCC1_WO1)
#define PORT_PA11E_TCC1_WO1    (_UL_(1) << 11)
#define PIN_PA31E_TCC1_WO1             _L_(31) /**< \brief TCC1 signal: WO1 on PA31 mux E */
#define MUX_PA31E_TCC1_WO1              _L_(4)
#define PINMUX_PA31E_TCC1_WO1      ((PIN_PA31E_TCC1_WO1 << 16) | MUX_PA31E_TCC1_WO1)
#define PORT_PA31E_TCC1_WO1    (_UL_(1) << 31)
#define PIN_PA08F_TCC1_WO2              _L_(8) /**< \brief TCC1 signal: WO2 on PA08 mux F */
#define MUX_PA08F_TCC1_WO2              _L_(5)
#define PINMUX_PA08F_TCC1_WO2      ((PIN_PA08F_TCC1_WO2 << 16) | MUX_PA08F_TCC1_WO2)
#define PORT_PA08F_TCC1_WO2    (_UL_(1) <<  8)
#define PIN_PA24F_TCC1_WO2             _L_(24) /**< \brief TCC1 signal: WO2 on PA24 mux F */
#define MUX_PA24F_TCC1_WO2              _L_(5)
#define PINMUX_PA24F_TCC1_WO2      ((PIN_PA24F_TCC1_WO2 << 16) | MUX_PA24F_TCC1_WO2)
#define PORT_PA24F_TCC1_WO2    (_UL_(1) << 24)
#define PIN_PA09F_TCC1_WO3              _L_(9) /**< \brief TCC1 signal: WO3 on PA09 mux F */
#define MUX_PA09F_TCC1_WO3              _L_(5)
#define PINMUX_PA09F_TCC1_WO3      ((PIN_PA09F_TCC1_WO3 << 16) | MUX_PA09F_TCC1_WO3)
#define PORT_PA09F_TCC1_WO3    (_UL_(1) <<  9)
#define PIN_PA25F_TCC1_WO3             _L_(25) /**< \brief TCC1 signal: WO3 on PA25 mux F */
#define MUX_PA25F_TCC1_WO3              _L_(5)
#define PINMUX_PA25F_TCC1_WO3      ((PIN_PA25F_TCC1_WO3 << 16) | MUX_PA25F_TCC1_WO3)
#define PORT_PA25F_TCC1_WO3    (_UL_(1) << 25)
/* ========== PORT definition for TCC2 peripheral ========== */
#define PIN_PA16E_TCC2_WO0             _L_(16) /**< \brief TCC2 signal: WO0 on PA16 mux E */
#define MUX_PA16E_TCC2_WO0              _L_(4)
#define PINMUX_PA16E_TCC2_WO0      ((PIN_PA16E_TCC2_WO0 << 16) | MUX_PA16E_TCC2_WO0)
#define PORT_PA16E_TCC2_WO0    (_UL_(1) << 16)
#define PIN_PA00E_TCC2_WO0              _L_(0) /**< \brief TCC2 signal: WO0 on PA00 mux E */
#define MUX_PA00E_TCC2_WO0              _L_(4)
#define PINMUX_PA00E_TCC2_WO0      ((PIN_PA00E_TCC2_WO0 << 16) | MUX_PA00E_TCC2_WO0)
#define PORT_PA00E_TCC2_WO0    (_UL_(1) <<  0)
#define PIN_PA17E_TCC2_WO1             _L_(17) /**< \brief TCC2 signal: WO1 on PA17 mux E */
#define MUX_PA17E_TCC2_WO1              _L_(4)
#define PINMUX_PA17E_TCC2_WO1      ((PIN_PA17E_TCC2_WO1 << 16) | MUX_PA17E_TCC2_WO1)
#define PORT_PA17E_TCC2_WO1    (_UL_(1) << 17)
#define PIN_PA01E_TCC2_WO1              _L_(1) /**< \brief TCC2 signal: WO1 on PA01 mux E */
#define MUX_PA01E_TCC2_WO1              _L_(4)
#define PINMUX_PA01E_TCC2_WO1      ((PIN_PA01E_TCC2_WO1 << 16) | MUX_PA01E_TCC2_WO1)
#define PORT_PA01E_TCC2_WO1    (_UL_(1) <<  1)
/* ========== PORT definition for TC0 peripheral ========== */
#define PIN_PA22E_TC0_WO0              _L_(22) /**< \brief TC0 signal: WO0 on PA22 mux E */
#define MUX_PA22E_TC0_WO0               _L_(4)
#define PINMUX_PA22E_TC0_WO0       ((PIN_PA22E_TC0_WO0 << 16) | MUX_PA22E_TC0_WO0)
#define PORT_PA22E_TC0_WO0     (_UL_(1) << 22)
#define PIN_PA23E_TC0_WO1              _L_(23) /**< \brief TC0 signal: WO1 on PA23 mux E */
#define MUX_PA23E_TC0_WO1               _L_(4)
#define PINMUX_PA23E_TC0_WO1       ((PIN_PA23E_TC0_WO1 << 16) | MUX_PA23E_TC0_WO1)
#define PORT_PA23E_TC0_WO1     (_UL_(1) << 23)
/* ========== PORT definition for TC1 peripheral ========== */
#define PIN_PA24E_TC1_WO0              _L_(24) /**< \brief TC1 signal: WO0 on PA24 mux E */
#define MUX_PA24E_TC1_WO0               _L_(4)
#define PINMUX_PA24E_TC1_WO0       ((PIN_PA24E_TC1_WO0 << 16) | MUX_PA24E_TC1_WO0)
#define PORT_PA24E_TC1_WO0     (_UL_(1) << 24)
#define PIN_PA25E_TC1_WO1              _L_(25) /**< \brief TC1 signal: WO1 on PA25 mux E */
#define MUX_PA25E_TC1_WO1               _L_(4)
#define PINMUX_PA25E_TC1_WO1       ((PIN_PA25E_TC1_WO1 << 16) | MUX_PA25E_TC1_WO1)
#define PORT_PA25E_TC1_WO1     (_UL_(1) << 25)
/* ========== PORT definition for DAC peripheral ========== */
#define PIN_PA02B_DAC_VOUT0             _L_(2) /**< \brief DAC signal: VOUT0 on PA02 mux B */
#define MUX_PA02B_DAC_VOUT0             _L_(1)
#define PINMUX_PA02B_DAC_VOUT0     ((PIN_PA02B_DAC_VOUT0 << 16) | MUX_PA02B_DAC_VOUT0)
#define PORT_PA02B_DAC_VOUT0   (_UL_(1) <<  2)
#define PIN_PA05B_DAC_VOUT1             _L_(5) /**< \brief DAC signal: VOUT1 on PA05 mux B */
#define MUX_PA05B_DAC_VOUT1             _L_(1)
#define PINMUX_PA05B_DAC_VOUT1     ((PIN_PA05B_DAC_VOUT1 << 16) | MUX_PA05B_DAC_VOUT1)
#define PORT_PA05B_DAC_VOUT1   (_UL_(1) <<  5)
#define PIN_PA03B_DAC_VREFP             _L_(3) /**< \brief DAC signal: VREFP on PA03 mux B */
#define MUX_PA03B_DAC_VREFP             _L_(1)
#define PINMUX_PA03B_DAC_VREFP     ((PIN_PA03B_DAC_VREFP << 16) | MUX_PA03B_DAC_VREFP)
#define PORT_PA03B_DAC_VREFP   (_UL_(1) <<  3)
/* ========== PORT definition for SERCOM5 peripheral ========== */
#define PIN_PA22D_SERCOM5_PAD0         _L_(22) /**< \brief SERCOM5 signal: PAD0 on PA22 mux D */
#define MUX_PA22D_SERCOM5_PAD0          _L_(3)
#define PINMUX_PA22D_SERCOM5_PAD0  ((PIN_PA22D_SERCOM5_PAD0 << 16) | MUX_PA22D_SERCOM5_PAD0)
#define PORT_PA22D_SERCOM5_PAD0  (_UL_(1) << 22)
#define PIN_PA23D_SERCOM5_PAD1         _L_(23) /**< \brief SERCOM5 signal: PAD1 on PA23 mux D */
#define MUX_PA23D_SERCOM5_PAD1          _L_(3)
#define PINMUX_PA23D_SERCOM5_PAD1  ((PIN_PA23D_SERCOM5_PAD1 << 16) | MUX_PA23D_SERCOM5_PAD1)
#define PORT_PA23D_SERCOM5_PAD1  (_UL_(1) << 23)
#define PIN_PA24D_SERCOM5_PAD2         _L_(24) /**< \brief SERCOM5 signal: PAD2 on PA24 mux D */
#define MUX_PA24D_SERCOM5_PAD2          _L_(3)
#define PINMUX_PA24D_SERCOM5_PAD2  ((PIN_PA24D_SERCOM5_PAD2 << 16) | MUX_PA24D_SERCOM5_PAD2)
#define PORT_PA24D_SERCOM5_PAD2  (_UL_(1) << 24)
#define PIN_PA25D_SERCOM5_PAD3         _L_(25) /**< \brief SERCOM5 signal: PAD3 on PA25 mux D */
#define MUX_PA25D_SERCOM5_PAD3          _L_(3)
#define PINMUX_PA25D_SERCOM5_PAD3  ((PIN_PA25D_SERCOM5_PAD3 << 16) | MUX_PA25D_SERCOM5_PAD3)
#define PORT_PA25D_SERCOM5_PAD3  (_UL_(1) << 25)
/* ========== PORT definition for TC4 peripheral ========== */
#define PIN_PA18E_TC4_WO0              _L_(18) /**< \brief TC4 signal: WO0 on PA18 mux E */
#define MUX_PA18E_TC4_WO0               _L_(4)
#define PINMUX_PA18E_TC4_WO0       ((PIN_PA18E_TC4_WO0 << 16) | MUX_PA18E_TC4_WO0)
#define PORT_PA18E_TC4_WO0     (_UL_(1) << 18)
#define PIN_PA14E_TC4_WO0              _L_(14) /**< \brief TC4 signal: WO0 on PA14 mux E */
#define MUX_PA14E_TC4_WO0               _L_(4)
#define PINMUX_PA14E_TC4_WO0       ((PIN_PA14E_TC4_WO0 << 16) | MUX_PA14E_TC4_WO0)
#define PORT_PA14E_TC4_WO0     (_UL_(1) << 14)
#define PIN_PA19E_TC4_WO1              _L_(19) /**< \brief TC4 signal: WO1 on PA19 mux E */
#define MUX_PA19E_TC4_WO1               _L_(4)
#define PINMUX_PA19E_TC4_WO1       ((PIN_PA19E_TC4_WO1 << 16) | MUX_PA19E_TC4_WO1)
#define PORT_PA19E_TC4_WO1     (_UL_(1) << 19)
#define PIN_PA15E_TC4_WO1              _L_(15) /**< \brief TC4 signal: WO1 on PA15 mux E */
#define MUX_PA15E_TC4_WO1               _L_(4)
#define PINMUX_PA15E_TC4_WO1       ((PIN_PA15E_TC4_WO1 << 16) | MUX_PA15E_TC4_WO1)
#define PORT_PA15E_TC4_WO1     (_UL_(1) << 15)
/* ========== PORT definition for ADC peripheral ========== */
#define PIN_PA02B_ADC_AIN0              _L_(2) /**< \brief ADC signal: AIN0 on PA02 mux B */
#define MUX_PA02B_ADC_AIN0              _L_(1)
#define PINMUX_PA02B_ADC_AIN0      ((PIN_PA02B_ADC_AIN0 << 16) | MUX_PA02B_ADC_AIN0)
#define PORT_PA02B_ADC_AIN0    (_UL_(1) <<  2)
#define PIN_PA03B_ADC_AIN1              _L_(3) /**< \brief ADC signal: AIN1 on PA03 mux B */
#define MUX_PA03B_ADC_AIN1              _L_(1)
#define PINMUX_PA03B_ADC_AIN1      ((PIN_PA03B_ADC_AIN1 << 16) | MUX_PA03B_ADC_AIN1)
#define PORT_PA03B_ADC_AIN1    (_UL_(1) <<  3)
#define PIN_PA04B_ADC_AIN4              _L_(4) /**< \brief ADC signal: AIN4 on PA04 mux B */
#define MUX_PA04B_ADC_AIN4              _L_(1)
#define PINMUX_PA04B_ADC_AIN4      ((PIN_PA04B_ADC_AIN4 << 16) | MUX_PA04B_ADC_AIN4)
#define PORT_PA04B_ADC_AIN4    (_UL_(1) <<  4)
#define PIN_PA05B_ADC_AIN5              _L_(5) /**< \brief ADC signal: AIN5 on PA05 mux B */
#define MUX_PA05B_ADC_AIN5              _L_(1)
#define PINMUX_PA05B_ADC_AIN5      ((PIN_PA05B_ADC_AIN5 << 16) | MUX_PA05B_ADC_AIN5)
#define PORT_PA05B_ADC_AIN5    (_UL_(1) <<  5)
#define PIN_PA06B_ADC_AIN6              _L_(6) /**< \brief ADC signal: AIN6 on PA06 mux B */
#define MUX_PA06B_ADC_AIN6              _L_(1)
#define PINMUX_PA06B_ADC_AIN6      ((PIN_PA06B_ADC_AIN6 << 16) | MUX_PA06B_ADC_AIN6)
#define PORT_PA06B_ADC_AIN6    (_UL_(1) <<  6)
#define PIN_PA07B_ADC_AIN7              _L_(7) /**< \brief ADC signal: AIN7 on PA07 mux B */
#define MUX_PA07B_ADC_AIN7              _L_(1)
#define PINMUX_PA07B_ADC_AIN7      ((PIN_PA07B_ADC_AIN7 << 16) | MUX_PA07B_ADC_AIN7)
#define PORT_PA07B_ADC_AIN7    (_UL_(1) <<  7)
#define PIN_PA08B_ADC_AIN16             _L_(8) /**< \brief ADC signal: AIN16 on PA08 mux B */
#define MUX_PA08B_ADC_AIN16             _L_(1)
#define PINMUX_PA08B_ADC_AIN16     ((PIN_PA08B_ADC_AIN16 << 16) | MUX_PA08B_ADC_AIN16)
#define PORT_PA08B_ADC_AIN16   (_UL_(1) <<  8)
#define PIN_PA09B_ADC_AIN17             _L_(9) /**< \brief ADC signal: AIN17 on PA09 mux B */
#define MUX_PA09B_ADC_AIN17             _L_(1)
#define PINMUX_PA09B_ADC_AIN17     ((PIN_PA09B_ADC_AIN17 << 16) | MUX_PA09B_ADC_AIN17)
#define PORT_PA09B_ADC_AIN17   (_UL_(1) <<  9)
#define PIN_PA10B_ADC_AIN18            _L_(10) /**< \brief ADC signal: AIN18 on PA10 mux B */
#define MUX_PA10B_ADC_AIN18             _L_(1)
#define PINMUX_PA10B_ADC_AIN18     ((PIN_PA10B_ADC_AIN18 << 16) | MUX_PA10B_ADC_AIN18)
#define PORT_PA10B_ADC_AIN18   (_UL_(1) << 10)
#define PIN_PA11B_ADC_AIN19            _L_(11) /**< \brief ADC signal: AIN19 on PA11 mux B */
#define MUX_PA11B_ADC_AIN19             _L_(1)
#define PINMUX_PA11B_ADC_AIN19     ((PIN_PA11B_ADC_AIN19 << 16) | MUX_PA11B_ADC_AIN19)
#define PORT_PA11B_ADC_AIN19   (_UL_(1) << 11)
#define PIN_PA04B_ADC_VREFP             _L_(4) /**< \brief ADC signal: VREFP on PA04 mux B */
#define MUX_PA04B_ADC_VREFP             _L_(1)
#define PINMUX_PA04B_ADC_VREFP     ((PIN_PA04B_ADC_VREFP << 16) | MUX_PA04B_ADC_VREFP)
#define PORT_PA04B_ADC_VREFP   (_UL_(1) <<  4)
/* ========== PORT definition for AC peripheral ========== */
#define PIN_PA04B_AC_AIN0               _L_(4) /**< \brief AC signal: AIN0 on PA04 mux B */
#define MUX_PA04B_AC_AIN0               _L_(1)
#define PINMUX_PA04B_AC_AIN0       ((PIN_PA04B_AC_AIN0 << 16) | MUX_PA04B_AC_AIN0)
#define PORT_PA04B_AC_AIN0     (_UL_(1) <<  4)
#define PIN_PA05B_AC_AIN1               _L_(5) /**< \brief AC signal: AIN1 on PA05 mux B */
#define MUX_PA05B_AC_AIN1               _L_(1)
#define PINMUX_PA05B_AC_AIN1       ((PIN_PA05B_AC_AIN1 << 16) | MUX_PA05B_AC_AIN1)
#define PORT_PA05B_AC_AIN1     (_UL_(1) <<  5)
#define PIN_PA06B_AC_AIN2               _L_(6) /**< \brief AC signal: AIN2 on PA06 mux B */
#define MUX_PA06B_AC_AIN2               _L_(1)
#define PINMUX_PA06B_AC_AIN2       ((PIN_PA06B_AC_AIN2 << 16) | MUX_PA06B_AC_AIN2)
#define PORT_PA06B_AC_AIN2     (_UL_(1) <<  6)
#define PIN_PA07B_AC_AIN3               _L_(7) /**< \brief AC signal: AIN3 on PA07 mux B */
#define MUX_PA07B_AC_AIN3               _L_(1)
#define PINMUX_PA07B_AC_AIN3       ((PIN_PA07B_AC_AIN3 << 16) | MUX_PA07B_AC_AIN3)
#define PORT_PA07B_AC_AIN3     (_UL_(1) <<  7)
#define PIN_PA18H_AC_CMP0              _L_(18) /**< \brief AC signal: CMP0 on PA18 mux H */
#define MUX_PA18H_AC_CMP0               _L_(7)
#define PINMUX_PA18H_AC_CMP0       ((PIN_PA18H_AC_CMP0 << 16) | MUX_PA18H_AC_CMP0)
#define PORT_PA18H_AC_CMP0     (_UL_(1) << 18)
#define PIN_PA19H_AC_CMP1              _L_(19) /**< \brief AC signal: CMP1 on PA19 mux H */
#define MUX_PA19H_AC_CMP1               _L_(7)
#define PINMUX_PA19H_AC_CMP1       ((PIN_PA19H_AC_CMP1 << 16) | MUX_PA19H_AC_CMP1)
#define PORT_PA19H_AC_CMP1     (_UL_(1) << 19)
/* ========== PORT definition for OPAMP peripheral ========== */
#define PIN_PA02B_OPAMP_OANEG0          _L_(2) /**< \brief OPAMP signal: OANEG0 on PA02 mux B */
#define MUX_PA02B_OPAMP_OANEG0          _L_(1)
#define PINMUX_PA02B_OPAMP_OANEG0  ((PIN_PA02B_OPAMP_OANEG0 << 16) | MUX_PA02B_OPAMP_OANEG0)
#define PORT_PA02B_OPAMP_OANEG0  (_UL_(1) <<  2)
#define PIN_PA07B_OPAMP_OAOUT0          _L_(7) /**< \brief OPAMP signal: OAOUT0 on PA07 mux B */
#define MUX_PA07B_OPAMP_OAOUT0          _L_(1)
#define PINMUX_PA07B_OPAMP_OAOUT0  ((PIN_PA07B_OPAMP_OAOUT0 << 16) | MUX_PA07B_OPAMP_OAOUT0)
#define PORT_PA07B_OPAMP_OAOUT0  (_UL_(1) <<  7)
#define PIN_PA04B_OPAMP_OAOUT2          _L_(4) /**< \brief OPAMP signal: OAOUT2 on PA04 mux B */
#define MUX_PA04B_OPAMP_OAOUT2          _L_(1)
#define PINMUX_PA04B_OPAMP_OAOUT2  ((PIN_PA04B_OPAMP_OAOUT2 << 16) | MUX_PA04B_OPAMP_OAOUT2)
#define PORT_PA04B_OPAMP_OAOUT2  (_UL_(1) <<  4)
#define PIN_PA06B_OPAMP_OAPOS0          _L_(6) /**< \brief OPAMP signal: OAPOS0 on PA06 mux B */
#define MUX_PA06B_OPAMP_OAPOS0          _L_(1)
#define PINMUX_PA06B_OPAMP_OAPOS0  ((PIN_PA06B_OPAMP_OAPOS0 << 16) | MUX_PA06B_OPAMP_OAPOS0)
#define PORT_PA06B_OPAMP_OAPOS0  (_UL_(1) <<  6)
#define PIN_PA05B_OPAMP_OAPOS2          _L_(5) /**< \brief OPAMP signal: OAPOS2 on PA05 mux B */
#define MUX_PA05B_OPAMP_OAPOS2          _L_(1)
#define PINMUX_PA05B_OPAMP_OAPOS2  ((PIN_PA05B_OPAMP_OAPOS2 << 16) | MUX_PA05B_OPAMP_OAPOS2)
#define PORT_PA05B_OPAMP_OAPOS2  (_UL_(1) <<  5)
/* ========== PORT definition for CCL peripheral ========== */
#define PIN_PA04I_CCL_IN0               _L_(4) /**< \brief CCL signal: IN0 on PA04 mux I */
#define MUX_PA04I_CCL_IN0               _L_(8)
#define PINMUX_PA04I_CCL_IN0       ((PIN_PA04I_CCL_IN0 << 16) | MUX_PA04I_CCL_IN0)
#define PORT_PA04I_CCL_IN0     (_UL_(1) <<  4)
#define PIN_PA16I_CCL_IN0              _L_(16) /**< \brief CCL signal: IN0 on PA16 mux I */
#define MUX_PA16I_CCL_IN0               _L_(8)
#define PINMUX_PA16I_CCL_IN0       ((PIN_PA16I_CCL_IN0 << 16) | MUX_PA16I_CCL_IN0)
#define PORT_PA16I_CCL_IN0     (_UL_(1) << 16)
#define PIN_PA05I_CCL_IN1               _L_(5) /**< \brief CCL signal: IN1 on PA05 mux I */
#define MUX_PA05I_CCL_IN1               _L_(8)
#define PINMUX_PA05I_CCL_IN1       ((PIN_PA05I_CCL_IN1 << 16) | MUX_PA05I_CCL_IN1)
#define PORT_PA05I_CCL_IN1     (_UL_(1) <<  5)
#define PIN_PA17I_CCL_IN1              _L_(17) /**< \brief CCL signal: IN1 on PA17 mux I */
#define MUX_PA17I_CCL_IN1               _L_(8)
#define PINMUX_PA17I_CCL_IN1       ((PIN_PA17I_CCL_IN1 << 16) | MUX_PA17I_CCL_IN1)
#define PORT_PA17I_CCL_IN1     (_UL_(1) << 17)
#define PIN_PA06I_CCL_IN2               _L_(6) /**< \brief CCL signal: IN2 on PA06 mux I */
#define MUX_PA06I_CCL_IN2               _L_(8)
#define PINMUX_PA06I_CCL_IN2       ((PIN_PA06I_CCL_IN2 << 16) | MUX_PA06I_CCL_IN2)
#define PORT_PA06I_CCL_IN2     (_UL_(1) <<  6)
#define PIN_PA18I_CCL_IN2              _L_(18) /**< \brief CCL signal: IN2 on PA18 mux I */
#define MUX_PA18I_CCL_IN2               _L_(8)
#define PINMUX_PA18I_CCL_IN2       ((PIN_PA18I_CCL_IN2 << 16) | MUX_PA18I_CCL_IN2)
#define PORT_PA18I_CCL_IN2     (_UL_(1) << 18)
#define PIN_PA08I_CCL_IN3               _L_(8) /**< \brief CCL signal: IN3 on PA08 mux I */
#define MUX_PA08I_CCL_IN3               _L_(8)
#define PINMUX_PA08I_CCL_IN3       ((PIN_PA08I_CCL_IN3 << 16) | MUX_PA08I_CCL_IN3)
#define PORT_PA08I_CCL_IN3     (_UL_(1) <<  8)
#define PIN_PA30I_CCL_IN3              _L_(30) /**< \brief CCL signal: IN3 on PA30 mux I */
#define MUX_PA30I_CCL_IN3               _L_(8)
#define PINMUX_PA30I_CCL_IN3       ((PIN_PA30I_CCL_IN3 << 16) | MUX_PA30I_CCL_IN3)
#define PORT_PA30I_CCL_IN3     (_UL_(1) << 30)
#define PIN_PA09I_CCL_IN4               _L_(9) /**< \brief CCL signal: IN4 on PA09 mux I */
#define MUX_PA09I_CCL_IN4               _L_(8)
#define PINMUX_PA09I_CCL_IN4       ((PIN_PA09I_CCL_IN4 << 16) | MUX_PA09I_CCL_IN4)
#define PORT_PA09I_CCL_IN4     (_UL_(1) <<  9)
#define PIN_PA10I_CCL_IN5              _L_(10) /**< \brief CCL signal: IN5 on PA10 mux I */
#define MUX_PA10I_CCL_IN5               _L_(8)
#define PINMUX_PA10I_CCL_IN5       ((PIN_PA10I_CCL_IN5 << 16) | MUX_PA10I_CCL_IN5)
#define PORT_PA10I_CCL_IN5     (_UL_(1) << 10)
#define PIN_PA22I_CCL_IN6              _L_(22) /**< \brief CCL signal: IN6 on PA22 mux I */
#define MUX_PA22I_CCL_IN6               _L_(8)
#define PINMUX_PA22I_CCL_IN6       ((PIN_PA22I_CCL_IN6 << 16) | MUX_PA22I_CCL_IN6)
#define PORT_PA22I_CCL_IN6     (_UL_(1) << 22)
#define PIN_PA23I_CCL_IN7              _L_(23) /**< \brief CCL signal: IN7 on PA23 mux I */
#define MUX_PA23I_CCL_IN7               _L_(8)
#define PINMUX_PA23I_CCL_IN7       ((PIN_PA23I_CCL_IN7 << 16) | MUX_PA23I_CCL_IN7)
#define PORT_PA23I_CCL_IN7     (_UL_(1) << 23)
#define PIN_PA24I_CCL_IN8              _L_(24) /**< \brief CCL signal: IN8 on PA24 mux I */
#define MUX_PA24I_CCL_IN8               _L_(8)
#define PINMUX_PA24I_CCL_IN8       ((PIN_PA24I_CCL_IN8 << 16) | MUX_PA24I_CCL_IN8)
#define PORT_PA24I_CCL_IN8     (_UL_(1) << 24)
#define PIN_PA07I_CCL_OUT0              _L_(7) /**< \brief CCL signal: OUT0 on PA07 mux I */
#define MUX_PA07I_CCL_OUT0              _L_(8)
#define PINMUX_PA07I_CCL_OUT0      ((PIN_PA07I_CCL_OUT0 << 16) | MUX_PA07I_CCL_OUT0)
#define PORT_PA07I_CCL_OUT0    (_UL_(1) <<  7)
#define PIN_PA19I_CCL_OUT0             _L_(19) /**< \brief CCL signal: OUT0 on PA19 mux I */
#define MUX_PA19I_CCL_OUT0              _L_(8)
#define PINMUX_PA19I_CCL_OUT0      ((PIN_PA19I_CCL_OUT0 << 16) | MUX_PA19I_CCL_OUT0)
#define PORT_PA19I_CCL_OUT0    (_UL_(1) << 19)
#define PIN_PA11I_CCL_OUT1             _L_(11) /**< \brief CCL signal: OUT1 on PA11 mux I */
#define MUX_PA11I_CCL_OUT1              _L_(8)
#define PINMUX_PA11I_CCL_OUT1      ((PIN_PA11I_CCL_OUT1 << 16) | MUX_PA11I_CCL_OUT1)
#define PORT_PA11I_CCL_OUT1    (_UL_(1) << 11)
#define PIN_PA31I_CCL_OUT1             _L_(31) /**< \brief CCL signal: OUT1 on PA31 mux I */
#define MUX_PA31I_CCL_OUT1              _L_(8)
#define PINMUX_PA31I_CCL_OUT1      ((PIN_PA31I_CCL_OUT1 << 16) | MUX_PA31I_CCL_OUT1)
#define PORT_PA31I_CCL_OUT1    (_UL_(1) << 31)
#define PIN_PA25I_CCL_OUT2             _L_(25) /**< \brief CCL signal: OUT2 on PA25 mux I */
#define MUX_PA25I_CCL_OUT2              _L_(8)
#define PINMUX_PA25I_CCL_OUT2      ((PIN_PA25I_CCL_OUT2 << 16) | MUX_PA25I_CCL_OUT2)
#define PORT_PA25I_CCL_OUT2    (_UL_(1) << 25)

#endif /* _SAML21E17B_PIO_ */
