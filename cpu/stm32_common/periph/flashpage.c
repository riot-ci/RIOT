/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_flashpage
 * @{
 *
 * @file
 * @brief       Low-level flash page driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#include "periph/flashpage.h"

#if defined(CPU_FAM_STM32L0)
#define CNTRL_REG              (FLASH->PECR)
#define CNTRL_REG_LOCK         (FLASH_PECR_PELOCK)
#define KEY_REG                (FLASH->PEKEYR)
#define FLASH_CR_PER           (FLASH_PECR_ERASE | FLASH_PECR_PROG)
#define FLASH_CR_PG            (FLASH_PECR_FPRG | FLASH_PECR_PROG)
#define FLASHPAGE_LIMIT        (FLASHPAGE_SIZE / 4)
#else
#define CNTRL_REG              (FLASH->CR)
#define CNTRL_REG_LOCK         (FLASH_CR_LOCK)
#define KEY_REG                (FLASH->KEYR)
#define FLASHPAGE_LIMIT        (FLASHPAGE_SIZE / 2)
#endif

void flashpage_write(int page, void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

#if defined(CPU_FAM_STM32L0)
    /* STM32L0 only supports word sizes */
    uint32_t *page_addr = (uint32_t *)flashpage_addr(page);
    uint32_t *data_addr = (uint32_t *)data;
#else
    /* Default is to support half-word sizes */
    uint16_t *page_addr = flashpage_addr(page);
    uint16_t *data_addr = (uint16_t *)data;
#endif
    uint32_t hsi_state = (RCC->CR & RCC_CR_HSION);

    /* the internal RC oscillator (HSI) must be enabled */
    RCC->CR |= (RCC_CR_HSION);
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}

    /* unlock the flash module */
    DEBUG("[flashpage] unlocking the flash module\n");
    if (CNTRL_REG & CNTRL_REG_LOCK) {
        KEY_REG = FLASH_KEY1;
        KEY_REG = FLASH_KEY2;
    }

#if defined(CPU_FAM_STM32L0)
    DEBUG("[flashpage] unlocking the flash program memory\n");
    if (!(CNTRL_REG & CNTRL_REG_LOCK)) {
        if (FLASH->PECR & FLASH_PECR_PRGLOCK) {
            DEBUG("[flashpage] setting program unlock keys\n");
            FLASH->PRGKEYR = FLASH_PRGKEY1;
            FLASH->PRGKEYR = FLASH_PRGKEY2;
        }
    }
#endif

    /* ERASE sequence */
    /* make sure no flash operation is ongoing */
    DEBUG("[flashpage] erase: waiting for any operation to finish\n");
    while (FLASH->SR & FLASH_SR_BSY) {}
    /* set page erase bit and program page address */
    DEBUG("[flashpage] erase: setting the erase bit\n");
    CNTRL_REG |= FLASH_CR_PER;
    DEBUG("address to erase: %p\n", page_addr);
#if defined(CPU_FAM_STM32L0)
    DEBUG("[flashpage] erase: trigger the page erase\n");
    *page_addr = (uint32_t)0;
#else
    DEBUG("[flashpage] erase: setting the page address\n");
    FLASH->AR = (uint32_t)page_addr;
    /* trigger the page erase and wait for it to be finished */
    DEBUG("[flashpage] erase: trigger the page erase\n");
    CNTRL_REG |= FLASH_CR_STRT;
#endif
    DEBUG("[flashpage] erase: wait as long as device is busy\n");
    while (FLASH->SR & FLASH_SR_BSY) {}
    /* reset PER bit */
    DEBUG("[flashpage] erase: resetting the page erase bit\n");
    CNTRL_REG &= ~(FLASH_CR_PER);

    /* WRITE sequence */
    if (data != NULL) {
        DEBUG("[flashpage] write: now writing the data\n");
#if !defined(CPU_FAM_STM32L0)
        /* set PG bit and program page to flash */
        CNTRL_REG |= FLASH_CR_PG;
#endif
        for (unsigned i = 0; i < FLASHPAGE_LIMIT; i++) {
            *page_addr++ = data_addr[i];
        }
        while (FLASH->SR & FLASH_SR_BSY) {}
        /* clear program bit again */
        CNTRL_REG &= ~(FLASH_CR_PG);
        DEBUG("[flashpage] write: done writing data\n");
    }

    /* finally, lock the flash module again */
    DEBUG("[flashpage] now locking the flash module again\n");
    CNTRL_REG |= CNTRL_REG_LOCK;

    /* restore the HSI state */
    if (!hsi_state) {
        RCC->CR &= ~(RCC_CR_HSION);
        while (RCC->CR & RCC_CR_HSIRDY) {}
    }
}
