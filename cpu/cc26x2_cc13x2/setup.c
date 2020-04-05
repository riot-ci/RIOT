/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief       CC26x2/CC13x2 Device setup functions
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 *
 * @}
 */

#include "cpu.h"

/**
 * @brief   FMC pump wakeup time
 *
 * This value is provided by TI. It's the optimal wait time when the flash
 * wakes up from sleep.
 */
#define FMC_FLASH_WAKEUP_TIME (0x139)

static void _trim_device(uint32_t rev);

static void _set_boot_det(void) {
    const uint32_t clr_m = AON_PMCTL_RESETCTL_BOOT_DET_1_CLR_m |
                           AON_PMCTL_RESETCTL_BOOT_DET_0_CLR_m |
                           AON_PMCTL_RESETCTL_BOOT_DET_1_SET_m |
                           AON_PMCTL_RESETCTL_BOOT_DET_0_SET_m |
                           AON_PMCTL_RESETCTL_MCU_WARM_RESET_m;
    const uint32_t boot_det_m = AON_PMCTL_RESETCTL_BOOT_DET_1_m |
                                AON_PMCTL_RESETCTL_BOOT_DET_0_m;
    const uint32_t boot_det_s = AON_PMCTL_RESETCTL_BOOT_DET_0_s;
    reg32_t reg = AON_PMCTL->RESETCTL;

    /**
     * Set BOOT_DET bits to 3 if already found to be 1
     * Note: The BOOT_DET_x_CLR/SET bits must be manually cleared
     */
    if (((reg & boot_det_m) >> boot_det_s) == 1) {
        reg &= ~clr_m;
        AON_PMCTL->RESETCTL = reg | AON_PMCTL_RESETCTL_BOOT_DET_1_SET_m;
        AON_PMCTL->RESETCTL = reg;
    }
}

void setup_trim_device(void)
{
    /* Get layout revision of the factory configuration area. Handle undefined
     * revision as revision = 0.
     */
    uint32_t rev = FCFG->FCFG1_REVISION;
    if (rev == 0xFFFFFFFF) {
        rev = 0;
    }

    /* Enable standby in flash bank */
    FLASH->CFG &= ~FLASH_CFG_DIS_STANDBY;

    /* Select correct CACHE mode and set correct CACHE configuration */
    rom_setup_set_cache_mode_according_to_ccfg_setting();

    _trim_device(rev);

    /* Set VIMS power domain control.
     * PDCTL1VIMS = 0 ==> VIMS power domain is only powered when CPU power
     * domain is powered
     */
    PRCM->PDCTL1VIMS = 0;

    /* Configure optimal wait time for flash FSM in cases where flash pump
     * wakes up from sleep */
    FLASH->FPAC1 &= ~FLASH_FPAC1_PSLEEPTDIS_m;
    FLASH->FPAC1 |= (FMC_FLASH_WAKEUP_TIME << FLASH_FPAC1_PSLEEPTDIS_s);

    /* Finally at the end of the flash boot process:
     * Set BOOT_DET bits */
    _set_boot_det();

    /* Make sure there are no ongoing VIMS mode change when leaving (There
     * should typically be no wait time here, but need to be sure)
     */
    while (VIMS->STAT & VIMS_STAT_MODE_CHANGING) {}
}

static void _set_lpm_bias(void)
{
    /* Propagate the LPM_BIAS trim */
    uint32_t trim_reg = FCFG->DAC_BIAS_CNF;
    uint32_t trim_value = ((trim_reg & FCFG1_DAC_BIAS_CNF_LPM_TRIM_IOUT_m) >>
                          FCFG1_DAC_BIAS_CNF_LPM_TRIM_IOUT_s);
    adi_write(ADI_4_AUX_BASE, ADI_4_AUX_LPMBIAS,
              (trim_value << ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_s) &
              ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_m);

    /* Set LPM_BIAS_BACKUP_EN according to FCFG configuration */
    if (trim_reg & FCFG1_DAC_BIAS_CNF_LPM_BIAS_BACKUP_EN) {
        adi_set(ADI3_BASE, ADI_3_REFSYS_AUX_DEBUG,
                ADI_3_REFSYS_AUX_DEBUG_LPM_BIAS_BACKUP_EN);
    }
    else {
        adi_clr(ADI3_BASE, ADI_3_REFSYS_AUX_DEBUG,
                ADI_3_REFSYS_AUX_DEBUG_LPM_BIAS_BACKUP_EN);
    }

    /* Set LPM_BIAS_WIDTH_TRIM according to FCFG1 configuration */
    uint32_t width_trim =
        ((trim_reg & FCFG1_DAC_BIAS_CNF_LPM_BIAS_WIDTH_TRIM_m) >>
        FCFG1_DAC_BIAS_CNF_LPM_BIAS_WIDTH_TRIM_s);
    width_trim = width_trim << ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_s;
    adi_write_8bits(ADI_4_AUX_BASE, ADI_4_AUX_COMP,
                    ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_m, width_trim);
}

