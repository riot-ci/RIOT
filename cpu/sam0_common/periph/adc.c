/*
 * Copyright (C) 2017 Dan Evans <photonthunder@gmail.com>
 * Copyright (C) 2017 Travis Griggs <travisgriggs@gmail.com>
 * Copyright (C) 2017 Dylan Laduranty <dylanladuranty@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @}
 */
#include <stdint.h>
#include "cpu.h"
#include "periph/gpio.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* The SAMD5x/SAME5x family has two ADCs: ADC0 and ADC1.
 * Introducing ADC_DEV as alias for the respective device (ADC/ADC0/ADC1). */
#ifndef ADC_DEV
    #ifdef ADC0
        #define ADC_DEV ADC0
    #else
        #define ADC_DEV ADC
    #endif
#endif

#ifndef ADC_GCLK_SRC
    #define ADC_GCLK_SRC SAM0_GCLK_MAIN
#endif

/* Prototypes */
static bool _adc_syncing(void);
static void _adc_poweroff(void);
static void _adc_calibrate(void);
static void _setup_clock(adc_res_t res);
static int _adc_configure(adc_res_t res);

static mutex_t _lock = MUTEX_INIT;

static inline void _prep(void)
{
    mutex_lock(&_lock);
}

static inline void _done(void)
{
    mutex_unlock(&_lock);
}

static bool _adc_syncing(void)
{
#ifdef CPU_SAMD21
    if (ADC_DEV->STATUS.reg & ADC_STATUS_SYNCBUSY) {
        return true;
    }
#else /* CPU_SAML21 */
    if (ADC_DEV->SYNCBUSY.reg) {
        return true;
    }
#endif
    return false;
}

static void _adc_poweroff(void)
{
    while (_adc_syncing()) {}
    /* Disable */
    ADC_DEV->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
    while (_adc_syncing()) {}
    /* Disable bandgap */
#ifdef CPU_SAMD21
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_INT1V) {
        SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
    }
#else /* CPU_SAML21 */
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_INTREF) {
        SUPC->VREF.reg &= ~SUPC_VREF_VREFOE;
    }
#endif
}

