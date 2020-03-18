/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2_definitions
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 PRCM register definitions
 */

#ifndef CC26X2_CC13X2_PRCM_H
#define CC26X2_CC13X2_PRCM_H

#include <stdbool.h>
#include <cc26xx_cc13xx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DDI_0_OSC registers
 */
typedef struct {
    reg32_t CTL0; /**< control 0 */
    reg32_t CTL1; /**< control 1 */
    reg32_t RADCEXTCFG; /**< RADC external config */
    reg32_t AMPCOMPCTL; /**< amplitude compensation control */
    reg32_t AMPCOMPTH1; /**< amplitude compensation threshold 1 */
    reg32_t AMPCOMPTH2; /**< amplitude compensation threshold 2 */
    reg32_t ANABYPASSVAL1; /**< analog bypass values 1 */
    reg32_t ANABYPASSVAL2; /**< analog bypass values 2 */
    reg32_t ATESTCTL; /**< analog test control */
    reg32_t ADCDOUBLERNANOAMPCTL; /**< ADC doubler nanoamp control */
    reg32_t XOSCHFCTL; /**< XOSCHF control */
    reg32_t LFOSCCTL; /**< low frequency oscillator control */
    reg32_t RCOSCHFCTL; /**< RCOSCHF control */
    reg32_t RCOSCHMCTL; /**< RCOSCHM control */
    reg32_t STAT0; /**< status 0 */
    reg32_t STAT1; /**< status 1 */
    reg32_t STAT2; /**< status 2 */
} ddi0_osc_regs_t;

/**
 * @brief   DDI_0_OSC register values
 * @{
 */
#define DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL              0x00000001
#define DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL              0x0000000C
#define DDI_0_OSC_CTL0_ACLK_TDC_SRC_SEL             0x00000180
#define DDI_0_OSC_CTL0_CLK_LOSS_EN                  0x00000200 /* enable clock loss detection */
#define DDI_0_OSC_CTL0_XOSC_LF_DIG_BYPASS           0x00000400 /* bypass XOSC_LF and use digital input clock from AON foor xosx_lf (precuations in datasheet) */
#define DDI_0_OSC_CTL0_XOSC_HF_POWER_MODE           0x00000800
#define DDI_0_OSC_CTL0_RCOSC_LF_TRIMMED             0x00001000
#define DDI_0_OSC_CTL0_HPOSC_MODE_EN                0x00004000
#define DDI_0_OSC_CTL0_DCDC_SRC_SEL                 0x01000000
#define DDI_0_OSC_CTL0_DOUBLER_RESET_DURATION       0x02000000
#define DDI_0_OSC_CTL0_DOUBLER_START_DURATION       0x0C000000
#define DDI_0_OSC_CTL0_BYPASS_RCOSC_LF_CLK_QUAL     0x10000000
#define DDI_0_OSC_CTL0_BYPASS_XOSC_LF_CLK_QUAL      0x20000000
#define DDI_0_OSC_CTL0_XTAL_IS_24M                  0x80000000
/** @} */

/** @ingroup cpu_cc26x2_cc13x2_peripheral_memory_map
  * @{
  */
#define DDI0_OSC_BASE       0x400CA000 /**< DDI0_OSC base address */
/*@}*/

#define DDI_0_OSC ((ddi0_osc_regs_t *) (DDI0_OSC_BASE)) /**< DDI_0_OSC register bank */

/**
 * @brief   OSC API values
 * @{
 */
#define OSC_SRC_CLK_HF          0x00000001
#define OSC_SRC_CLK_LF          0x00000004

#define OSC_RCOSC_HF            0x00000000
#define OSC_XOSC_HF             0x00000001
#define OSC_RCOSC_LF            0x00000002
#define OSC_XOSC_LF             0x00000003
/** @} */

/**
 * @brief   Get the source clock settings.
 *
 * @param[in] src_clk Is the source clock to check.
 *
 * @return Returns the type of oscillator that drives the clock source.
 */
uint32_t osc_clock_source_get(uint32_t src_clk);

/**
 * @brief   Configure the oscillator input to a source clock.
 *
 * Use this function to set the oscillator source for one or more of the system
 * source clocks.
 *
 * When selecting the high frequency clock source (@ref OSC_SRC_CLK_HF), this
 * function will not do the actual switch. Enabling the high frequency XTAL can
 * take several hundred micro seconds, so the actual switch is done in a
 * separate function, @ref osc_hf_source_switch(), leaving System CPU free to
 * perform other tasks as the XTAL starts up.
 *
 * @note The High Frequency (@ref OSC_SRC_CLK_HF) can only be derived from the
 * high frequency oscillator. The Low Frequency source clock
 * (@ref OSC_SRC_CLK_LF) can be derived from all 4 oscillators.
 *
 * @note If enabling @ref OSC_XOSC_LF it is not safe to go to
 * powerdown/shutdown until the LF clock is running which can be checked using
 * @ref OSCClockSourceGet().
 *
 * @note Clock loss reset generation must be disabled before SCLK_LF
 * (@ref OSC_SRC_CLK_LF) clock source is changed and remain disabled until
 * the change is confirmed.
 *
 * @param[in] src_clk is the source clocks to configure.
 *  - @ref OSC_SRC_CLK_HF
 *  - @ref OSC_SRC_CLK_LF
 * @param[in] osc Is the oscillator that drives the source clock.
 *  - @ref OSC_RCOSC_HF
 *  - @ref OSC_XOSC_HF
 *  - @ref OSC_RCOSC_LF (only when src_clk is @ref OSC_SRC_CLK_LF)
 *  - @ref OSC_XOSC_LF (only when src_clk is @ref OSC_SRC_CLK_LF)
 */
