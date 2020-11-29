/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @{
 *
 * @file
 * @brief       Implementation specific CPU configuration options
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#include "vendor/QN908XC.h"
#include "vendor/QN908XC_features.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO    (1U)
/**
 * NUMBER_OF_INT_VECTORS in the QN908XC.h is defined as including the standard
 * ARM interrupt vectors and headers, however CPU_IRQ_NUMOF does not include
 * the first 15 interrupt values and the stack pointer.
 */
#define CPU_IRQ_NUMOF           (NUMBER_OF_INT_VECTORS - 16)
/**
 * The flash is aliased at several addresses in the memory range. In particular,
 * address 0 can be mapped to RAM or flash, so it is possible to run from
 * address 0 from flash, or even turn off the flash altogether and run from RAM
 * to save power. This setting uses the ROM_START_ADDR value set in the
 * Makefile.
 */
#define CPU_FLASH_BASE          (QN908X_ROM_START_ADDR)
/** @} */

/**
 * @name   Code Read Protection
 * @{
 * @brief Image "Code Read Protection" field definitions.
 *
 * The Code Read Protection (CRP) is a 32-bit field stored in one of the
 * reserved fields in the Cortex-M interrupt vector and therefore part of the
 * image. It allows to enable or disable access to the flash from the In-System
 * Programming (ISP) interface to read, erase or write flash pages, as well as
 * external SWD access for debugging or programming the flash. Not all the CRP
 * values are valid and an invalid value may render the flash inaccessible and
 * effectively brick the device.
 *
 * To select the access level define the @ref QN908X_CRP macro from the global
 * compile options, otherwise the default value in this module will be used
 * (allowing everything). The value of the uint32_t CRP field in the Image
 * vector table should be the "or" of the following QN908X_CRP_* macros. Every
 * field must be either enabled or disabled, otherwise it would result in an
 * invalid CRP value.
 */

/**
 * @brief Number of pages to protect (0 to 255).
 *
 * This defines the number of pages to protect starting from 0. A value of 0
 * in this macro means that no page is protected. The maximum number allowed to
 * be passed to this macro is 255, however there are 256 pages in the flash. The
 * last page is protected if any other page is protected.
 *
 * Protected pages can't be erased or written to by the ISP.
 */
#define QN908X_CRP_PROTECT_PAGES(X) (255 - (X))

/**
 * @brief Mass erase from ISP allowed.
 */
#define QN908X_CRP_MASS_ERASE_ALLOW     (0x800)
/**
 * @brief Mass erase from ISP not allowed.
 */
#define QN908X_CRP_MASS_ERASE_DISALLOW  (0x400)

/**
 * @brief Page erase/write from ISP (for unprotected pages) allowed.
 */
#define QN908X_CRP_PAGE_ERASE_WRITE_ALLOW     (0x2000)
/**
 * @brief Page erase/write from ISP (for unprotected pages) not allowed.
 */
#define QN908X_CRP_PAGE_ERASE_WRITE_DISALLOW  (0x1000)

/**
 * @brief Flash read (for unprotected pages) from ISP allowed or not.
 */
#define QN908X_CRP_FLASH_READ_ALLOW     (0x8000)
/**
 * @brief Flash read (for unprotected pages) from ISP not allowed.
 */
#define QN908X_CRP_FLASH_READ_DISALLOW  (0x4000)

/**
 * @brief ISP entry is allowed (via CHIP_MODE pin).
 */
#define QN908X_CRP_ISP_ENTRY_ALLOW     (0x20000)
/**
 * @brief ISP entry via CHIP_MODE pin is not allowed.
 */
#define QN908X_CRP_ISP_ENTRY_DISALLOW  (0x10000)

/**
 * @brief External access is allowed (including SWD interface).
 */
#define QN908X_CRP_EXTERNAL_ACCESS_ALLOW     (0x80000)
/**
 * @brief External access is not allowed (including SWD interface).
 */
#define QN908X_CRP_EXTERNAL_ACCESS_DISALLOW  (0x40000)

/** @} */

/**
 * @brief   Default "Code Read Protection" allows everything.
 */
#ifndef QN908X_CRP
#define QN908X_CRP \
    (QN908X_CRP_PROTECT_PAGES(0) \
     | QN908X_CRP_MASS_ERASE_ALLOW \
     | QN908X_CRP_PAGE_ERASE_WRITE_ALLOW \
     | QN908X_CRP_FLASH_READ_ALLOW \
     | QN908X_CRP_ISP_ENTRY_ALLOW \
     | QN908X_CRP_EXTERNAL_ACCESS_ALLOW)
#endif  /* QN908X_CRP */

/**
 * @brief   The "Code Read Protection" is stored at the offset 0x20.
 *
 * To modify the CRP field define the macro @ref QN908X_CRP.
 */
#define CORTEXM_VECTOR_RESERVED_0X20 QN908X_CRP

/* Safety checks that the QN908X_CRP value is valid. */
#if !(QN908X_CRP & QN908X_CRP_MASS_ERASE_ALLOW) == \
    !(QN908X_CRP & QN908X_CRP_MASS_ERASE_DISALLOW)
#error "Must select exactly one of QN908X_CRP_MASS_ERASE_* in the QN908X_CRP"
#endif
#if !(QN908X_CRP & QN908X_CRP_PAGE_ERASE_WRITE_ALLOW) == \
    !(QN908X_CRP & QN908X_CRP_PAGE_ERASE_WRITE_DISALLOW)
#error \
    "Must select exactly one of QN908X_CRP_PAGE_ERASE_WRITE_* in the QN908X_CRP"
#endif
#if !(QN908X_CRP & QN908X_CRP_FLASH_READ_ALLOW) == \
    !(QN908X_CRP & QN908X_CRP_FLASH_READ_DISALLOW)
#error "Must select exactly one of QN908X_CRP_FLASH_READ_* in the QN908X_CRP"
#endif
#if !(QN908X_CRP & QN908X_CRP_ISP_ENTRY_ALLOW) == \
    !(QN908X_CRP & QN908X_CRP_ISP_ENTRY_DISALLOW)
#error "Must select exactly one of QN908X_CRP_ISP_ENTRY_* in the QN908X_CRP"
#endif
#if !(QN908X_CRP & QN908X_CRP_EXTERNAL_ACCESS_ALLOW) == \
    !(QN908X_CRP & QN908X_CRP_EXTERNAL_ACCESS_DISALLOW)
#error \
    "Must select exactly one of QN908X_CRP_EXTERNAL_ACCESS_* in the QN908X_CRP"
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
