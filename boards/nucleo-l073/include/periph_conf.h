/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-l073
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-l073 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock system configuration
 * @{
 */
#define CLOCK_HSI           (16000000U)         /* internal oscillator */
#define CLOCK_CORECLOCK     (32000000U)         /* desired core clock frequency */

/* configuration of PLL prescaler and multiply values */
/* CORECLOCK := HSI / CLOCK_PLL_DIV * CLOCK_PLL_MUL */
#define CLOCK_PLL_DIV       RCC_CFGR_PLLDIV2
#define CLOCK_PLL_MUL       RCC_CFGR_PLLMUL4
/* configuration of peripheral bus clock prescalers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* AHB clock -> 32MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* APB2 clock -> 32MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV1     /* APB1 clock -> 32MHz */
/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY

/* bus clocks for simplified peripheral initialization, UPDATE MANUALLY! */
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1          (CLOCK_CORECLOCK / 1)
/** @} */

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF4,
        .tx_af      = GPIO_AF4,
        .bus        = APB1,
        .irqn       = USART2_IRQn
    },
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF4,
        .tx_af      = GPIO_AF4,
        .bus        = APB2,
        .irqn       = USART1_IRQn
    },
    {
        .dev        = USART4,
        .rcc_mask   = RCC_APB1ENR_USART4EN,
        .rx_pin     = GPIO_PIN(PORT_C, 11),
        .tx_pin     = GPIO_PIN(PORT_C, 10),
        .rx_af      = GPIO_AF6,
        .tx_af      = GPIO_AF6,
        .bus        = APB1,
        .irqn       = USART4_5_IRQn
    },
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)
#define UART_2_ISR          (isr_usart4_5)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 4) /* D5 */, .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_C, 7) /* D9 */, .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_C, 8)         , .cc_chan = 2 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 } },
        .af       = GPIO_AF2,
        .bus      = APB1
    }
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */

/**
 * @name    SPI configuration
 *
 * @note    The spi_divtable is auto-generated from
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
 * @{
 */
static const uint8_t spi_divtable[2][5] = {
    {       /* for APB1 @ 32000000Hz */
        7,  /* -> 125000Hz */
        5,  /* -> 500000Hz */
        4,  /* -> 1000000Hz */
        2,  /* -> 4000000Hz */
        1   /* -> 8000000Hz */
    },
    {       /* for APB2 @ 32000000Hz */
        7,  /* -> 125000Hz */
        5,  /* -> 500000Hz */
        4,  /* -> 1000000Hz */
        2,  /* -> 4000000Hz */
        1   /* -> 8000000Hz */
    }
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_UNDEF,
        .af       = GPIO_AF0,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name    ADC configuration
 * @{
 */
#define ADC_NUMOF           (0)
/** @} */


/**
 * @name    DAC configuration
 * @{
 */
#define DAC_NUMOF           (0)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_0_EN            1
#define I2C_1_EN            1
#define I2C_NUMOF           (I2C_0_EN + I2C_1_EN)
#define I2C_IRQ_PRIO        1
#define I2C_APBCLK          (CLOCK_APB1)

/* I2C 0 device configuration */
#define I2C_0_DEV           I2C1
#define I2C_0_CLKEN()       (periph_clk_en(APB1, RCC_APB1ENR_I2C1EN))
#define I2C_0_CLKDIS()      (periph_clk_dis(APB1, RCC_APB1ENR_I2C1EN))
#define I2C_0_EVT_IRQ       I2C1_IRQn
#define I2C_0_EVT_ISR       isr_i2c1
/* I2C 0 pin configuration */
#define I2C_0_SCL_PORT      PORT_B
#define I2C_0_SCL_PIN       8
#define I2C_0_SCL_AF        4
#define I2C_0_SCL_CLKEN()   (periph_clk_en(AHB, RCC_IOPENR_GPIOBEN))
#define I2C_0_SDA_PORT      PORT_B
#define I2C_0_SDA_PIN       9
#define I2C_0_SDA_AF        4
#define I2C_0_SDA_CLKEN()   (periph_clk_en(AHB, RCC_IOPENR_GPIOBEN))

/* I2C 1 device configuration */
#define I2C_1_DEV           I2C3
#define I2C_1_CLKEN()       (periph_clk_en(APB1, RCC_APB1ENR_I2C3EN))
#define I2C_1_CLKDIS()      (periph_clk_dis(APB1, RCC_APB1ENR_I2C3EN))
#define I2C_1_EVT_IRQ       I2C3_IRQn
#define I2C_1_EVT_ISR       isr_i2c3
/* I2C 1 pin configuration */
#define I2C_1_SCL_PORT      PORT_A
#define I2C_1_SCL_PIN       8
#define I2C_1_SCL_AF        3
#define I2C_1_SCL_CLKEN()   (periph_clk_en(AHB, RCC_IOPENR_GPIOAEN))
#define I2C_1_SDA_PORT      PORT_B
#define I2C_1_SDA_PIN       5
#define I2C_1_SDA_AF        8
#define I2C_1_SDA_CLKEN()   (periph_clk_en(AHB, RCC_IOPENR_GPIOBEN))
/** @} */

/** @} */

/**
 * @name    RTC configuration
 * @{
 */
#define RTC_NUMOF           (1U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