void osc_clock_source_set(uint32_t src_clk, uint32_t osc);

/**
 * @brief   Check if the HF clock source is ready to be switched.
 *
 * If a request to switch the HF clock source has been made, this function can
 * be used to check if the clock source is ready to be switched.
 *
 * Once the HF clock source is ready the switch can be performed by calling
 * the @ref osc_hf_source_switch()
 *
 * @return true HF clock source is ready.
 * @return false HF clock source is not ready.
 */
bool osc_hf_source_ready(void);

/**
 * @brief   Switch the high frequency clock.
 *
 * When switching the HF clock source the clock period might be prolonged
 * leaving the clock 'stuck-at' high or low for a few cycles. To ensure that
 * this does not coincide with a read access to the Flash, potentially freezing
 * the device, the HF clock source switch must be executed from ROM.
 *
 * @note This function will not return until the clock source has been switched.
 * It is left to the programmer to ensure, that there is a pending request for a
 * HF clock source switch before this function is called.
 */
void osc_hf_source_switch(void);

/**
* AON_PMCTL registers
*/
typedef struct {
   reg32_t __reserved1; /**< meh */
   reg32_t AUXSCECLK; /**< AUX SCE management */
   reg32_t RAMCFG; /**< RAM configuration */
   reg32_t __reserved2; /**< meh */
   reg32_t PWRCTL; /**< Power management control */
   reg32_t PWRSTAT; /**< Power status */
   reg32_t SHUTDOWN; /**< Shutdown control */
   reg32_t RECHARGECFG; /**< Recharge controller configuration */
   reg32_t RECHARGESTAT; /**< Recharge controller status */
   reg32_t OSCCFG; /**< Oscillator configuration */
   reg32_t RESETCTL; /**< Reset control */
   reg32_t SLEEPCTL; /**< Reset control */
   reg32_t __reserved3; /**< meh */
   reg32_t JTAGCFG; /**< JTAG configuration */
   reg32_t JTAGUSERCODE; /**< JTAG USERCODE */
} aon_pmctl_regs_t;

/** @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AON_PMCTL_BASE       0x40090000 /**< AON_PMCTL base address */
/*@}*/

#define AON_PMCTL ((aon_pmctl_regs_t *) (AON_PMCTL_BASE)) /**< AON_PMCTL register bank */

/**
 * PRCM registers
 */
