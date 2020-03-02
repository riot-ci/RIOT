/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd5x
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for Microchip SAMD5x/SAME5x MCUs
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

#define KHZ(x)    ((x) * 1000)
#define MHZ(x) (KHZ(x) * 1000)

#if CLOCK_CORECLOCK == 0
#error Please select CLOCK_CORECLOCK
#endif

#if USE_XOSC_ONLY
#define USE_DPLL 0
#define USE_DFLL 0
#define USE_XOSC 1
#else
#define USE_DFLL 1
#define USE_XOSC 0
/* use DFLL for low frequency operation */
#if CLOCK_CORECLOCK > SAM0_DFLL_FREQ_HZ
#define USE_DPLL 1
#else
#define USE_DPLL 0
#if (SAM0_DFLL_FREQ_HZ % CLOCK_CORECLOCK)
#error For frequencies < 48 MHz, CLOCK_CORECLOCK must be a divider of 48 MHz
#endif
#endif
#endif

#ifndef XOSC0_FREQUENCY
#define XOSC0_FREQUENCY MHZ(12)
#endif

/* If the CPU clock is lower than the minimal DPLL Freq
   set fDPLL = 2 * CLOCK_CORECLOCK */
#if USE_DPLL && (CLOCK_CORECLOCK < SAM0_DPLL_FREQ_MIN_HZ)
#define DPLL_DIV 2
#else
#define DPLL_DIV 1
#endif

#define VREG_LDO  (0)
#define VREG_BUCK (1)

static inline void set_voltage_regulator(uint8_t src)
{
    SUPC->VREG.bit.SEL = src;
    while (!SUPC->STATUS.bit.VREGRDY) {}
}

static void xosc32k_init(void)
{
#if EXTERNAL_OSC32_SOURCE
    OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_ENABLE
                            | OSC32KCTRL_XOSC32K_EN1K
                            | OSC32KCTRL_XOSC32K_EN32K
                            | OSC32KCTRL_XOSC32K_RUNSTDBY
                            | OSC32KCTRL_XOSC32K_XTALEN
                            | OSC32KCTRL_XOSC32K_STARTUP(7);

    while (!OSC32KCTRL->STATUS.bit.XOSC32KRDY) {}
#endif
}

static void xosc_init(uint8_t idx, uint32_t freq)
{
#if USE_XOSC
    uint32_t reg = OSCCTRL_XOSCCTRL_XTALEN
                | OSCCTRL_XOSCCTRL_ENALC
                | OSCCTRL_XOSCCTRL_ENABLE;

    /* SAM D5x/E5x Manual 54.12.1 (Crystal oscillator characteristics) &
     * 28.8.6 (External Multipurpose Crystal Oscillator Control)
     */
    if (freq <= MHZ(8)) {
        /* 72200 cycles @ 8MHz = 9025 µs */
        reg |= OSCCTRL_XOSCCTRL_STARTUP(9)
            |  OSCCTRL_XOSCCTRL_IMULT(3)
            |  OSCCTRL_XOSCCTRL_IPTAT(2);
    } else if (freq <= MHZ(16)) {
        /* 62000 cycles @ 16MHz = 3875 µs */
        reg |= OSCCTRL_XOSCCTRL_STARTUP(7)
            |  OSCCTRL_XOSCCTRL_IMULT(4)
            |  OSCCTRL_XOSCCTRL_IPTAT(3);
    } else if (freq <= MHZ(24)) {
        /* 68500 cycles @ 24MHz = 2854 µs */
        reg |= OSCCTRL_XOSCCTRL_STARTUP(7)
            |  OSCCTRL_XOSCCTRL_IMULT(5)
            |  OSCCTRL_XOSCCTRL_IPTAT(3);
    } else {
        /* 38500 cycles @ 48MHz = 802 µs */
        reg |= OSCCTRL_XOSCCTRL_STARTUP(5)
            |  OSCCTRL_XOSCCTRL_IMULT(6)
            |  OSCCTRL_XOSCCTRL_IPTAT(3);
    }

    OSCCTRL->XOSCCTRL[idx].reg = reg;
    while (!(OSCCTRL->STATUS.vec.XOSCRDY & (idx + 1))) {}
#else
    (void) idx;
    (void) freq;
#endif
}

