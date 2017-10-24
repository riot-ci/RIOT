/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32l0
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Aurélien Fillau <aurelien.fillau@we-sens.com>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"

#ifdef ADC_CONFIG

/**
 * @brief   Maximum allowed ADC clock speed
 */
#define MAX_ADC_SPEED           (12000000U)

/**
 * @brief   Load the ADC configuration
 */
static const adc_conf_t adc_config[] = ADC_CONFIG;

/**
 * @brief   Allocate locks for all three available ADC device
 *
 * All STM32L0 CPUs we support so far only come with a single ADC device.
 */
static mutex_t lock = MUTEX_INIT;

static inline void prep(void)
{
    mutex_lock(&lock);
    periph_clk_en(APB2, RCC_APB2ENR_ADCEN);
}

static inline void done(void)
{
    periph_clk_dis(APB2, RCC_APB2ENR_ADCEN);
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    /* make sure the given line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock and power on the device */
    prep();

    if ((adc_config[line].chan != 17) && (adc_config[line].chan != 18)) {
        /*configure the pin */
        gpio_init_analog(adc_config[line].pin);
    }

    if ((ADC1->CR & ADC_CR_ADEN) != 0) /* (1) */
    {
        ADC1->CR |= ADC_CR_ADDIS;
        while(ADC1->CR & ADC_CR_ADEN) {} /* Wait for ADC disabled */
    }

    if ((ADC1->CR & ADC_CR_ADEN) == 0)
    {
        /* Then, start a calibration */
        ADC1->CR |= ADC_CR_ADCAL;
        while(ADC1->CR & ADC_CR_ADCAL) {} /* Wait for the end of calibration */
    }

    /* Clear flag */
    ADC1->ISR |= ADC_ISR_ADRDY;

    /* enable device */
    ADC1->CR = ADC_CR_ADVREGEN | ADC_CR_ADEN;

    /* Wait for ADC to be ready */
    while (!(ADC1->ISR & ADC_ISR_ADRDY)) {}

    ADC1->CFGR1 = 0; //no watchdog, no discontinuous mode, no auto off, single conv, no trigger, right align, 12bits, no dma, no wait
    ADC1->CFGR2 = 0; //no oversampling: Watch out, MSB (CKMODE) MUST not be changed while on (it is zero by default)
    //activate VREF, and set prescaler to 4 (4Mhz clock)
    //activate also temp sensor, so that it will be ready for temp measure
    ADC->CCR = ADC_CCR_VREFEN | ADC_CCR_TSEN | ADC_CCR_PRESC_1;
    /*Sampling time selection: 7 => 160 clocks => 40µs @ 4MHz (must be 10+10 for ref start and sampling time)*/
    ADC1->SMPR |= ADC_SMPR_SMP;
    //clear previous flag
    ADC1->ISR |= ADC_ISR_EOC;

    /* power off an release device for now */
    done();

    return 0;
}

int adc_sample(adc_t line,  adc_res_t res)
{
    int sample;
    uint8_t _res = 0;

    /* check if resolution is applicable */
    if (res > 0x3) {
        return -1;
    }

    /* lock and power on the ADC device  */
    prep();

    switch (res) {
    case ADC_RES_6BIT:
        _res = 0x11;
        break;
    case ADC_RES_8BIT:
        _res = 0x10;
        break;
    case ADC_RES_10BIT:
        _res = 0x01;
        break;
    case ADC_RES_12BIT:
        _res = 0x00;
        break;
    default:
        return -1;
        break;
    }

    /* Disable ADC before renenabling it */
    if ((ADC1->CR & ADC_CR_ADEN) != 0) /* (1) */
    {
        ADC1->CR |= ADC_CR_ADDIS;
        while(ADC1->CR & ADC_CR_ADEN) {} /* Wait for ADC disabled */
    }

    if ((ADC1->CR & ADC_CR_ADEN) == 0)
    {
        /* Then, start a calibration */
        ADC1->CR |= ADC_CR_ADCAL;
        while(ADC1->CR & ADC_CR_ADCAL) {} /* Wait for the end of calibration */
    }

    /* Clear flag */
    ADC1->ISR |= ADC_ISR_ADRDY;

    /* enable device */
    ADC1->CR = ADC_CR_ADVREGEN | ADC_CR_ADEN;

    /* Wait for ADC to be ready */
    while (!(ADC1->ISR & ADC_ISR_ADRDY)) {}

    if (adc_config[line].chan == 17) {
        ADC->CCR |= ADC_CCR_VREFEN;
    }
    else if (adc_config[line].chan == 18) {
        ADC->CCR |= ADC_CCR_TSEN;
    }
    /* else nothing */

    /* set resolution and channel */
    ADC1->CFGR1 &= ~ADC_CFGR1_RES;
    ADC1->CFGR1 |= _res << ADC_CFGR1_RES_Pos;
    ADC1->CHSELR = (1 << adc_config[line].chan);

    /* clear flag */
    ADC1->ISR |= ADC_ISR_EOC;

    /* start conversion and wait for results */
    ADC1->CR |= ADC_CR_ADSTART;

    while (!(ADC1->ISR & ADC_ISR_EOC)) {}

    /* read result */
    sample = (int)ADC1->DR;

    /* Disable ADC */
    if ((ADC1->CR & ADC_CR_ADEN) != 0) /* (1) */
    {
        ADC1->CR |= ADC_CR_ADDIS;
        while(ADC1->CR & ADC_CR_ADEN) {} /* Wait for ADC disabled */
        /* Disable Voltage regulator */
        ADC1->CR = 0;
        ADC1->ISR = 0;
    }

    /* Deactivate VREFINT and temperature sensor to save power */
    ADC->CCR &= ~(ADC_CCR_VREFEN | ADC_CCR_TSEN);

    /* unlock and power off device again */
    done();

    return sample;
}
#else
typedef int dont_be_pedantic;
#endif /* ADC_CONFIG */