typedef struct {
    reg32_t INFRCLKDIVR; /**< infrastructure clock division factor for run mode */
    reg32_t INFRCLKDIVS; /**< infrastructure clock division factor for sleep mode */
    reg32_t INFRCLKDIVDS; /**< infrastructure clock division factor for deep sleep mode */
    reg32_t VDCTL; /**< MCU voltage domain control */
    reg32_t __reserved1[6]; /**< meh */
    reg32_t CLKLOADCTL; /**< clock load control */
    reg32_t RFCCLKG; /**< RFC clock gate */
    reg32_t VIMSCLKG; /**< VIMS clock gate */
    reg32_t __reserved2[2]; /**< meh */
    reg32_t SECDMACLKGR; /**< TRNG, CRYPTO, and UDMA clock gate for run mode */
    reg32_t SECDMACLKGS; /**< TRNG, CRYPTO, and UDMA clock gate for sleep mode */
    reg32_t SECDMACLKGDS; /**< TRNG, CRYPTO, and UDMA clock gate for deep sleep mode */
    reg32_t GPIOCLKGR; /**< GPIO clock gate for run mode */
    reg32_t GPIOCLKGS; /**< GPIO clock gate for sleep mode */
    reg32_t GPIOCLKGDS; /**< GPIO clock gate for deep sleep mode */
    reg32_t GPTCLKGR; /**< GPT clock gate for run mode */
    reg32_t GPTCLKGS; /**< GPT clock gate for sleep mode */
    reg32_t GPTCLKGDS; /**< GPT clock gate for deep sleep mode */
    reg32_t I2CCLKGR; /**< I2C clock gate for run mode */
    reg32_t I2CCLKGS; /**< I2C clock gate for sleep mode */
    reg32_t I2CCLKGDS; /**< I2C clock gate for deep sleep mode */
    reg32_t UARTCLKGR; /**< UART clock gate for run mode */
    reg32_t UARTCLKGS; /**< UART clock gate for sleep mode */
    reg32_t UARTCLKGDS; /**< UART clock gate for deep sleep mode */
    reg32_t SSICLKGR; /**< SSI clock gate for run mode */
    reg32_t SSICLKGS; /**< SSI clock gate for sleep mode */
    reg32_t SSICLKGDS; /**< SSI clock gate for deep sleep mode */
    reg32_t I2SCLKGR; /**< I2S clock gate for run mode */
    reg32_t I2SCLKGS; /**< I2S clock gate for sleep mode */
    reg32_t I2SCLKGDS; /**< I2S clock gate for deep sleep mode */
    reg32_t __reserved3[9]; /**< meh */
    reg32_t SYSBUSCLKDIV; /**< System bus clock division factor */
    reg32_t CPUCLKDIV; /**< CPU clock division factor */
    reg32_t PERBUSCPUCLKDIV; /**< Peripheral bus division factor */
    reg32_t __reserved4; /**< meh */
    reg32_t PERDMACLKDIV; /**< DMA clock division factor */
    reg32_t I2SBCLKSEL; /**< I2S clock select */
    reg32_t GPTCLKDIV; /**< GPT scalar */
    reg32_t I2SCLKCTL; /**< I2S clock control */
    reg32_t I2SMCLKDIV; /**< MCLK division ratio */
    reg32_t I2SBCLKDIV; /**< BCLK division ratio */
    reg32_t I2SWCLKDIV; /**< WCLK division ratio */
    reg32_t __reserved5[4]; /**< meh */
    reg32_t RESETSECDMA; /**< Reset SEC and UDMA */
    reg32_t RESETGPIO; /**< Reset GPIO */
    reg32_t RESETGPT; /**< Reset GPTs */
    reg32_t RESETI2C; /**< Reset I2C */
    reg32_t RESETUART; /**< Reset UART */
    reg32_t RESETSSI; /**< Reset SSI */
    reg32_t RESETI2S; /**< Reset I2S */
    reg32_t __reserved6[8]; /**< meh */
    reg32_t PDCTL0; /**< power domain control */
    reg32_t PDCTL0RFC; /**< RFC power domain control */
    reg32_t PDCTL0SERIAL; /**< SERIAL power domain control */
    reg32_t PDCTL0PERIPH; /**< PERIPH power domain control */
    reg32_t __reserved7; /**< meh */
    reg32_t PDSTAT0; /**< power domain status */
    reg32_t PDSTAT0RFC; /**< RFC power domain status */
    reg32_t PDSTAT0SERIAL; /**< SERIAL power domain status */
    reg32_t PDSTAT0PERIPH; /**< PERIPH power domain status */
    reg32_t __reserved8[11]; /**< meh */
    reg32_t PDCTL1; /**< power domain control */
    reg32_t __reserved9; /**< power domain control */
    reg32_t PDCTL1CPU; /**< CPU power domain control */
    reg32_t PDCTL1RFC; /**< RFC power domain control */
    reg32_t PDCTL1VIMS; /**< VIMS power domain control */
    reg32_t __reserved10; /**< meh */
    reg32_t PDSTAT1; /**< power domain status */
    reg32_t PDSTAT1BUS; /**< BUS power domain status */
    reg32_t PDSTAT1RFC; /**< RFC power domain status */
    reg32_t PDSTAT1CPU; /**< CPU power domain status */
    reg32_t PDSTAT1VIMS; /**< VIMS power domain status */
    reg32_t __reserved11[9]; /**< meh */
    reg32_t RFCBITS; /**< Control to RFC */
    reg32_t RFCMODESEL; /**< selected RFC mode */
    reg32_t RFCMODEHWOPT; /**< allowed RFC modes */
    reg32_t __reserved12[2]; /**< meh */
    reg32_t PWRPROFSTAT; /**< power profiler register */
    reg32_t __reserved13[14]; /**< meh */
    reg32_t MCUSRAMCFG; /**< MCU SRAM configuration */
    reg32_t __reserved14; /**< meh */
    reg32_t RAMRETEN; /**< memory retention control */
    reg32_t __reserved15[27]; /**< meh */
    reg32_t OSCIMSC; /**< oscillator interrupt mask */
    reg32_t OSCRIS; /**< oscillator raw interrupt status */
    reg32_t OSCICR; /**< oscillator raw interrupt clear */
} prcm_regs_t;

/**
 * @brief   PRCM register values
 * @{
 */
#define CLKLOADCTL_LOAD        0x1
#define CLKLOADCTL_LOADDONE    0x2

#define PDCTL0_RFC_ON       0x1
#define PDCTL0_SERIAL_ON    0x2
#define PDCTL0_PERIPH_ON    0x4

#define PDSTAT0_RFC_ON       0x1
#define PDSTAT0_SERIAL_ON    0x2
#define PDSTAT0_PERIPH_ON    0x4

#define PDCTL1_CPU_ON       0x2
#define PDCTL1_RFC_ON       0x4
#define PDCTL1_VIMS_ON      0x8

#define PDSTAT1_CPU_ON      0x2
#define PDSTAT1_RFC_ON      0x4
#define PDSTAT1_VIMS_ON     0x8
/** @} */

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define PRCM_BASE       0x40082000 /**< PRCM base address */
/*@}*/

#define PRCM ((prcm_regs_t *) (PRCM_BASE)) /**< PRCM register bank */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_PRCM_H */

/*@}*/
