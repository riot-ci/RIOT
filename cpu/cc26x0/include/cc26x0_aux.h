/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_cc26x0_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 AUX register definitions
 */

#ifndef CC26X0_AUX_H
#define CC26X0_AUX_H

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   AUX_AIODIO registers
 */
typedef struct {
    reg32_t GPIODOUT; /**< GPIO data out */
    reg32_t IOMODE; /**< Input output mode */
    reg32_t GPIODIN; /**< GPIO data in */
    reg32_t GPIODOUTSET; /**< GPIO data out set */
    reg32_t GPIODOUTCLR; /**< GPIO data out clear */
    reg32_t GPIODOUTTGL; /**< GPIO data out toggle */
    reg32_t GPIODIE; /**< GPIO data input enable */
} aux_aiodio_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_AIODIO0 base address
 */
#define AUX_AIODIO0_BASE     (PERIPH_BASE + 0xC1000)
/**
 * @brief   AUX_AIODIO1 base address
 */
#define AUX_AIODIO1_BASE     (PERIPH_BASE + 0xC2000)
/** @} */

/**
 * @brief   AUX_AIODIO0 register bank
 */
#define AUX_AIODIO0          ((aux_aiodio_regs_t *) (AUX_AIODIO0_BASE))
/**
 * @brief   AUX_AIODIO1 register bank
 */
#define AUX_AIODIO1          ((aux_aiodio_regs_t *) (AUX_AIODIO1_BASE))

/**
 * @brief   AUX_TDC registers
 */
typedef struct {
    reg32_t CTL; /**< Control */
    reg32_t STAT; /**< Status */
    reg32_t RESULT; /**< Result */
    reg32_t SATCFG; /**< Saturaion configuration */
    reg32_t TRIGSRC; /**< Trigger source */
    reg32_t TRIGCNT; /**< Trigger counter */
    reg32_t TRIGCNTLOAD; /**< Trigger counter load */
    reg32_t TRIGCNTCFG; /**< Trigger counter config */
    reg32_t PRECTL; /**< Prescaler control */
    reg32_t PRECNT; /**< Prescaler counter */
} aux_tdc_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_TDC base address
 */
#define AUX_TDC_BASE         (PERIPH_BASE + 0xC4000)
/** @} */

/**
 * @brief   AUX_TDC register bank
 */
#define AUX_TDC              ((aux_tdc_regs_t *) (AUX_TDC_BASE))

/**
 * @brief   AUX_EVCTL registers
 */
typedef struct {
    reg32_t VECCFG0; /**< Vector config 0 */
    reg32_t VECCFG1; /**< Vector config 1 */
    reg32_t SCEWEVSEL; /**< Sensor controller engine wait event selection */
    reg32_t EVTOAONFLAGS; /**< Events to AON domain flags */
    reg32_t EVTOAONPOL; /**< Events to AON domain polarity */
    reg32_t DMACTL; /**< Direct memoty access control */
    reg32_t SWEVSET; /**< Software event set */
    reg32_t EVSTAT0; /**< Event status 0 */
    reg32_t EVSTAT1; /**< Event status 1 */
    reg32_t EVTOMCUPOL; /**< Event to MCU domain polarity */
    reg32_t EVTOMCUFLAGS; /**< Event to MCU domain flags */
    reg32_t COMBEVTOMCUMASK; /**< Combined event to MCU domain mask */
    reg32_t __reserved; /**< Reserved */
    reg32_t VECFLAGS; /**< Vector flags */
    reg32_t EVTOMCUFLAGSCLR; /**< Events to MCU domain flags clear */
    reg32_t EVTOAONFLAGSCLR; /**< Events to AON domain clear */
    reg32_t VECFLAGSCLR; /**< Vector flags clear */
} aux_evtcl_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_EVCTL base address
 */
#define AUX_EVCTL_BASE       (PERIPH_BASE + 0xC5000)
/** @} */

/**
 * @brief   AUX_EVCTL register bank
 */
#define AUX_EVCTL            ((aux_evctl_regs_t *) (AUX_EVCTL_BASE))

/**
 * @brief   AUX_WUC registers
 */
typedef struct {
    reg32_t MODCLKEN0; /**< Module clock enable */
    reg32_t PWROFFREQ; /**< Power off request */
    reg32_t PWRDWNREQ; /**< Power down request */
    reg32_t PWRDWNACK; /**<  Power down acknowledgement */
    reg32_t CLKLFREQ; /**< Low frequency clock request */
    reg32_t CLKLFACK; /**< Low frequency clock acknowledgement */
    reg32_t __reserved1[4]; /**< Reserved */
    reg32_t WUEVFLAGS; /**< Wake-up event flags */
    reg32_t WUEVCLR; /**< Wake-up event clear */
    reg32_t ADCCLKCTL; /**< ADC clock control */
    reg32_t TDCCLKCTL; /**< TDC clock control */
    reg32_t REFCLKCTL; /**< Reference clock control */
    reg32_t RTCSUBSECINC0; /**< Real time counter sub second increment 0 */
    reg32_t RTCSUBSECINC1; /**< Real time counter sub second increment 1 */
    reg32_t RTCSUBSECINCCTL; /**< Real time counter sub second increment control */
    reg32_t MCUBUSCTL; /**< MCU bus control */
    reg32_t MCUBUSSTAT; /**< MCU bus status */
    reg32_t AONCTLSTAT; /**< AON domain control status */
    reg32_t AUXIOLATCH; /**< AUX input output latch */
    reg32_t MODCLKEN1; /**< Module clock enable 1 */
} aux_wuc_regs_t;

