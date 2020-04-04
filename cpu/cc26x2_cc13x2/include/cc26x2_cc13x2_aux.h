/*
 * Copyright (C) 2016 Leon George
 * Copyright (C) 2018 Anton Gerasimov
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_cc26x2_cc13x2_definitions
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 AUX register definitions
 */

#ifndef CC26X2_CC13X2_AUX_H
#define CC26X2_CC13X2_AUX_H

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   AUX_AIODIO registers
 */
typedef struct {
    reg32_t IOMODE; /**< Input output mode */
    reg32_t GPIODIE; /**< GPIO data input enable */
    reg32_t IOPOE; /**< IO peripheral output enable */
    reg32_t GPIODOUT; /**< GPIO data output */
    reg32_t GPIODIN; /**< GPIO data input */
    reg32_t GPIODOUTSET; /**< GPIO data out set */
    reg32_t GPIODOUTCLR; /**< GPIO data out clear */
    reg32_t GPIODOUTTGL; /**< GPIO data out toggle */
    reg32_t IO0PSEL; /**< I/O 0 peripheral select */
    reg32_t IO1PSEL; /**< I/O 1 peripheral select */
    reg32_t IO2PSEL; /**< I/O 2 peripheral select */
    reg32_t IO3PSEL; /**< I/O 3 peripheral select */
    reg32_t IO4PSEL; /**< I/O 4 peripheral select */
    reg32_t IO5PSEL; /**< I/O 5 peripheral select */
    reg32_t IO6PSEL; /**< I/O 6 peripheral select */
    reg32_t IO7PSEL; /**< I/O 7 peripheral select */
    reg32_t IOMODEH; /**< Input output mode high */
    reg32_t IOMODEL; /**< Input output mode low */
} aux_aiodio_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_AIODIO0 base address
 */
#define AUX_AIODIO0_BASE     (PERIPH_BASE + 0xCC000)
/**
 * @brief   AUX_AIODIO1 base address
 */
#define AUX_AIODIO1_BASE     (PERIPH_BASE + 0xCD000)
/**
 * @brief   AUX_AIODIO2 base address
 */
#define AUX_AIODIO2_BASE     (PERIPH_BASE + 0xCE000)
/**
 * @brief   AUX_AIODIO3 base address
 */
#define AUX_AIODIO3_BASE     (PERIPH_BASE + 0xCF000)
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
 * @brief   AUX_AIODIO2 register bank
 */
#define AUX_AIODIO2          ((aux_aiodio_regs_t *) (AUX_AIODIO2_BASE))
/**
 * @brief   AUX_AIODIO3 register bank
 */
#define AUX_AIODIO3          ((aux_aiodio_regs_t *) (AUX_AIODIO3_BASE))

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
    reg32_t EVSTAT0; /**< Event status 0 */
    reg32_t EVSTAT1; /**< Event status 1 */
    reg32_t EVSTAT2; /**< Event status 2 */
    reg32_t EVSTAT3; /**< Event status 3 */
    reg32_t SCEWEVSEL0; /**< Sensor controller engine wait event configuration 0 */
    reg32_t SCEWEVSEL1; /**< Sensor controller engine wait event configuration 1 */
    reg32_t DMACTL; /**< Direct memoty access control */
    reg32_t __reserved; /**< Reserved */
    reg32_t SWEVSET; /**< Software event set */
    reg32_t EVTOAONFLAGS; /**< Events to AON domain flags */
    reg32_t EVTOAONPOL; /**< Events to AON domain polarity */
    reg32_t EVTOAONFLAGSCLR; /**< Events to AON domain clear */
    reg32_t EVTOMCUFLAGS; /**< Event to MCU domain flags */
    reg32_t EVTOMCUPOL; /**< Event to MCU domain polarity */
    reg32_t EVTOMCUFLAGSCLR; /**< Events to MCU domain flags clear */
    reg32_t COMBEVTOMCUMASK; /**< Combined event to MCU domain mask */
    reg32_t PROGDLY; /**< Programmable delay */
    reg32_t MANUAL; /**< Manual */
    reg32_t EVSTAT0L; /**< Event status 0 low */
    reg32_t EVSTAT0H; /**< Event status 0 high */
    reg32_t EVSTAT1L; /**< Event status 1 low */
    reg32_t EVSTAT1H; /**< Event status 1 high */
    reg32_t EVSTAT2L; /**< Event status 2 low */
    reg32_t EVSTAT2H; /**< Event status 2 high */
    reg32_t EVSTAT3L; /**< Event status 3 low */
    reg32_t EVSTAT3H; /**< Event status 3 high */

} aux_evtcl_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_EVCTL base address
 */
