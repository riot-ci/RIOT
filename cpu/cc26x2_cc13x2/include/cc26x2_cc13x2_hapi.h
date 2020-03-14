/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2_definitions
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 ROM Hardware API definitions
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26X2_CC13X2_HAPI_H
#define CC26X2_CC13X2_HAPI_H

#include <cc26xx_cc13xx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ROM Hard-API function interface types
 * @{
 */
typedef uint32_t (* hapi_crc32_t)(uint8_t *, uint32_t, uint32_t);
typedef uint32_t (* hapi_getflsize_t)(void);
typedef uint32_t (* hapi_getchipid_t)(void);
typedef uint32_t (* hapi_reserved1_t)(uint32_t);
typedef uint32_t (* hapi_reserved2_t)(void);
typedef uint32_t (* hapi_reserved3_t)(uint8_t *, uint32_t, uint32_t);
typedef void (* hapi_resetdev_t)(void);
typedef uint32_t (* hapi_fletcher32_t)(uint16_t *, uint16_t, uint16_t);
typedef uint32_t (* hapi_minval_t)(uint32_t *, uint32_t);
typedef uint32_t (* hapi_maxval_t)(uint32_t *, uint32_t);
typedef uint32_t (* hapi_meanval_t)(uint32_t *, uint32_t);
typedef uint32_t (* hapi_stddval_t)(uint32_t *, uint32_t);
typedef void (* hapi_hfsourcesafeswitch_t)(void);
typedef void (* hapi_reserved4_t)(uint32_t);
typedef void (* hapi_reserved5_t)(uint32_t);
typedef void (* hapi_compain_t)(uint8_t);
typedef void (* hapi_comparef_t)(uint8_t);
typedef void (* hapi_adccompbin_t)(uint8_t);
typedef void (* hapi_dacvref_t)(uint8_t);
/** @} */

/**
 * @brief   ROM HAPI
 */
typedef struct {
    hapi_crc32_t Crc32;
    hapi_getflsize_t FlashGetSize;
    hapi_getchipid_t GetChipId;
    hapi_reserved1_t ReservedLocation1;
    hapi_reserved2_t ReservedLocation2;
    hapi_reserved3_t ReservedLocation3;
    hapi_resetdev_t ResetDevice;
    hapi_fletcher32_t Fletcher32;
    hapi_minval_t MinValue;
    hapi_maxval_t MaxValue;
    hapi_meanval_t MeanValue;
    hapi_stddval_t StandDeviationValue;
    hapi_reserved4_t ReservedLocation4;
    hapi_reserved5_t ReservedLocation5;
    hapi_hfsourcesafeswitch_t HFSourceSafeSwitch;
    hapi_compain_t SelectCompAInput;
    hapi_comparef_t SelectCompARef;
    hapi_adccompbin_t SelectADCCompBInput;
    hapi_dacvref_t SelectDACVref;
} hard_api_t;

#define ROM_HAPI_TABLE_BASE (0x10000048) /**< Base address por ROM hardware API */

#define ROM_HAPI ((hard_api_t *) (ROM_HAPI_TABLE_BASE)) /**< ROM_HAPI functions */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_HAPI_H */

/*@}*/
