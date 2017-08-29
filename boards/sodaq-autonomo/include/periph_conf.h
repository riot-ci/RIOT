/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-autonomo
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the SODAQ Autonomo board
 *
 * @author      Kees Bakker <kees@sodaq.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#include "cpu.h"
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
 * @name    Timer peripheral configuration
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
 * @name    UART configuration
 * @{
 * See Table 6.1 of the SAM D21 Datasheet
 */
static const uart_conf_t uart_config[] = {
    {
        .dev      = &SERCOM0->USART,
        .rx_pin   = GPIO_PIN(PA,9),
        .tx_pin   = GPIO_PIN(PA,10),
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_2,
        .runstdby = 0,
        .gclk_src = GCLK_CLKCTRL_GEN_GCLK0
    },
    {
        .dev      = &SERCOM5->USART,
        .rx_pin   = GPIO_PIN(PB,31),
        .tx_pin   = GPIO_PIN(PB,30),
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0_RTS_2_CTS_3,
        .runstdby = 0,
        .gclk_src = GCLK_CLKCTRL_GEN_GCLK0
    },
    {
        .dev      = &SERCOM4->USART,
        .rx_pin   = GPIO_PIN(PB,13),
        .tx_pin   = GPIO_PIN(PA,14),
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_2,
        .runstdby = 0,
        .gclk_src = GCLK_CLKCTRL_GEN_GCLK0
    },
    {
        .dev      = &SERCOM1->USART,
        .rx_pin   = GPIO_PIN(PA,17),
        .tx_pin   = GPIO_PIN(PA,18),
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_2,
        .runstdby = 0,
        .gclk_src = GCLK_CLKCTRL_GEN_GCLK0
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom0
#define UART_1_ISR          isr_sercom5
#define UART_2_ISR          isr_sercom4
#define UART_3_ISR          isr_sercom1

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
#define PWM_0_EN            1
#define PWM_1_EN            1
#define PWM_MAX_CHANNELS    3
/* for compatibility with test application */
#define PWM_0_CHANNELS      PWM_MAX_CHANNELS
#define PWM_1_CHANNELS      PWM_MAX_CHANNELS

/* PWM device configuration */
static const pwm_conf_t pwm_config[] = {
#if PWM_0_EN
    {TCC1, {
        /* GPIO pin, MUX value, TCC channel */
        { GPIO_PIN(PA, 6), GPIO_MUX_E, 0 },
        { GPIO_PIN(PA, 7), GPIO_MUX_E, 1 },
        { GPIO_UNDEF, (gpio_mux_t)0, 2 }
    }},
#endif
#if PWM_1_EN
    {TCC0, {
        /* GPIO pin, MUX value, TCC channel */
        { GPIO_PIN(PA, 16), GPIO_MUX_F, 0 },
        { GPIO_PIN(PA, 18), GPIO_MUX_F, 2 },
        { GPIO_PIN(PA, 19), GPIO_MUX_F, 3 }
    }}
#endif
};

/* number of devices that are actually defined */
#define PWM_NUMOF           (2U)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = &SERCOM3->SPI,
        .miso_pin = GPIO_PIN(PA, 22),
        .mosi_pin = GPIO_PIN(PA, 20),
        .clk_pin  = GPIO_PIN(PA, 21),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_0,
        .mosi_pad = SPI_PAD_MOSI_2_SCK_3,
    },
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
#define I2C_NUMOF           (1U)
#define I2C_0_EN            1
#define I2C_1_EN            0
#define I2C_2_EN            0
#define I2C_3_EN            0
#define I2C_IRQ_PRIO        1

#define I2C_0_DEV           SERCOM2->I2CM
#define I2C_0_IRQ           SERCOM2_IRQn
#define I2C_0_ISR           isr_sercom2
/* I2C 0 GCLK */
#define I2C_0_GCLK_ID       SERCOM2_GCLK_ID_CORE
#define I2C_0_GCLK_ID_SLOW  SERCOM2_GCLK_ID_SLOW
/* I2C 0 pin configuration */
#define I2C_0_SDA           GPIO_PIN(PA, 12)
#define I2C_0_SCL           GPIO_PIN(PA, 13)
#define I2C_0_MUX           GPIO_MUX_C

/**
 * @name    RTC configuration
 * @{
 */
#define RTC_NUMOF          (1U)
#define RTC_DEV            RTC->MODE2
/** @} */

/**
 * @name    RTT configuration
 * @{
 */
#define RTT_NUMOF          (1U)
#define RTT_DEV            RTC->MODE0
#define RTT_IRQ            RTC_IRQn
#define RTT_IRQ_PRIO       10
#define RTT_ISR            isr_rtc
#define RTT_MAX_VALUE      (0xffffffff)
#define RTT_FREQUENCY      (32768U)    /* in Hz. For changes see `rtt.c` */
#define RTT_RUNSTDBY       (1)         /* Keep RTT running in sleep states */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