#define AUX_EVCTL_BASE      (PERIPH_BASE + 0xC5000)
/** @} */

/**
 * @brief   AUX_EVCTL register bank
 */
#define AUX_EVCTL           ((aux_evctl_regs_t *) (AUX_EVCTL_BASE))

/**
 * @brief   AUX_SYSIF registers
 */
typedef struct {
    reg32_t OPMODEREQ; /**< Operational mode request */
    reg32_t OPMODEACK; /**< Operational mode acknowledgement */
    reg32_t PROGWU0CFG; /**< Programmanble wakeup 0 configuration */
    reg32_t PROGWU1CFG; /**< Programmanble wakeup 0 configuration */
    reg32_t PROGWU2CFG; /**< Programmanble wakeup 0 configuration */
    reg32_t PROGWU3CFG; /**< Programmanble wakeup 0 configuration */
    reg32_t SWUTRIG; /**< Software wakeup triggers */
    reg32_t WUFLAGS; /**< Wakeup flags */
    reg32_t WUFLAGSCLR; /**< Wakeup flags clear */
    reg32_t WUGATE; /**< Wakeup gate */
    reg32_t VECCFG0; /**< Vector configuration 0 */
    reg32_t VECCFG1; /**< Vector configuration 1 */
    reg32_t VECCFG2; /**< Vector configuration 2 */
    reg32_t VECCFG3; /**< Vector configuration 3 */
    reg32_t VECCFG4; /**< Vector configuration 4 */
    reg32_t VECCFG5; /**< Vector configuration 5 */
    reg32_t VECCFG6; /**< Vector configuration 6 */
    reg32_t VECCFG7; /**< Vector configuration 7 */
    reg32_t EVSYNCRATE; /**< Event synchronization rate */
    reg32_t PEROPRATE; /**< Peripheral operational rate */
    reg32_t ADCCLKCTL; /**< ADC clock control */
    reg32_t TDCCLKCTL; /**< TDC clock control */
    reg32_t TDCREFCLKCTL; /**< TDC reference clock control */
    reg32_t TIMER2CLKCTL; /**< AUX_TIMER2 clock control */
    reg32_t TIMER2CLKSTAT; /**< AUX_TIMER2 clock status */
    reg32_t TIMER2CLKSWITCH; /**< AUX_TIMER2 clock switch */
    reg32_t TIMER2DBGCTL; /**< AUX_TIMER2 debug control */
    reg32_t __reserved1; /**< Reserved */
    reg32_t CLKSHIFTDET; /**< Clock shift detection */
    reg32_t RECHARGETRIG; /**< Recharge trigger */
    reg32_t RECHARGEDET; /**< Recharge detector */
    reg32_t RTCSUBSECINC0; /**< RTC sub-second increment 0 */
    reg32_t RTCSUBSECINC1; /**< RTC sub-second increment 1 */
    reg32_t RTCSUBSECINCCTL; /**< RTC sub-second increment control */
    reg32_t RTCSEC; /**< RTC second */
    reg32_t RTCSUBSEC; /**< RTC sub-second */
    reg32_t RTCEVCLR; /**< RTC event clear */
    reg32_t BATMONBAT; /**< battery voltage */
    reg32_t __reserved2; /**< Reserved */
    reg32_t BATMONTEMP; /**< Batmon temperature */
    reg32_t TIMERHALT; /**< Timer halt */
    reg32_t __reserved3[3]; /**< Reserved */
    reg32_t TIMER2BRIDGE; /**< Timer2 bridge */
    reg32_t SWPWRPROF; /**< Software power profiler */
} aux_sysif_regs_t;

/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_SYSIF base address
 */
#define AUX_SYSIF_BASE      (PERIPH_BASE + 0xC6000)
/** @} */

/**
 * @brief   AUX_SYSIF register bank
 */
#define AUX_SYSIF           ((aux_sysif_regs_t *) (AUX_SYSIF_BASE))

/**
 * @brief   AUX_TIMER registers
 */