/**
 * @brief   AUX_WUC register values
 * @{
 */
#define MODCLKEN0_SMPH_EN           0x00000001  /* enable clock for AUX_SMPH */
#define MODCLKEN0_AIODIO0_EN        0x00000002  /* enable clock for AUX_AIODIO0 */
#define MODCLKEN0_AIODIO1_EN        0x00000004  /* enable clock for AUX_AIODIO1 */
#define MODCLKEN0_TIMER_EN          0x00000008  /* enable clock for AUX_TIMER */
#define MODCLKEN0_ANAIF_EN          0x00000010  /* enable clock for AUX_ANAIF */
#define MODCLKEN0_TDC_EN            0x00000020  /* enable clock for AUX_TDC */
#define MODCLKEN0_AUX_DDI0_OSC_EN   0x00000040  /* enable clock for AUX_DDI0_OSC */
#define MODCLKEN0_AUX_ADI4_EN       0x00000080  /* enable clock for AUX_ADI4 */
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_WUC base address
 */
#define AUX_WUC_BASE         (PERIPH_BASE + 0xC6000)
/** @} */

/**
 * @brief   AUX_WUC register bank
 */
#define AUX_WUC              ((aux_wuc_regs_t *) (AUX_WUC_BASE))

/**
 * @brief   AUX_TIMER registers
 */
typedef struct {
    reg32_t T0CFG; /**< Timer 0 config */
    reg32_t T1CFG; /**< Timer 1 config */
    reg32_t T0CTL; /**< Timer 0 control */
    reg32_t T0TARGET; /**< Timer 0 target */
    reg32_t T1TARGET; /**< Timer 1 target */
    reg32_t T1CTL; /**< Timer 1 control */
} aux_timer_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_WUC base address
 */
#define AUX_TIMER_BASE       (PERIPH_BASE + 0xC7000)
/** @} */

/**
 * @brief   AUX_TIMER register bank
 */
#define AUX_TIMER            ((aux_timer_regs_t *) (AUX_TIMER_BASE))

/**
 * @brief   AUX_SMPH registers
 */
typedef struct {
    reg32_t SMPH0; /**< Semaphore 0 */
    reg32_t SMPH1; /**< Semaphore 1 */
    reg32_t SMPH2; /**< Semaphore 2 */
    reg32_t SMPH3; /**< Semaphore 3 */
    reg32_t SMPH4; /**< Semaphore 4 */
    reg32_t SMPH5; /**< Semaphore 5 */
    reg32_t SMPH6; /**< Semaphore 6 */
    reg32_t SMPH7; /**< Semaphore 7 */
    reg32_t AUTOTAKE; /**< Sticky request for single semaphore */
} aux_smph_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_SMPH base address
 */
#define AUX_SMPH_BASE        (PERIPH_BASE + 0xC8000)
/* @} */

/**
 * @brief   AUX_SMPH register bank
 */
#define AUX_SMPH             ((aux_smph_regs_t *) (AUX_SMPH_BASE))

/**
 * @brief   AUX_ANAIF registers
 */
typedef struct {
    reg32_t __reserved[4]; /**< Reserved */
    reg32_t ADCCTL; /**< ADC control */
    reg32_t ADCFIFOSTAT; /**< ADC fifo status */
    reg32_t ADCFIFO; /**< ADC fifo */
    reg32_t ADCTRIG; /**< ADC trigger */
    reg32_t ISRCCTL; /**< current source control */
} aux_anaif_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_ANAIF base address
 */
#define AUX_ANAIF_BASE       (PERIPH_BASE + 0xC9000)
/** @} */

/**
 * @brief   AUX_ANAIF register bank
 */
#define AUX_ANAIF            ((aux_anaif_regs_t *) (AUX_ANAIF_BASE))

/**
 * @brief   ADI_4_AUX registers
 */
typedef struct {
    reg8_t MUX0; /**< Multiplexer 0 */
    reg8_t MUX1; /**< Multiplexer 1 */
    reg8_t MUX2; /**< Multiplexer 2 */
    reg8_t MUX3; /**< Multiplexer 3 */
    reg8_t ISRC; /**< Current source */
    reg8_t COMP; /**< Comparator */
    reg8_t MUX4; /**< Multiplexer 4 */
    reg8_t ADC0; /**< ADC control 0 */
    reg8_t ADC1; /**< ADC control 1 */
    reg8_t ADCREF0; /**< ADC reference 0 */
    reg8_t ADCREF1; /**< ADC reference 1 */
} adi_4_aux_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   ADI_4_AUX base address
 */
#define ADI_4_AUX_BASE       (PERIPH_BASE + 0xCB000)
/** @} */

/**
 * @brief   ADI_4_AUX register bank
 */
#define ADI_4_AUX            ((adi_4_aux_regs_t *) (ADI_4_AUX_BASE))

/**
 * @brief   The semamphore used for ADDI
 */
#define ADDI_SEM             (AUX_SMPH->SMPH0)

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X0_AUX_H */

/** @}*/
