/*
 * Copyright (C) 2016 Leon George
 * Copyright (C) 2018 Anton Gerasimov
 * Copyright (C) 2020 Locha Inc
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
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
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
 * @brief   AUX_SYSIF register values
 * @{
 */
#define AUX_SYSIF_OPMODEREQ_REQ_PDLP 0x00000003
#define AUX_SYSIF_OPMODEREQ_REQ_PDA  0x00000002
#define AUX_SYSIF_OPMODEREQ_REQ_LP   0x00000001
#define AUX_SYSIF_OPMODEREQ_REQ_A    0x00000000
/** @} */

/**
 * @brief   AUX_SYSIF functions
 * @{
 */
/**
 * @brief   Controls AUX operational mode change
 *
 * @param[in] target_opmode The new operational mode.
 */
void aux_sysif_opmode_change(uint32_t target_opmode);
/** @} */

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
 * @brief   ADI_4_AUX register values
 * @{
 */
#define ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_m 0x00000038
#define ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_s 3
#define ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_m    0x0000003F
#define ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_s    0
/** @} */

/**
 * @brief   ADI_4_AUX register offsets
 * @{
 */
#define ADI_4_AUX_MUX0       0x00000000 /**< Internal */
#define ADI_4_AUX_MUX1       0x00000001 /**< Internal */
#define ADI_4_AUX_MUX2       0x00000002 /**< Internal */
#define ADI_4_AUX_MUX3       0x00000003 /**< Internal */
#define ADI_4_AUX_ISRC       0x00000004 /**< Current source */
#define ADI_4_AUX_COMP       0x00000005 /**< Comparator */
#define ADI_4_AUX_MUX4       0x00000007 /**< Internal */
#define ADI_4_AUX_ADC0       0x00000008 /**< ADC Control 0 */
#define ADI_4_AUX_ADC1       0x00000009 /**< ADC Control 1 */
#define ADI_4_AUX_ADCREF0    0x0000000A /**< ADC Reference 0 */
#define ADI_4_AUX_ADCREF1    0x0000000B /**< ADC Reference 1 */
#define ADI_4_AUX_LPMBIAS    0x0000000E /**< Internal */
/** @} */

/**
 * @brief   ADI_3_REFSYS registers
 */
typedef struct {
    reg8_t ATESTCTL1; /**< Internal */
    reg8_t REFSYSCTL0; /**< Internal */
    reg8_t REFSYSCTL1; /**< Internal */
    reg8_t REFSYSCTL2; /**< Internal */
    reg8_t REFSYSCTL3; /**< Internal */
    reg8_t DCDCCTL0; /**< DCDC Control 0 */
    reg8_t DCDCCTL1; /**< DCDC Control 1 */
    reg8_t DCDCCTL2; /**< DCDC Control 2 */
    reg8_t DCDCCTL3; /**< Internal */
    reg8_t DCDCCTL4; /**< Internal */
    reg8_t DCDCCTL5; /**< Internal */
    reg8_t AUX_DEBUG; /**< Recharge control 1 */
    reg8_t CTL_RECHARGE_CMP0; /** Recharge Comparator Control Byte 0 */
    reg8_t CTL_RECHARGE_CMP1; /** Recharge Comparator Control Byte 1 */
} adi_3_refsys_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   ADI3 base address
 */
#define ADI3_BASE            (PERIPH_BASE + 0x86200)
/** @} */

#define ADI_3_REFSYS         ((adi_3_refsys_regs_t *) (ADI3_BASE))

/**
 * @brief   ADI_3_REFSYS register values
 * @{
 */
#define ADI_3_REFSYS_DCDCCTL3_VDDR_BOOST_COMP_BOOST 0x00000002
#define ADI_3_REFSYS_AUX_DEBUG_LPM_BIAS_BACKUP_EN   0x00000040
/** @} */

/**
 * @brief   ADI_3_REFSYS register offsets
 * @{
 */
#define ADI_3_REFSYS_ATESTCTL1         0x00000001 /**< Internal */
#define ADI_3_REFSYS_REFSYSCTL0        0x00000002 /**< Internal */
#define ADI_3_REFSYS_REFSYSCTL1        0x00000003 /**< Internal */
#define ADI_3_REFSYS_REFSYSCTL2        0x00000004 /**< Internal */
#define ADI_3_REFSYS_REFSYSCTL3        0x00000005 /**< Internal */
#define ADI_3_REFSYS_DCDCCTL0          0x00000006 /**< DCDC Control 0 */
#define ADI_3_REFSYS_DCDCCTL1          0x00000007 /**< DCDC Control 1 */
#define ADI_3_REFSYS_DCDCCTL2          0x00000008 /**< DCDC Control 2 */
#define ADI_3_REFSYS_DCDCCTL3          0x00000009 /**< Internal */
#define ADI_3_REFSYS_DCDCCTL4          0x0000000A /**< Internal */
#define ADI_3_REFSYS_DCDCCTL5          0x0000000B /**< Internal */
#define ADI_3_REFSYS_AUX_DEBUG         0x0000000C /**< Recharge control 1 */
/** Recharge Comparator Control Byte 0 */
#define ADI_3_REFSYS_CTL_RECHARGE_CMP0 0x0000000D
/** Recharge Comparator Control Byte 1 */
#define ADI_3_REFSYS_CTL_RECHARGE_CMP1 0x0000000E
/** @} */

/**
 * @brief   The semamphore used for ADDI
 */
#define ADDI_SEM             (AUX_SMPH->SMPH0)

/**
 * @brief   ADI instruction offsets
 * @{
 */
#define ADI_DIR              0x00000000
#define ADI_SET              0x00000010
#define ADI_CLR              0x00000020
#define ADI_MASK4B           0x00000040
#define ADI_MASK8B           0x00000060
#define ADI_MASK16B          0x00000080
/** @} */

/**
 * @brief   ADI instruction functions
 * @{
 */
/**
 * @brief   Write to ADI register
 *
 * @param[in] base The ADI base address.
 * @param[in] reg  The register offset.
 * @param[in] data The data to write.
 */
static inline void adi_write(uint32_t base, uint32_t reg, uint8_t data)
{
    reg8_t *addr = (reg8_t *)(base + reg);
    *addr = data;
}

/**
 * @brief   Write 4 bits into an ADI register
 *
 * @param[in] base The ADI base address.
 * @param[in] reg  The register offset.
 * @param[in] data Data to be written.
 * @param[in] mask 4-bit mask indicating bits to write. Each bit set to `1`
 *            is one bit written,
 */
static inline void adi_write_4bits(uint32_t base, uint32_t reg, uint8_t data,
                                   uint8_t mask)
{
    reg8_t *addr = (reg8_t *)(base + (reg << 1) + ADI_MASK4B);
    *addr = mask | data;
}

/**
 * @brief   Write 8 bits into an ADI register
 *
 * @param[in] base The ADI base address.
 * @param[in] reg  The register offset.
 * @param[in] data Data to be written.
 * @param[in] mask 4-bit mask indicating bits to write. Each bit set to `1`
 *            is one bit written,
 */
static inline void adi_write_8bits(uint32_t base, uint32_t reg, uint8_t data,
                                   uint8_t mask)
{
    reg16_t *addr = (reg16_t *)(base + (reg << 1) + ADI_MASK8B);
    *addr = (((uint16_t)mask) << 8) | (uint16_t)data;
}

/**
 * @brief   ADI set instruction
 *
 * @param[in] base The ADI base address.
 * @param[in] reg  The register offset.
 * @param[in] data The bits to set.
 */
static inline void adi_set(uint32_t base, uint32_t reg, uint8_t data)
{
    reg8_t *addr = (reg8_t *)(base + reg + ADI_SET);
    *addr = data;
}

/**
 * @brief   ADI clear instruction
 *
 * @param[in] base The ADI base address.
 * @param[in] reg  The register offset.
 * @param[in] data The bits to clear.
 */
static inline void adi_clr(uint32_t base, uint32_t reg, uint8_t data)
{
    reg8_t *addr = (reg8_t *)(base + reg + ADI_CLR);
    *addr = data;
}
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_AUX_H */

/** @}*/