typedef struct {
    reg32_t T0CFG; /**< Timer 0 config */
    reg32_t T0CTL; /**< Timer 0 control */
    reg32_t T0TARGET; /**< Timer 0 target */
    reg32_t T0CNTR; /**< Timer 0 counter */
    reg32_t T1CFG; /**< Timer 1 config */
    reg32_t T1TARGET; /**< Timer 1 target */
    reg32_t T1CTL; /**< Timer 1 control */
    reg32_t T1CNTR; /**< Timer 0 counter */
} aux_timer01_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_TIMER01 base address
 */
#define AUX_TIMER01_BASE     (PERIPH_BASE + 0xC7000)
/** @} */

/**
 * @brief   AUX_TIMER01 register bank
 */
#define AUX_TIMER01          ((aux_timer01_regs_t *) (AUX_TIMER01_BASE))

/**
 * @brief   AUX_TIMER2 registers
 */
typedef struct {
    reg32_t CTL; /**< Timer 2 control */
    reg32_t TARGET; /**< Timer 2 target */
    reg32_t SHDWTARGET; /**< Timer 2 shadow target */
    reg32_t CNTR; /**< Timer 2 counter */
    reg32_t PRECFG; /**< Timer 2 prescaler config */
    reg32_t EVCTL; /**< Timer 2 event control */
    reg32_t PULSETRIG; /**< Timer 2 pulse trigger */
    reg32_t __reserved[25]; /**< Reserved */
    reg32_t CH0EVCFG; /**< Timer 2 channel 0 event configuration */
    reg32_t CH0CCFG; /**< Timer 2 channel 0 capture configuration */
    reg32_t CH0PCC; /**< Timer 2 channel 0 pipeline capture compare */
    reg32_t CH0CC; /**< Timer 2 channel 0 capture compare */
    reg32_t CH1EVCFG; /**< Timer 2 channel 1 event configuration */
    reg32_t CH1CCFG; /**< Timer 2 channel 1 capture configuration */
    reg32_t CH1PCC; /**< Timer 2 channel 1 pipeline capture compare */
    reg32_t CH1CC; /**< Timer 2 channel 1 capture compare */
    reg32_t CH2EVCFG; /**< Timer 2 channel 2 event configuration */
    reg32_t CH2CCFG; /**< Timer 2 channel 2 capture configuration */
    reg32_t CH2PCC; /**< Timer 2 channel 2 pipeline capture compare */
    reg32_t CH2CC; /**< Timer 2 channel 2 capture compare */
    reg32_t CH3EVCFG; /**< Timer 2 channel 3 event configuration */
    reg32_t CH3CCFG; /**< Timer 2 channel 3 capture configuration */
    reg32_t CH3PCC; /**< Timer 2 channel 3 pipeline capture compare */
    reg32_t CH3CC; /**< Timer 2 channel 3 capture compare */
} aux_timer2_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   AUX_TIMER2 base address
 */
#define AUX_TIMER2_BASE      (PERIPH_BASE + 0xC3000)
/** @} */

/**
 * @brief   AUX_TIMER2 register bank
 */
#define AUX_TIMER2           ((aux_timer2_regs_t *) (AUX_TIMER2_BASE))

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
/** @} */

/**
 * @brief   AUX_SMPH register bank
 */
#define AUX_SMPH             ((aux_smph_regs_t *) (AUX_SMPH_BASE))

/**
 * @brief   AUX_ANAIF registers
 */
typedef struct {
    reg32_t __reserved1[4]; /**< Reserved */
    reg32_t ADCCTL; /**< ADC control */
    reg32_t ADCFIFOSTAT; /**< ADC fifo status */
    reg32_t ADCFIFO; /**< ADC fifo */
    reg32_t ADCTRIG; /**< ADC trigger */
    reg32_t ISRCCTL; /**< Current source control */
    reg32_t __reserved2[3]; /**< meh */
    reg32_t DACCTL; /**< DAC control */
    reg32_t LPMBIASCTL; /**< Low-power mode bias control */
    reg32_t DACSMPLCTL; /**< DAC sample control */
    reg32_t DACSMPLCFG0; /**< DAC sample configuration 0 */
    reg32_t DACSMPLCFG1; /**< DAC sample configuration 1 */
    reg32_t DACVALUE; /**< DAC value */
    reg32_t DACSTAT; /**< DAC status */
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
 * @brief   AUX_WUC base address
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

#endif /* CC26X2_CC13X2_AUX_H */

/** @}*/