static void dfll_init(void)
{
#if USE_DFLL
    uint32_t reg = OSCCTRL_DFLLCTRLB_QLDIS
#ifdef OSCCTRL_DFLLCTRLB_WAITLOCK
          | OSCCTRL_DFLLCTRLB_WAITLOCK
#endif
    ;

    /* workaround for Errata 2.8.3 DFLLVAL.FINE Value When DFLL48M Re-enabled */
    OSCCTRL->DFLLMUL.reg = 0;   /* Write new DFLLMULL configuration */
    OSCCTRL->DFLLCTRLB.reg = 0; /* Select Open loop configuration */
    OSCCTRL->DFLLCTRLA.bit.ENABLE = 1; /* Enable DFLL */
    OSCCTRL->DFLLVAL.reg = OSCCTRL->DFLLVAL.reg; /* Reload DFLLVAL register */
    OSCCTRL->DFLLCTRLB.reg = reg; /* Write final DFLL configuration */
    OSCCTRL->DFLLCTRLA.reg = OSCCTRL_DFLLCTRLA_ENABLE;

    while (!OSCCTRL->STATUS.bit.DFLLRDY) {}
#endif /* USE_DFLL */
}

static void fdpll0_init(uint32_t f_cpu)
{
#if USE_DPLL
    /* We source the DPLL from 32kHz GCLK1 */
    const uint32_t LDR = ((f_cpu << 5) / 32768);

    /* disable the DPLL before changing the configuration */
    OSCCTRL->Dpll[0].DPLLCTRLA.bit.ENABLE = 0;
    while (OSCCTRL->Dpll[0].DPLLSYNCBUSY.reg) {}

    /* set DPLL clock source */
    GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL0].reg = GCLK_PCHCTRL_GEN(1) | GCLK_PCHCTRL_CHEN;
    while (!(GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL0].reg & GCLK_PCHCTRL_CHEN)) {}

    OSCCTRL->Dpll[0].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDRFRAC(LDR & 0x1F)
                                   | OSCCTRL_DPLLRATIO_LDR((LDR >> 5) - 1);

    /* Without LBYPASS, startup takes very long, see errata section 2.13. */
    OSCCTRL->Dpll[0].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_REFCLK_GCLK
                                   | OSCCTRL_DPLLCTRLB_WUF
                                   | OSCCTRL_DPLLCTRLB_LBYPASS;

    OSCCTRL->Dpll[0].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE;

    while (OSCCTRL->Dpll[0].DPLLSYNCBUSY.reg) {}
    while (!(OSCCTRL->Dpll[0].DPLLSTATUS.bit.CLKRDY &&
             OSCCTRL->Dpll[0].DPLLSTATUS.bit.LOCK)) {}
#else
    (void) f_cpu;
#endif
}

static void gclk_connect(uint8_t id, uint8_t src, uint32_t flags) {
    GCLK->GENCTRL[id].reg = GCLK_GENCTRL_SRC(src) | GCLK_GENCTRL_GENEN | flags | GCLK_GENCTRL_IDC;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(id)) {}
}

void sam0_gclk_enable(uint8_t id)
{
    /* clocks 0 & 1 are always running */

    switch (id) {
    case SAM0_GCLK_8MHZ:
        /* 8 MHz clock used by xtimer */
        if (USE_DPLL) {
            gclk_connect(SAM0_GCLK_8MHZ,
                         GCLK_SOURCE_DPLL0,
                         GCLK_GENCTRL_DIV(DPLL_DIV * CLOCK_CORECLOCK / 8000000));
        } else if (USE_DFLL) {
            gclk_connect(SAM0_GCLK_8MHZ,
                         GCLK_SOURCE_DFLL,
                         GCLK_GENCTRL_DIV(SAM0_DFLL_FREQ_HZ / 8000000));
        } else if (USE_XOSC) {
            gclk_connect(SAM0_GCLK_8MHZ,
                         GCLK_SOURCE_XOSC,
                         GCLK_GENCTRL_DIV(SAM0_DFLL_FREQ_HZ / 8000000));
        }
        break;
    case SAM0_GCLK_48MHZ:
        if (USE_DFLL) {
            gclk_connect(SAM0_GCLK_48MHZ, GCLK_SOURCE_DFLL, 0);
        } else if (USE_XOSC) {
            gclk_connect(SAM0_GCLK_48MHZ, GCLK_SOURCE_XOSC, 0);
        }

        break;
    }
}

uint32_t sam0_gclk_freq(uint8_t id)
{
    switch (id) {
    case SAM0_GCLK_MAIN:
        return CLOCK_CORECLOCK;
    case SAM0_GCLK_32KHZ:
        return 32768;
    case SAM0_GCLK_8MHZ:
        return 8000000;
    case SAM0_GCLK_48MHZ:
        if (USE_DFLL) {
            return SAM0_DFLL_FREQ_HZ;
        } else if (USE_XOSC) {
            return SAM0_DFLL_FREQ_HZ;
        } else {
            assert(0);
            return 0;
        }
    default:
        return 0;
    }
}

