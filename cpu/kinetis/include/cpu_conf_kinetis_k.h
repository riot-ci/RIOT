/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_kinetis
 * @brief           CPU specific implementations for the NXP Kinetis K series of
 *                  Cortex-M MCUs
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef CPU_CONF_KINETIS_K_H
#define CPU_CONF_KINETIS_K_H

#if (KINETIS_FAMILY == 2)
#if (KINETIS_SUBFAMILY == 2)
/* Kinetis K22 */
#if defined(CPU_MODEL_MK22FX512VLH12) || \
    defined(CPU_MODEL_MK22FN1M0VLH12) || \
    defined(CPU_MODEL_MK22FX512VLK12) || \
    defined(CPU_MODEL_MK22FN1M0VLK12) || \
    defined(CPU_MODEL_MK22FX512VLL12) || \
    defined(CPU_MODEL_MK22FN1M0VLL12) || \
    defined(CPU_MODEL_MK22FX512VLQ12) || \
    defined(CPU_MODEL_MK22FN1M0VLQ12) || \
    defined(CPU_MODEL_MK22FX512VMC12) || \
    defined(CPU_MODEL_MK22FN1M0VMC12) || \
    defined(CPU_MODEL_MK22FX512VMD12) || \
    defined(CPU_MODEL_MK22FN1M0VMD12)
#include "MK22F12.h"
#elif defined(CPU_MODEL_MK22FN128VDC10) || \
    defined(CPU_MODEL_MK22FN128VLH10) || \
    defined(CPU_MODEL_MK22FN128VLL10) || \
    defined(CPU_MODEL_MK22FN128VMP10)
#include "MK22F12810.h"
#elif defined(CPU_MODEL_MK22FN128CAH12) || \
    defined(CPU_MODEL_MK22FN256CAH12) || \
    defined(CPU_MODEL_MK22FN256VDC12) || \
    defined(CPU_MODEL_MK22FN256VLH12) || \
    defined(CPU_MODEL_MK22FN256VLL12) || \
    defined(CPU_MODEL_MK22FN256VMP12)
#include "MK22F25612.h"
#elif defined(CPU_MODEL_MK22FN512CAP12) || \
    defined(CPU_MODEL_MK22FN512VDC12) || \
    defined(CPU_MODEL_MK22FN512VFX12) || \
    defined(CPU_MODEL_MK22FN512VLH12) || \
    defined(CPU_MODEL_MK22FN512VLL12) || \
    defined(CPU_MODEL_MK22FN512VMP12)
#include "MK22F51212.h"
#elif defined(CPU_MODEL_MK22FN1M0AVLH12) || \
    defined(CPU_MODEL_MK22FN1M0AVLK12) || \
    defined(CPU_MODEL_MK22FN1M0AVLL12) || \
    defined(CPU_MODEL_MK22FN1M0AVLQ12) || \
    defined(CPU_MODEL_MK22FN1M0AVMC12) || \
    defined(CPU_MODEL_MK22FN1M0AVMD12) || \
    defined(CPU_MODEL_MK22FX512AVLH12) || \
    defined(CPU_MODEL_MK22FX512AVLK12) || \
    defined(CPU_MODEL_MK22FX512AVLL12) || \
    defined(CPU_MODEL_MK22FX512AVLQ12) || \
    defined(CPU_MODEL_MK22FX512AVMC12) || \
    defined(CPU_MODEL_MK22FX512AVMD12)
#include "MK22FA12.h"
#endif
#endif /* (KINETIS_SUBFAMILY == y) */
#elif (KINETIS_FAMILY == 6)
#if (KINETIS_SUBFAMILY == 0)
#if defined(CPU_MODEL_MK60DN256VLL10) || \
    defined(CPU_MODEL_MK60DN256VLQ10) || \
    defined(CPU_MODEL_MK60DN256VMC10) || \
    defined(CPU_MODEL_MK60DN256VMD10) || \
    defined(CPU_MODEL_MK60DN512VLL10) || \
    defined(CPU_MODEL_MK60DN512VLQ10) || \
    defined(CPU_MODEL_MK60DN512VMC10) || \
    defined(CPU_MODEL_MK60DN512VMD10) || \
    defined(CPU_MODEL_MK60DX256VLL10) || \
    defined(CPU_MODEL_MK60DX256VLQ10) || \
    defined(CPU_MODEL_MK60DX256VMC10) || \
    defined(CPU_MODEL_MK60DX256VMD10)
#include "MK60D10.h"
#endif
/**
 * @brief This CPU provides an additional ADC clock divider as CFG1[ADICLK]=1
 */
#define KINETIS_HAVE_ADICLK_BUS_DIV_2 1

#elif (KINETIS_SUBFAMILY == 4)
#if defined(CPU_MODEL_MK64FN1M0CAJ12) || \
    defined(CPU_MODEL_MK64FN1M0VDC12) || \
    defined(CPU_MODEL_MK64FN1M0VLL12) || \
    defined(CPU_MODEL_MK64FN1M0VLQ12) || \
    defined(CPU_MODEL_MK64FN1M0VMD12) || \
    defined(CPU_MODEL_MK64FX512VDC12) || \
    defined(CPU_MODEL_MK64FX512VLL12) || \
    defined(CPU_MODEL_MK64FX512VLQ12) || \
    defined(CPU_MODEL_MK64FX512VMD12)
#include "MK64F12.h"
#endif
#endif /* (KINETIS_SUBFAMILY == y) */
#endif /* (KINETIS_FAMILY == x) */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_KINETIS_K_H */
/** @} */
