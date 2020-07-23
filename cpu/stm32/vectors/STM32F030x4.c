/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions for STM32F030x4 cpu line
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* STM32F030x4 specific interrupt vectors */
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_dma1_channel1(void);
WEAK_DEFAULT void isr_dma1_channel2_3(void);
WEAK_DEFAULT void isr_dma1_channel4_5(void);
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_tim1_brk_up_trg_com(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim14(void);
WEAK_DEFAULT void isr_tim16(void);
WEAK_DEFAULT void isr_tim17(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_usart1(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* shared vectors for all family members */
    [ 0] = isr_wwdg,                 /* [ 0] Window WatchDog Interrupt */
    [ 2] = isr_rtc,                  /* [ 2] RTC Interrupt through EXTI Lines 17, 19 and 20 */
    [ 3] = isr_flash,                /* [ 3] FLASH global Interrupt */
    [ 4] = isr_rcc,                  /* [ 4] RCC global Interrupt */
    [ 5] = isr_exti,                 /* [ 5] EXTI Line 0 and 1 Interrupt */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line 2 and 3 Interrupt */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line 4 to 15 Interrupt */
    [ 9] = isr_dma1_channel1,        /* [ 9] DMA1 Channel 1 Interrupt */
    [10] = isr_dma1_channel2_3,      /* [10] DMA1 Channel 2 and Channel 3 Interrupt */
    [11] = isr_dma1_channel4_5,      /* [11] DMA1 Channel 4 and Channel 5 Interrupt */
    [12] = isr_adc1,                 /* [12] ADC1 Interrupt */
    [13] = isr_tim1_brk_up_trg_com,  /* [13] TIM1 Break, Update, Trigger and Commutation Interrupt */
    [14] = isr_tim1_cc,              /* [14] TIM1 Capture Compare Interrupt */
    [16] = isr_tim3,                 /* [16] TIM3 global Interrupt */
    [19] = isr_tim14,                /* [19] TIM14 global Interrupt */
    [21] = isr_tim16,                /* [21] TIM16 global Interrupt */
    [22] = isr_tim17,                /* [22] TIM17 global Interrupt */
    [23] = isr_i2c1,                 /* [23] I2C1 Event Interrupt */
    [25] = isr_spi1,                 /* [25] SPI1 global Interrupt */
    [27] = isr_usart1,               /* [27] USART1 global Interrupt */
};