static void _setup_clock(adc_res_t res)
{
#ifdef CPU_SAMD21
 /* GCLK Setup */
    GCLK->CLKCTRL.reg = (uint32_t)(GCLK_CLKCTRL_CLKEN
            | GCLK_CLKCTRL_GEN(ADC_GCLK_SRC) | (GCLK_CLKCTRL_ID(ADC_GCLK_ID)));
    /* Configure CTRLB Register HERE IS THE RESOLUTION SET! */
    ADC_DEV->CTRLB.reg = ADC_PRESCALER | res;
#else
#ifdef ADC0_GCLK_ID
    /* GCLK Setup */
    if (ADC_DEV == ADC0) {
        GCLK->PCHCTRL[ADC0_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                | GCLK_PCHCTRL_GEN(ADC_GCLK_SRC);
    }
    else {
        GCLK->PCHCTRL[ADC1_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                | GCLK_PCHCTRL_GEN(ADC_GCLK_SRC);
    }
    /* Configure CTRLA & CTRLB Register */
    ADC_DEV->CTRLA.reg = ADC_PRESCALER;
    ADC_DEV->CTRLB.reg |= res;
#else
    /* GCLK Setup */
    GCLK->PCHCTRL[ADC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
            | GCLK_PCHCTRL_GEN(ADC_GCLK_SRC);
    /* Configure CTRLB & CTRLC Register */
    ADC_DEV->CTRLB.reg = ADC_PRESCALER;
    ADC_DEV->CTRLC.reg |= res;
#endif
#endif
}

static void _adc_calibrate(void)
{
#ifdef CPU_SAMD21
    /* Load the fixed device calibration constants */
    ADC_DEV->CALIB.reg =
        ADC_CALIB_BIAS_CAL((*(uint32_t*)ADC_FUSES_BIASCAL_ADDR >>
                            ADC_FUSES_BIASCAL_Pos)) |
        ADC_CALIB_LINEARITY_CAL((*(uint64_t*)ADC_FUSES_LINEARITY_0_ADDR >>
                                ADC_FUSES_LINEARITY_0_Pos));
#else
    /* Set default calibration from NVM */
#ifdef ADC0_FUSES_BIASCOMP_ADDR
    if (ADC_DEV == ADC0) {
        ADC_DEV->CALIB.reg =
            ADC0_FUSES_BIASCOMP((*(uint32_t*)ADC0_FUSES_BIASCOMP_ADDR)) >>
            ADC_CALIB_BIASCOMP_Pos |
            ADC0_FUSES_BIASREFBUF((*(uint32_t*)ADC0_FUSES_BIASREFBUF_ADDR) >>
            ADC0_FUSES_BIASREFBUF_Pos);
    }
    else {
        ADC_DEV->CALIB.reg =
            ADC1_FUSES_BIASCOMP((*(uint32_t*)ADC1_FUSES_BIASCOMP_ADDR)) >>
            ADC_CALIB_BIASCOMP_Pos |
            ADC1_FUSES_BIASREFBUF((*(uint32_t*)ADC1_FUSES_BIASREFBUF_ADDR) >>
            ADC1_FUSES_BIASREFBUF_Pos);
    }
#else
    ADC_DEV->CALIB.reg =
            ADC_FUSES_BIASCOMP((*(uint32_t*)ADC_FUSES_BIASCOMP_ADDR)) >>
            ADC_CALIB_BIASCOMP_Pos |
            ADC_FUSES_BIASREFBUF((*(uint32_t*)ADC_FUSES_BIASREFBUF_ADDR) >>
            ADC_FUSES_BIASREFBUF_Pos);
#endif
#endif
}

static int _adc_configure(adc_res_t res)
{
    /* Individual comparison necessary because ADC Resolution Bits are not
     * numerically in order and 16Bit (averaging - not currently supported)
     * falls between 12bit and 10bit.  See datasheet for details */
    if (!((res == ADC_RES_8BIT) || (res == ADC_RES_10BIT) ||
          (res == ADC_RES_12BIT))){
        return -1;
    }
    _adc_poweroff();
    if (ADC_DEV->CTRLA.reg & ADC_CTRLA_SWRST ||
        ADC_DEV->CTRLA.reg & ADC_CTRLA_ENABLE ) {
        DEBUG("adc: not ready\n");
        return -1;
    }
#ifdef CPU_SAMD21
    /* Power On */
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;
    _setup_clock(res);
    _adc_calibrate();
    /* Set Voltage Reference */
    ADC_DEV->REFCTRL.reg = ADC_REF_DEFAULT;
    /* Disable all interrupts */
    ADC_DEV->INTENCLR.reg = (ADC_INTENCLR_SYNCRDY) | (ADC_INTENCLR_WINMON) |
                              (ADC_INTENCLR_OVERRUN) | (ADC_INTENCLR_RESRDY);
    while (_adc_syncing()) {}
    /* Enable bandgap if VREF is internal 1V */
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_INT1V) {
        SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
    }
#else /* CPU_SAML21 */
    /* Power on */
#ifdef CPU_SAML1X
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_ADC;
#endif
#ifdef MCLK_APBDMASK_ADC0
    if (ADC_DEV == ADC0) {
        MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC0;
    } else {
        MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC1;
    }
#else
    MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC;
#endif

    _setup_clock(res);
    _adc_calibrate();
    /* Set Voltage Reference */
    ADC_DEV->REFCTRL.reg = ADC_REF_DEFAULT;
    /* Disable all interrupts */
    ADC_DEV->INTENCLR.reg = ADC_INTENCLR_WINMON | ADC_INTENCLR_OVERRUN |
                              ADC_INTENCLR_RESRDY;
    while (_adc_syncing()) {}
    /* Enable bandgap if necessary */
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_INTREF) {
        SUPC->VREF.reg |= SUPC_VREF_VREFOE;
    }
#endif
    /*  Enable ADC Module */
    ADC_DEV->CTRLA.reg |= ADC_CTRLA_ENABLE;
    while (_adc_syncing()) {}
    return 0;
}

int adc_init(adc_t line)
{
    if (line >= ADC_NUMOF) {
        DEBUG("adc: line arg not applicable\n");
        return -1;
    }
    _prep();
    gpio_init(adc_channels[line].pin, GPIO_IN);
    gpio_init_mux(adc_channels[line].pin, GPIO_MUX_B);
    _done();
    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    if (line >= ADC_NUMOF) {
        DEBUG("adc: line arg not applicable\n");
        return -1;
    }
    _prep();
    if (_adc_configure(res) != 0) {
        _done();
        DEBUG("adc: configuration failed\n");
        return -1;
    }
#ifdef CPU_SAMD21
    ADC_DEV->INPUTCTRL.reg = ADC_GAIN_FACTOR_DEFAULT |
                         adc_channels[line].muxpos | ADC_NEG_INPUT;
#else /* CPU_SAML21 */
    ADC_DEV->INPUTCTRL.reg = adc_channels[line].muxpos | ADC_NEG_INPUT;
#endif
    while (_adc_syncing()) {}
    /* Start the conversion */
    ADC_DEV->SWTRIG.reg = ADC_SWTRIG_START;
    /* Wait for the result */
    while (!(ADC_DEV->INTFLAG.reg & ADC_INTFLAG_RESRDY)) {}
    int result = ADC_DEV->RESULT.reg;
    _adc_poweroff();
    _done();
    return result;
}
