/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-kl43z
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the FRDM-KL43Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock system configuration
 * @{
 */
static const clock_config_t clock_config = {
    /*
     * This configuration results in the system running with the internal clock
     * with the following clock frequencies:
     * Core:   8 MHz
     * Bus:    8 MHz
     * Flash:  8 MHz
     */
    .clkdiv1            = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(0),
    /* unsure if this RTC load cap configuration is correct */
    .rtc_clc            = RTC_CR_SC8P_MASK | RTC_CR_SC4P_MASK,
    /* Use the 32 kHz system oscillator output as ERCLK32K. */
    .osc32ksel          = SIM_SOPT1_OSC32KSEL(0),
    .clock_flags =
        KINETIS_CLOCK_RTCOSC_EN |
        KINETIS_CLOCK_USE_FAST_IRC |
        KINETIS_CLOCK_MCGIRCLK_EN | /* Used for LPUART clocking */
        KINETIS_CLOCK_MCGIRCLK_STOP_EN |
        0,
    /* Using LIRC8M mode by default */
    .default_mode       = KINETIS_MCG_MODE_LIRC8M,
    /* The crystal connected to EXTAL0 is 32.768 kHz */
    .erc_range          = KINETIS_MCG_ERC_RANGE_LOW,
    .osc_clc            = 0, /* no load cap configuration, rtc_clc overrides this value on KL43Z */
    .fcrdiv             = MCG_SC_FCRDIV(0), /* LIRC_DIV1 divide by 1 => 8 MHz */
    .lirc_div2          = MCG_MC_LIRC_DIV2(0), /* LIRC_DIV2 divide by 1 => 8 MHz */
};
#define CLOCK_CORECLOCK              ( 8000000ul)
#define CLOCK_MCGIRCLK               ( 8000000ul)
#define CLOCK_BUSCLOCK               (CLOCK_CORECLOCK / 1)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define PIT_NUMOF               (1U)
#define PIT_CONFIG {             \
    {                            \
        .prescaler_ch = 0,       \
        .count_ch = 1,           \
    },                           \
}
#define LPTMR_NUMOF             (1U)
#define LPTMR_CONFIG {           \
    {                            \
        .dev = LPTMR0,           \
        .irqn = LPTMR0_IRQn,     \
    }                            \
}
#define TIMER_NUMOF             ((PIT_NUMOF) + (LPTMR_NUMOF))

#define PIT_BASECLOCK           (CLOCK_BUSCLOCK)
#define PIT_ISR_0               isr_pit1
#define LPTMR_ISR_0             isr_lptmr0
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev    = LPUART0,
        .freq   = CLOCK_MCGIRCLK,
        .pin_rx = GPIO_PIN(PORT_A,  1),
        .pin_tx = GPIO_PIN(PORT_A,  2),
        .pcr_rx = PORT_PCR_MUX(2),
        .pcr_tx = PORT_PCR_MUX(2),
        .irqn   = LPUART0_IRQn,
        .scgc_addr = &SIM->SCGC5,
        .scgc_bit = SIM_SCGC5_LPUART0_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_LPUART,
    },
};
#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
#define LPUART_0_ISR        isr_lpuart0
/* Use MCGIRCLK (internal reference 4 MHz clock) */
#define LPUART_0_SRC        3
/** @} */

/**
 * @name ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    /* dev, pin, channel */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  0), .chan =  8 }, /* PTB0 (Arduino A0) */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  1), .chan =  9 }, /* PTB1 (Arduino A1) */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  2), .chan = 15 }, /* PTB2 (Arduino A2) */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  3), .chan =  4 }, /* PTB3 (Arduino A3) */
};

#define ADC_NUMOF           (sizeof(adc_config) / sizeof(adc_config[0]))
/*
 * KL43Z ADC reference settings:
 * 0: VREFH/VREFL external pin pair
 * 1: VDDA/VSSA supply pins
 * 2-3: reserved
 */
#define ADC_REF_SETTING     0
/** @} */

/**
* @name I2C configuration
* @{
*/
/* TODO */
#define I2C_NUMOF                    (1U)
#define I2C_0_EN                     1
/* Low (10 kHz): MUL = 2, SCL divider = 1536, total: 3072 */
#define KINETIS_I2C_F_ICR_LOW        (0x36)
#define KINETIS_I2C_F_MULT_LOW       (1)
/* Normal (100 kHz): MUL = 2, SCL divider = 160, total: 320 */
#define KINETIS_I2C_F_ICR_NORMAL     (0x1D)
#define KINETIS_I2C_F_MULT_NORMAL    (1)
/* Fast (400 kHz): MUL = 1, SCL divider = 80, total: 80 */
#define KINETIS_I2C_F_ICR_FAST       (0x14)
#define KINETIS_I2C_F_MULT_FAST      (0)
/* Fast plus (1000 kHz): MUL = 1, SCL divider = 30, total: 30 */
#define KINETIS_I2C_F_ICR_FAST_PLUS  (0x05)
#define KINETIS_I2C_F_MULT_FAST_PLUS (0)

/* I2C 0 device configuration */
#define I2C_0_DEV                    I2C0
#define I2C_0_CLKEN()                (SIM->SCGC4 |= (SIM_SCGC4_I2C0_MASK))
#define I2C_0_CLKDIS()               (SIM->SCGC4 &= ~(SIM_SCGC4_I2C0_MASK))
#define I2C_0_IRQ                    I2C0_IRQn
#define I2C_0_IRQ_HANDLER            isr_i2c0
/* I2C 0 pin configuration */
#define I2C_0_PORT                   PORTB
#define I2C_0_PORT_CLKEN()           (SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK))
#define I2C_0_PIN_AF                 2
#define I2C_0_SDA_PIN                3
#define I2C_0_SCL_PIN                2
#define I2C_0_PORT_CFG               (PORT_PCR_MUX(I2C_0_PIN_AF) | PORT_PCR_ODE_MASK)
/** @} */

/**
* @name RTT and RTC configuration
* @{
*/
#define RTT_NUMOF                    (1U)
#define RTC_NUMOF                    (1U)
#define RTT_DEV                      RTC
#define RTT_IRQ                      RTC_IRQn
#define RTT_IRQ_PRIO                 10
#define RTT_UNLOCK()                 (SIM->SCGC6 |= (SIM_SCGC6_RTC_MASK))
#define RTT_ISR                      isr_rtc
#define RTT_FREQUENCY                (1)
#define RTT_MAX_VALUE                (0xffffffff)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
