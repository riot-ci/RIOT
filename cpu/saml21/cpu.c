/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_saml21
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for Atmel SAML21 MCUs
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include "cpu.h"
#include "periph/init.h"
#include "periph_conf.h"
#include "stdio_base.h"

static void _gclk_setup(int gclk, uint32_t reg)
{
    GCLK->GENCTRL[gclk].reg = reg;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(gclk)) {}
}

static void _osc32k_setup(void)
{
#if INTERNAL_OSC32_SOURCE
    uint32_t * pCalibrationArea;
    uint32_t osc32kcal;

    /* Read OSC32KCAL, calibration data for OSC32 !!! */
    pCalibrationArea = (uint32_t*) NVMCTRL_OTP5;
    osc32kcal = ( (*pCalibrationArea) & 0x1FC0 ) >> 6;

    /* RTC use Low Power Internal Oscillator at 32kHz */
    OSC32KCTRL->OSC32K.reg = OSC32KCTRL_OSC32K_RUNSTDBY
                           | OSC32KCTRL_OSC32K_EN32K
                           | OSC32KCTRL_OSC32K_CALIB(osc32kcal)
                           | OSC32KCTRL_OSC32K_ENABLE;

    /* Wait OSC32K Ready */
    while (!OSC32KCTRL->STATUS.bit.OSC32KRDY) {}
#endif /* INTERNAL_OSC32_SOURCE */
}

static void _xosc32k_setup(void)
{
#if EXTERNAL_OSC32_SOURCE
    /* RTC uses External 32,768KHz Oscillator */
    OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_XTALEN
                            | OSC32KCTRL_XOSC32K_RUNSTDBY
                            | OSC32KCTRL_XOSC32K_EN32K
                            | OSC32KCTRL_XOSC32K_ENABLE;

    /* Wait XOSC32K Ready */
    while (!OSC32KCTRL->STATUS.bit.XOSC32KRDY) {}
#endif
}

static void _dfll_setup(void)
{
#if (CLOCK_CORECLOCK == 48000000U) || defined (MODULE_PERIPH_USBDEV)
    _gclk_setup(3, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K);
    /* Write Generic Clock Generator 3 configuration */
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(3)) {}

    GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].reg = GCLK_PCHCTRL_CHEN |
                                                GCLK_PCHCTRL_GEN_GCLK3;

    /* wait for sync */
    while (!(GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].reg & GCLK_PCHCTRL_CHEN)) {}

    OSCCTRL->DFLLCTRL.reg = OSCCTRL_DFLLCTRL_ENABLE;
    /* Wait for write synchronization */
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY)) {}
    OSCCTRL->DFLLVAL.reg = OSCCTRL_DFLLVAL_COARSE((*(uint32_t*)NVMCTRL_OTP5)
                           >> 26) |  OSCCTRL_DFLLVAL_FINE(512);

    /* Wait for write synchronization */
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY)) {}
    /* Generate a 48 Mhz clock from the 32KHz */
    OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_CSTEP(0x08) |
                           OSCCTRL_DFLLMUL_FSTEP(0x08) |
                           OSCCTRL_DFLLMUL_MUL((48000000U/32768));

    /* Disable DFLL before setting its configuration */
    OSCCTRL->DFLLCTRL.reg = 0;
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY)) {}
    /* Write full configuration to DFLL control register */
    OSCCTRL->DFLLCTRL.reg =  OSCCTRL_DFLLCTRL_WAITLOCK |
                             OSCCTRL_DFLLCTRL_MODE |
                             OSCCTRL_DFLLCTRL_CCDIS |
                             OSCCTRL_DFLLCTRL_BPLCKC |
                             OSCCTRL_DFLLCTRL_ENABLE;

    /* Ensure COARSE and FINE are locked */
    while((!(OSCCTRL->STATUS.bit.DFLLLCKC)) && (!(OSCCTRL->STATUS.bit.DFLLLCKF))) {}
    while (!(OSCCTRL->STATUS.bit.DFLLRDY)) {}
    /* Write Generic Clock Generator 0 configuration */

    /* Enable NVMCTRL */
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_NVMCTRL;
    /* Set Wait State to meet requirements */
    NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_RWS(2);
#endif
}
/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    /* disable the watchdog timer */
    WDT->CTRLA.bit.ENABLE = 0;

    /* initialize the Cortex-M core */
    cortexm_init();

    /* turn on only needed APB peripherals */
    MCLK->APBAMASK.reg =
        MCLK_APBAMASK_PM
        |MCLK_APBAMASK_MCLK
        |MCLK_APBAMASK_RSTC
        |MCLK_APBAMASK_OSCCTRL
        |MCLK_APBAMASK_OSC32KCTRL
        |MCLK_APBAMASK_SUPC
        |MCLK_APBAMASK_GCLK
        |MCLK_APBAMASK_WDT
        |MCLK_APBAMASK_RTC
        |MCLK_APBAMASK_EIC
        |MCLK_APBAMASK_PORT
        //|MCLK_APBAMASK_TAL
        ;

    /* Software reset the GCLK module to ensure it is re-initialized correctly */
    GCLK->CTRLA.reg = GCLK_CTRLA_SWRST;
    while (GCLK->CTRLA.reg & GCLK_CTRLA_SWRST) {}
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_SWRST) {}

    PM->PLCFG.reg = PM_PLCFG_PLSEL_PL2;
    while (!PM->INTFLAG.bit.PLRDY) {}

    /* set OSC16M to 16MHz */
    OSCCTRL->OSC16MCTRL.bit.FSEL = 3;
    OSCCTRL->OSC16MCTRL.bit.ONDEMAND = 0;
    OSCCTRL->OSC16MCTRL.bit.RUNSTDBY = 0;

    _osc32k_setup();
    _xosc32k_setup();
    _dfll_setup();

    /* Setup GCLK generators */
#if (CLOCK_CORECLOCK == 16000000U)
    _gclk_setup(0, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC16M);
#elif (CLOCK_CORECLOCK == 48000000U)
    _gclk_setup(0, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M);
#else
#error "Please select a valid CPU frequency"
#endif
    /* clock used by timers */
    _gclk_setup(5, GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC16M);
#ifdef MODULE_PERIPH_PM
    PM->CTRLA.reg = PM_CTRLA_MASK & (~PM_CTRLA_IORET);

    /* disable brownout detection
     * (Caused unexplicable reboots from sleep on saml21. /KS)
     */
    SUPC->BOD33.bit.ENABLE=0;
#endif

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();
}
