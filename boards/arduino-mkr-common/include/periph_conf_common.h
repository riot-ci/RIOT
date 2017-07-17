/*
 * Copyright (C)  2016 Freie Universität Berlin
 *                2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-mkr-common
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for Arduino MKR boards
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_COMMON_H
#define PERIPH_CONF_COMMON_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    External oscillator and clock configuration
 *
 * For selection of the used CORECLOCK, we have implemented two choices:
 *
 * - usage of the PLL fed by the internal 8MHz oscillator divided by 8
 * - usage of the internal 8MHz oscillator directly, divided by N if needed
 *
 *
 * The PLL option allows for the usage of a wider frequency range and a more
 * stable clock with less jitter. This is why we use this option as default.
 *
 * The target frequency is computed from the PLL multiplier and the PLL divisor.
 * Use the following formula to compute your values:
 *
 * CORECLOCK = ((PLL_MUL + 1) * 1MHz) / PLL_DIV
 *
 * NOTE: The PLL circuit does not run with less than 32MHz while the maximum PLL
 *       frequency is 96MHz. So PLL_MULL must be between 31 and 95!
 *
 *
 * The internal Oscillator used directly can lead to a slightly better power
 * efficiency to the cost of a less stable clock. Use this option when you know
 * what you are doing! The actual core frequency is adjusted as follows:
 *
 * CORECLOCK = 8MHz / DIV
 *
 * NOTE: A core clock frequency below 1MHz is not recommended
 *
 * @{
 */
#define CLOCK_USE_PLL       (1)

#if CLOCK_USE_PLL
/* edit these values to adjust the PLL output frequency */
#define CLOCK_PLL_MUL       (47U)               /* must be >= 31 & <= 95 */
#define CLOCK_PLL_DIV       (1U)                /* adjust to your needs */
/* generate the actual used core clock frequency */
#define CLOCK_CORECLOCK     (((CLOCK_PLL_MUL + 1) * 1000000U) / CLOCK_PLL_DIV)
#else
/* edit this value to your needs */
#define CLOCK_DIV           (1U)
/* generate the actual core clock frequency */
#define CLOCK_CORECLOCK     (8000000 / CLOCK_DIV)
#endif
/** @} */

/**
 * @name Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         TC3->COUNT16
#define TIMER_0_CHANNELS    2
#define TIMER_0_MAX_VALUE   (0xffff)
#define TIMER_0_ISR         isr_tc3

/* Timer 1 configuration */
#define TIMER_1_DEV         TC4->COUNT32
#define TIMER_1_CHANNELS    2
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_ISR         isr_tc4
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev    = &SERCOM5->USART,
        .rx_pin = GPIO_PIN(PB,23),
        .tx_pin = GPIO_PIN(PB,22),
        .mux    = GPIO_MUX_D,
        .rx_pad = UART_PAD_RX_3,
        .tx_pad = UART_PAD_TX_2
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom5

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name PWM configuration
 * @{
 */
#define PWM_0_EN            1
#define PWM_1_EN            1
#define PWM_MAX_CHANNELS    2
/* for compatibility with test application */
#define PWM_0_CHANNELS      PWM_MAX_CHANNELS
#define PWM_1_CHANNELS      PWM_MAX_CHANNELS

/* PWM device configuration */
static const pwm_conf_t pwm_config[] = {
#if PWM_0_EN
    {TCC0, {
        /* GPIO pin, MUX value, TCC channel */
        { GPIO_PIN(PA, 8), GPIO_MUX_E,  0 },
        { GPIO_PIN(PA, 9), GPIO_MUX_E,  1 },
    }},
#endif
#if PWM_1_EN
    {TCC1, {
        /* GPIO pin, MUX value, TCC channel */
        { GPIO_PIN(PA, 6), GPIO_MUX_E, 0 },
        { GPIO_PIN(PA, 7), GPIO_MUX_E, 1 },
    }},
#endif
};

/* number of devices that are actually defined */
#define PWM_NUMOF           (2U)
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define ADC_CONFIG {            \
    { GPIO_PIN(PA, 2), 0, 0  }, \
    { GPIO_PIN(PB, 2), 0, 2  }, \
    { GPIO_PIN(PB, 3), 0, 3  }, \
    { GPIO_PIN(PA, 4), 0, 4  }, \
    { GPIO_PIN(PA, 5), 0, 5  }, \
    { GPIO_PIN(PA, 6), 0, 10 }, \
    { GPIO_PIN(PA, 7), 0, 10 }}

#define ADC_NUMOF           (6)
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = &SERCOM1->SPI,
        .miso_pin = GPIO_PIN(PA, 19),
        .mosi_pin = GPIO_PIN(PB, 16),
        .clk_pin  = GPIO_PIN(PB, 17),
        .miso_mux = GPIO_MUX_D,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1
    },
    {
        .dev      = &SERCOM2->SPI,
        .miso_pin = GPIO_PIN(PA, 15),
        .mosi_pin = GPIO_PIN(PA, 12),
        .clk_pin  = GPIO_PIN(PA, 13),
        .miso_mux = GPIO_MUX_D,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_2_SCK_3
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF          (1U)
#define I2C_0_EN            1
#define I2C_1_EN            0
#define I2C_2_EN            0
#define I2C_3_EN            0
#define I2C_IRQ_PRIO        1

#define I2C_0_DEV           SERCOM0->I2CM
#define I2C_0_IRQ           SERCOM0_IRQn
#define I2C_0_ISR           isr_sercom0
/* I2C 0 GCLK */
#define I2C_0_GCLK_ID       SERCOM0_GCLK_ID_CORE
#define I2C_0_GCLK_ID_SLOW  SERCOM0_GCLK_ID_SLOW
/* I2C 0 pin configuration */
#define I2C_0_SDA           GPIO_PIN(PA, 8)
#define I2C_0_SCL           GPIO_PIN(PA, 9)
#define I2C_0_MUX           GPIO_MUX_C
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