void cpu_pm_cb_enter(int deep)
{
    if (deep) {
        /* no fast clocks are running in sleep -> switch to buck regulator */
        set_voltage_regulator(VREG_BUCK);
    }
}

void cpu_pm_cb_leave(int deep)
{
    if (deep) {
        /* Errata 2.19.1 -> we can't use BUCK with DPLL or DFLL */
        if (USE_DPLL || USE_DFLL) {
            set_voltage_regulator(VREG_LDO);
        }

        /* Errata 2.8.3 -> we have to manually re-init DFLL */
        dfll_init();
    }
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

    /* select LDO voltage regulator as we start with DFLL as main clock */
    set_voltage_regulator(VREG_LDO);

    /* turn on only needed APB peripherals */
    MCLK->APBAMASK.reg = MCLK_APBAMASK_MCLK
                       | MCLK_APBAMASK_OSCCTRL
                       | MCLK_APBAMASK_OSC32KCTRL
                       | MCLK_APBAMASK_GCLK
                       | MCLK_APBAMASK_SUPC
                       | MCLK_APBAMASK_PAC
#ifdef MODULE_PERIPH_PM
                       | MCLK_APBAMASK_PM
#endif
#ifdef MODULE_PERIPH_GPIO_IRQ
                       | MCLK_APBAMASK_EIC
#endif
                       ;

    MCLK->APBBMASK.reg = 0
#ifdef MODULE_PERIPH_FLASHPAGE
                       | MCLK_APBBMASK_NVMCTRL
#endif
#ifdef MODULE_PERIPH_GPIO
                       | MCLK_APBBMASK_PORT
#endif
                       ;

    MCLK->APBCMASK.reg = 0;
    MCLK->APBDMASK.reg = 0;

    /* enable the Cortex M Cache Controller */
    CMCC->CTRL.bit.CEN = 1;

    /* Software reset the GCLK module to ensure it is re-initialized correctly */
    GCLK->CTRLA.reg = GCLK_CTRLA_SWRST;
    while (GCLK->CTRLA.reg & GCLK_CTRLA_SWRST) {}
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_SWRST) {}

    xosc_init(0, XOSC0_FREQUENCY);
    xosc32k_init();

    if (EXTERNAL_OSC32_SOURCE) {
        gclk_connect(SAM0_GCLK_32KHZ, GCLK_SOURCE_XOSC32K, 0);
    } else if (ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE) {
        gclk_connect(SAM0_GCLK_32KHZ, GCLK_SOURCE_OSCULP32K, 0);
    }

    dfll_init();
    fdpll0_init(CLOCK_CORECLOCK * DPLL_DIV);

    /* select the source of the main clock */
    if (USE_DPLL) {
        gclk_connect(SAM0_GCLK_MAIN, GCLK_SOURCE_DPLL0,
                     GCLK_GENCTRL_DIV(DPLL_DIV));
    } else if (USE_DFLL) {
        gclk_connect(SAM0_GCLK_MAIN, GCLK_SOURCE_DFLL,
                     GCLK_GENCTRL_DIV(SAM0_DFLL_FREQ_HZ / CLOCK_CORECLOCK));
    } else if (USE_XOSC) {
        gclk_connect(SAM0_GCLK_MAIN, GCLK_SOURCE_XOSC0,
                     GCLK_GENCTRL_DIV(XOSC0_FREQUENCY / CLOCK_CORECLOCK));
    }

    /* Errata 2.19.1: Buck Converter mode is not supported when using PLLs.
                      Use the LDO Regulator mode when using FDPLL and DFLL configurations. */
    if (!USE_DPLL && !USE_DFLL) {

        /* make sure fast clocks are off */
        OSCCTRL->DFLLCTRLA.reg = 0;
        OSCCTRL->Dpll[0].DPLLCTRLA.reg = 0;
        OSCCTRL->Dpll[1].DPLLCTRLA.reg = 0;

        set_voltage_regulator(VREG_BUCK);
    }

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();

    /* set ONDEMAND bit after all clocks have been configured */
    /* This is to avoid setting the source for the main clock to ONDEMAND before using it. */
    OSCCTRL->DFLLCTRLA.reg |= OSCCTRL_DFLLCTRLA_ONDEMAND;
    OSCCTRL->Dpll[0].DPLLCTRLA.reg |= OSCCTRL_DPLLCTRLA_ONDEMAND;
}
