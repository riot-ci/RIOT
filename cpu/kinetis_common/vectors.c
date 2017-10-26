/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_kinetis
 * @{
 *
 * @file
 *
 * @brief Interrupt vector for Kinetis MCUs
 *
 * @author Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @note It is not necessary to modify this file to define custom interrupt
 * service routines. All symbols are defined weak, it is only necessary to
 * define a function with the same name in another file to override the default
 * interrupt handlers.
 */

/**
 * @name Interrupt vector definition
 * @{
 */

/* This is needed to homogenize the symbolic IRQ names across different versions
 * of the vendor headers. These must be defined before any vendor headers are
 * included */
#define FTFA_IRQn FTF_IRQn
#define FTFA_Collision_IRQn Read_Collision_IRQn
#define FTFE_IRQn FTF_IRQn
#define FTFE_Collision_IRQn Read_Collision_IRQn
#define FTFL_IRQn FTF_IRQn
#define FTFL_Collision_IRQn Read_Collision_IRQn
#define PMC_IRQn LVD_LVW_IRQn
#define Watchdog_IRQn WDOG_EWM_IRQn

#include "vectors_kinetis.h"

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
#ifdef DMA0
/* Devices with >16 DMA channels combine two channels per IRQ number */
#if defined(DMA_INT_INT16_MASK)
    [DMA0_DMA16_IRQn ] = isr_DMA0_DMA16,      /* DMA Channel 0, 16 Transfer Complete */
#elif defined(DMA_INT_INT0_MASK)
    [DMA0_IRQn       ] = isr_DMA0,            /* DMA Channel 0 Transfer Complete */
#endif
#if defined(DMA_INT_INT17_MASK)
    [DMA1_DMA17_IRQn ] = isr_DMA1_DMA17,      /* DMA Channel 1, 17 Transfer Complete */
#elif defined(DMA_INT_INT1_MASK)
    [DMA1_IRQn       ] = isr_DMA1,            /* DMA Channel 1 Transfer Complete */
#endif
#if defined(DMA_INT_INT18_MASK)
    [DMA2_DMA18_IRQn ] = isr_DMA2_DMA18,      /* DMA Channel 2, 18 Transfer Complete */
#elif defined(DMA_INT_INT2_MASK)
    [DMA2_IRQn       ] = isr_DMA2,            /* DMA Channel 2 Transfer Complete */
#endif
#if defined(DMA_INT_INT19_MASK)
    [DMA3_DMA19_IRQn ] = isr_DMA3_DMA19,      /* DMA Channel 3, 19 Transfer Complete */
#elif defined(DMA_INT_INT3_MASK)
    [DMA3_IRQn       ] = isr_DMA3,            /* DMA Channel 3 Transfer Complete */
#endif
#if defined(DMA_INT_INT20_MASK)
    [DMA4_DMA20_IRQn ] = isr_DMA4_DMA20,      /* DMA Channel 4, 20 Transfer Complete */
#elif defined(DMA_INT_INT4_MASK)
    [DMA4_IRQn       ] = isr_DMA4,            /* DMA Channel 4 Transfer Complete */
#endif
#if defined(DMA_INT_INT21_MASK)
    [DMA5_DMA21_IRQn ] = isr_DMA5_DMA21,      /* DMA Channel 5, 21 Transfer Complete */
#elif defined(DMA_INT_INT5_MASK)
    [DMA5_IRQn       ] = isr_DMA5,            /* DMA Channel 5 Transfer Complete */
#endif
#if defined(DMA_INT_INT22_MASK)
    [DMA6_DMA22_IRQn ] = isr_DMA6_DMA22,      /* DMA Channel 6, 22 Transfer Complete */
#elif defined(DMA_INT_INT6_MASK)
    [DMA6_IRQn       ] = isr_DMA6,            /* DMA Channel 6 Transfer Complete */
#endif
#if defined(DMA_INT_INT23_MASK)
    [DMA7_DMA23_IRQn ] = isr_DMA7_DMA23,      /* DMA Channel 7, 23 Transfer Complete */
#elif defined(DMA_INT_INT7_MASK)
    [DMA7_IRQn       ] = isr_DMA7,            /* DMA Channel 7 Transfer Complete */
#endif
#if defined(DMA_INT_INT24_MASK)
    [DMA8_DMA24_IRQn ] = isr_DMA8_DMA24,      /* DMA Channel 8, 24 Transfer Complete */
#elif defined(DMA_INT_INT8_MASK)
    [DMA8_IRQn       ] = isr_DMA8,            /* DMA Channel 8 Transfer Complete */
#endif
#if defined(DMA_INT_INT25_MASK)
    [DMA9_DMA25_IRQn ] = isr_DMA9_DMA25,      /* DMA Channel 9, 25 Transfer Complete */
#elif defined(DMA_INT_INT9_MASK)
    [DMA9_IRQn       ] = isr_DMA9,            /* DMA Channel 9 Transfer Complete */
#endif
#if defined(DMA_INT_INT26_MASK)
    [DMA10_DMA26_IRQn] = isr_DMA10_DMA26,     /* DMA Channel 10, 26 Transfer Complete */
#elif defined(DMA_INT_INT10_MASK)
    [DMA10_IRQn      ] = isr_DMA10,           /* DMA Channel 10 Transfer Complete */
#endif
#if defined(DMA_INT_INT27_MASK)
    [DMA11_DMA27_IRQn] = isr_DMA11_DMA27,     /* DMA Channel 11, 27 Transfer Complete */
#elif defined(DMA_INT_INT11_MASK)
    [DMA11_IRQn      ] = isr_DMA11,           /* DMA Channel 11 Transfer Complete */
#endif
#if defined(DMA_INT_INT28_MASK)
    [DMA12_DMA28_IRQn] = isr_DMA12_DMA28,     /* DMA Channel 12, 28 Transfer Complete */
#elif defined(DMA_INT_INT12_MASK)
    [DMA12_IRQn      ] = isr_DMA12,           /* DMA Channel 12 Transfer Complete */
#endif
#if defined(DMA_INT_INT29_MASK)
    [DMA13_DMA29_IRQn] = isr_DMA13_DMA29,     /* DMA Channel 13, 29 Transfer Complete */
#elif defined(DMA_INT_INT13_MASK)
    [DMA13_IRQn      ] = isr_DMA13,           /* DMA Channel 13 Transfer Complete */
#endif
#if defined(DMA_INT_INT30_MASK)
    [DMA14_DMA30_IRQn] = isr_DMA14_DMA30,     /* DMA Channel 14, 30 Transfer Complete */
#elif defined(DMA_INT_INT14_MASK)
    [DMA14_IRQn      ] = isr_DMA14,           /* DMA Channel 14 Transfer Complete */
#endif
#if defined(DMA_INT_INT31_MASK)
    [DMA15_DMA31_IRQn] = isr_DMA15_DMA31,     /* DMA Channel 15, 31 Transfer Complete */
#elif defined(DMA_INT_INT15_MASK)
    [DMA15_IRQn      ] = isr_DMA15,           /* DMA Channel 15 Transfer Complete */
#endif
    [DMA_Error_IRQn  ] = isr_DMA_Error,       /* DMA Error Interrupt */
#endif /* defined(DMA0) */
#ifdef MCM
    [MCM_IRQn        ] = isr_MCM,             /* Normal Interrupt */
#endif
#if defined(FTFA)
    [FTFA_IRQn       ] = isr_FTFA,            /* FTFA command complete */
    [FTFA_Collision_IRQn] = isr_FTFA_Collision, /* FTFA read collision */
#elif defined(FTFE)
    [FTFE_IRQn       ] = isr_FTFE,            /* FTFE command complete */
    [FTFE_Collision_IRQn] = isr_FTFE_Collision, /* FTFE read collision */
#elif defined(FTFL)
    [FTFL_IRQn       ] = isr_FTFL,            /* FTFL command complete */
    [FTFL_Collision_IRQn] = isr_FTFL_Collision, /* FTFL read collision */
#endif
#ifdef PMC
    [LVD_LVW_IRQn    ] = isr_LVD_LVW,         /* Low Voltage Detect, Low Voltage Warning */
#endif
#ifdef LLWU
    [LLWU_IRQn       ] = isr_LLWU,            /* Low Leakage Wakeup Unit */
#endif
#ifdef WDOG
    [WDOG_EWM_IRQn   ] = isr_WDOG_EWM,        /* WDOG/EWM Interrupt */
#endif
#ifdef RNG
    [RNG_IRQn        ] = isr_RNG,             /* RNG Interrupt */
#endif
#ifdef I2C0
    [I2C0_IRQn       ] = isr_I2C0,            /* I2C0 interrupt */
#endif
#ifdef I2C1
    [I2C1_IRQn       ] = isr_I2C1,            /* I2C1 interrupt */
#endif
#ifdef I2C2
    [I2C2_IRQn       ] = isr_I2C2,            /* I2C2 interrupt */
#endif
#ifdef I2C3
    [I2C3_IRQn       ] = isr_I2C3,            /* I2C3 interrupt */
#endif
#ifdef SPI0
    [SPI0_IRQn       ] = isr_SPI0,            /* SPI0 Interrupt */
#endif
#ifdef SPI1
    [SPI1_IRQn       ] = isr_SPI1,            /* SPI1 Interrupt */
#endif
#ifdef SPI2
    [SPI2_IRQn       ] = isr_SPI2,            /* SPI2 Interrupt */
#endif
#ifdef I2S0
    [I2S0_Tx_IRQn    ] = isr_I2S0_Tx,         /* I2S0 transmit interrupt */
    [I2S0_Rx_IRQn    ] = isr_I2S0_Rx,         /* I2S0 receive interrupt */
#endif
#ifdef UART0
    [UART0_RX_TX_IRQn] = isr_UART0_RX_TX,     /* UART0 Receive/Transmit interrupt */
    [UART0_ERR_IRQn  ] = isr_UART0_ERR,       /* UART0 Error interrupt */
#endif
#ifdef UART1
    [UART1_RX_TX_IRQn] = isr_UART1_RX_TX,     /* UART1 Receive/Transmit interrupt */
    [UART1_ERR_IRQn  ] = isr_UART1_ERR,       /* UART1 Error interrupt */
#endif
#ifdef UART2
    [UART2_RX_TX_IRQn] = isr_UART2_RX_TX,     /* UART2 Receive/Transmit interrupt */
    [UART2_ERR_IRQn  ] = isr_UART2_ERR,       /* UART2 Error interrupt */
#endif
#ifdef UART3
    [UART3_RX_TX_IRQn] = isr_UART3_RX_TX,     /* UART3 Receive/Transmit interrupt */
    [UART3_ERR_IRQn  ] = isr_UART3_ERR,       /* UART3 Error interrupt */
#endif
#ifdef UART4
    [UART4_RX_TX_IRQn] = isr_UART4_RX_TX,     /* UART4 Receive/Transmit interrupt */
    [UART4_ERR_IRQn  ] = isr_UART4_ERR,       /* UART4 Error interrupt */
#endif
#ifdef UART5
    [UART5_RX_TX_IRQn] = isr_UART5_RX_TX,     /* UART5 Receive/Transmit interrupt */
    [UART5_ERR_IRQn  ] = isr_UART5_ERR,       /* UART5 Error interrupt */
#endif
#ifdef ADC0
    [ADC0_IRQn       ] = isr_ADC0,            /* ADC0 interrupt */
#endif
#ifdef ADC1
    [ADC1_IRQn       ] = isr_ADC1,            /* ADC1 interrupt */
#endif
#ifdef ADC2
    [ADC2_IRQn       ] = isr_ADC2,            /* ADC2 interrupt */
#endif
#ifdef CMP0
    [CMP0_IRQn       ] = isr_CMP0,            /* CMP0 interrupt */
#endif
#ifdef CMP1
    [CMP1_IRQn       ] = isr_CMP1,            /* CMP1 interrupt */
#endif
#ifdef CMP2
    [CMP2_IRQn       ] = isr_CMP2,            /* CMP2 interrupt */
#endif
#ifdef CMP3
    [CMP3_IRQn       ] = isr_CMP3,            /* CMP3 interrupt */
#endif
#ifdef FTM0
    [FTM0_IRQn       ] = isr_FTM0,            /* FTM0 fault, overflow and channels interrupt */
#endif
#ifdef FTM1
    [FTM1_IRQn       ] = isr_FTM1,            /* FTM1 fault, overflow and channels interrupt */
#endif
#ifdef FTM2
    [FTM2_IRQn       ] = isr_FTM2,            /* FTM2 fault, overflow and channels interrupt */
#endif
#ifdef FTM3
    [FTM3_IRQn       ] = isr_FTM3,            /* FTM3 fault, overflow and channels interrupt */
#endif
#ifdef CMT
    [CMT_IRQn        ] = isr_CMT,             /* CMT interrupt */
#endif
#ifdef RTC
    [RTC_IRQn        ] = isr_RTC,             /* RTC interrupt */
    [RTC_Seconds_IRQn] = isr_RTC_Seconds,     /* RTC seconds interrupt */
#endif
#ifdef PIT
    [PIT0_IRQn       ] = isr_PIT0,            /* PIT timer channel 0 interrupt */
    [PIT1_IRQn       ] = isr_PIT1,            /* PIT timer channel 1 interrupt */
    [PIT2_IRQn       ] = isr_PIT2,            /* PIT timer channel 2 interrupt */
    [PIT3_IRQn       ] = isr_PIT3,            /* PIT timer channel 3 interrupt */
#endif
#ifdef PDB0
    [PDB0_IRQn       ] = isr_PDB0,            /* PDB0 Interrupt */
#endif
#ifdef USB0
    [USB0_IRQn       ] = isr_USB0,            /* USB0 interrupt */
#endif
#ifdef USBDCD
    [USBDCD_IRQn     ] = isr_USBDCD,          /* USBDCD Interrupt */
#endif
#if DAC0_BASE /* Not #ifdef because of error in MKW2xD.h files */
    [DAC0_IRQn       ] = isr_DAC0,            /* DAC0 interrupt */
#endif
#ifdef DAC1
    [DAC1_IRQn       ] = isr_DAC1,            /* DAC1 interrupt */
#endif
#ifdef MCG
    [MCG_IRQn        ] = isr_MCG,             /* MCG Interrupt */
#endif
#ifdef LPTMR0
    [LPTMR0_IRQn     ] = isr_LPTMR0,          /* LPTimer interrupt */
#endif
#ifdef PORTA
    [PORTA_IRQn      ] = isr_PORTA,           /* Port A interrupt */
#endif
#ifdef PORTB
    [PORTB_IRQn      ] = isr_PORTB,           /* Port B interrupt */
#endif
#ifdef PORTC
    [PORTC_IRQn      ] = isr_PORTC,           /* Port C interrupt */
#endif
#ifdef PORTD
    [PORTD_IRQn      ] = isr_PORTD,           /* Port D interrupt */
#endif
#ifdef PORTE
    [PORTE_IRQn      ] = isr_PORTE,           /* Port E interrupt */
#endif
#if __CORTEX_M >= 3
    [SWI_IRQn        ] = isr_SWI,             /* Software interrupt */
#endif
#ifdef CAN0
    [CAN0_ORed_Message_buffer_IRQn] = isr_CAN0_ORed_Message_buffer, /* CAN0 OR'd message buffers interrupt */
    [CAN0_Bus_Off_IRQn] = isr_CAN0_Bus_Off,    /* CAN0 bus off interrupt */
    [CAN0_Error_IRQn ] = isr_CAN0_Error,      /* CAN0 error interrupt */
    [CAN0_Tx_Warning_IRQn] = isr_CAN0_Tx_Warning, /* CAN0 Tx warning interrupt */
    [CAN0_Rx_Warning_IRQn] = isr_CAN0_Rx_Warning, /* CAN0 Rx warning interrupt */
    [CAN0_Wake_Up_IRQn] = isr_CAN0_Wake_Up,    /* CAN0 wake up interrupt */
#endif
#ifdef CAN1
    [CAN1_ORed_Message_buffer_IRQn] = isr_CAN1_ORed_Message_buffer, /* CAN1 OR'd message buffers interrupt */
    [CAN1_Bus_Off_IRQn] = isr_CAN1_Bus_Off,    /* CAN1 bus off interrupt */
    [CAN1_Error_IRQn ] = isr_CAN1_Error,      /* CAN1 error interrupt */
    [CAN1_Tx_Warning_IRQn] = isr_CAN1_Tx_Warning, /* CAN1 Tx warning interrupt */
    [CAN1_Rx_Warning_IRQn] = isr_CAN1_Rx_Warning, /* CAN1 Rx warning interrupt */
    [CAN1_Wake_Up_IRQn] = isr_CAN1_Wake_Up,    /* CAN1 wake up interrupt */
#endif
#ifdef SDHC
    [SDHC_IRQn       ] = isr_SDHC,            /* SDHC interrupt */
#endif
#ifdef ENET
    [ENET_1588_Timer_IRQn] = isr_ENET_1588_Timer, /* Ethernet MAC IEEE 1588 Timer Interrupt */
    [ENET_Transmit_IRQn] = isr_ENET_Transmit,   /* Ethernet MAC Transmit Interrupt */
    [ENET_Receive_IRQn] = isr_ENET_Receive,    /* Ethernet MAC Receive Interrupt */
    [ENET_Error_IRQn ] = isr_ENET_Error,      /* Ethernet MAC Error and miscelaneous Interrupt */
#endif
#ifdef LPUART0
    [LPUART0_IRQn    ] = isr_LPUART0,         /* LPUART0 status/error interrupt */
#endif
#ifdef LPUART1
    [LPUART1_IRQn    ] = isr_LPUART1,         /* LPUART1 status/error interrupt */
#endif
#ifdef LPUART2
    [LPUART2_IRQn    ] = isr_LPUART2,         /* LPUART2 status/error interrupt */
#endif
#ifdef LPUART3
    [LPUART3_IRQn    ] = isr_LPUART3,         /* LPUART3 status/error interrupt */
#endif
#ifdef LPUART4
    [LPUART4_IRQn    ] = isr_LPUART4,         /* LPUART4 status/error interrupt */
#endif
#ifdef LPUART5
    [LPUART5_IRQn    ] = isr_LPUART5,         /* LPUART5 status/error interrupt */
#endif
#ifdef TSI0
    [TSI0_IRQn       ] = isr_TSI0,            /* TSI0 interrupt */
#endif
#ifdef TPM0
    [TPM0_IRQn       ] = isr_TPM0,            /* TPM1 fault, overflow and channels interrupt */
#endif
#ifdef TPM1
    [TPM1_IRQn       ] = isr_TPM1,            /* TPM1 fault, overflow and channels interrupt */
#endif
#ifdef TPM2
    [TPM2_IRQn       ] = isr_TPM2,            /* TPM2 fault, overflow and channels interrupt */
#endif
#ifdef USBHSDCD
    [USBHSDCD_IRQn   ] = isr_USBHSDCD,        /* USBHSDCD, USBHS Phy Interrupt */
#endif
#ifdef USBHS
    [USBHS_IRQn      ] = isr_USBHS,           /* USB high speed OTG interrupt */
#endif
};

/** @} */