static void _set_dcdc_settings(void)
{
    /* Check in CCFG for alternative DCDC setting */
    if ((CCFG->SIZE_AND_DIS_FLAGS &
        CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING) == 0) {
        /**
         * ADI_3_REFSYS:DCDCCTL5[3] = CCFG:MODE_CONF_1[19] (=ALT_DCDC_DITHER_EN)
         * ADI_3_REFSYS:DCDCCTL5[2:0] = CCFG:MODE_CONF_1[18:16](=ALT_DCDC_IPEAK)
         *
         * Using a single 4-bit masked write since layout is equal for both
         * source and destination
         */
        uint32_t dcdc_setting = CCFG->MODE_CONF_1 >>
                                CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_s;
        adi_write_4bits(ADI3_BASE, ADI_3_REFSYS_DCDCCTL5, dcdc_setting, 0xF0);
    }

    /* Force DCDC to use RCOSC before starting up XOSC.
     * Clock loss detector does not use XOSC until SCLK_HF actually switches
     * and thus DCDC is not protected from clock loss on XOSC in that time
     * frame. The force must be released when the switch to XOSC has happened.
     * This is done in OSCHfSourceSwitch().
     */
    reg32_t *addr = (reg32_t *)(AUX_DDI0_OSC_BASE + (DDI_0_OSC_CTL0 << 1) +
                                DDI_MASK16B + 4);
    *addr = DDI_0_OSC_CTL0_CLK_DCDC_SRC_SEL_m |
            (DDI_0_OSC_CTL0_CLK_DCDC_SRC_SEL_m >> 16);

    /* Dummy read to ensure that the write has propagated */
    DDI_0_OSC->CTL0;
}

/* Trims to be applied when coming from SHUTDOWN (also called when
 * coming from PIN_RESET).
 */
static void _trim_device(uint32_t rev)
{
    _set_dcdc_settings();

    /* Read the MODE_CONF register in CCFG */
    uint32_t reg = CCFG->MODE_CONF;

    /* First part of trim done after cold reset and wakeup from shutdown:
     *
     * - Adjust the VDDR_TRIM_SLEEP value.
     * - Configure DCDC.
     */
    rom_setup_after_cold_reset_wakeup_from_shutdown_cfg1(reg);

    /* Addition to the CC1352 boost mode for HWREV >= 2.0
     * The combination VDDR_EXT_LOAD=0 and VDDS_BOD_LEVEL=1 is defined to select
     * boost mode
     */
    if (((reg & CCFG_MODE_CONF_VDDR_EXT_LOAD) == 0) &&
        ((reg & CCFG_MODE_CONF_VDDS_BOD_LEVEL) != 0)) {
        adi_write(ADI3_BASE, ADI_3_REFSYS_DCDCCTL3,
                  ADI_3_REFSYS_DCDCCTL3_VDDR_BOOST_COMP_BOOST);
    }

    /* Second part of trim done after cold reset and wakeup from shutdown:
     *
     * - Configure XOSC.
     */
    rom_setup_after_cold_reset_wakeup_from_shutdown_cfg2(rev, reg);

    /* Propagate the LPM_BIAS, LPM_BIAS_BACKUP_EN and LPM_BIAS_WIDTH_TRIMS
     * trims
     */
    _set_lpm_bias();

    /* Third part of trim done after cold reset and wakeup from shutdown:
     *
     * - Configure HPOSC.
     * - Setup the LF clock.
     */
    rom_setup_after_cold_reset_wakeup_from_shutdown_cfg3(reg);

    /* Set AUX into power down active mode */
    aux_sysif_opmode_change(AUX_SYSIF_OPMODEREQ_REQ_PDA);

    /* Disable EFUSE clock */
    FLASH->CFG |= FLASH_CFG_DIS_EFUSECLK;
}
