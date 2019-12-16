/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Implementation of required system calls
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "esp/common_macros.h"
#include "irq_arch.h"
#include "periph_cpu.h"
#include "syscalls.h"
#include "timex.h"

#include "rom/ets_sys.h"
#include "rom/libc_stubs.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "xtensa/xtensa_api.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define MHZ 1000000UL

/**
 * @name Other system functions
 */

void syscalls_init_arch(void)
{
    /* enable the system timer in us (TMG0 is enabled by default) */
    TIMER_SYSTEM.config.divider = rtc_clk_apb_freq_get()/MHZ;
    TIMER_SYSTEM.config.autoreload = 0;
    TIMER_SYSTEM.config.enable = 1;
}

uint32_t system_get_time(void)
{
    /* latch 64 bit timer value before read */
    TIMER_SYSTEM.update = 0;
    /* wait until instructions have been finished */
    __asm__ volatile ("isync");
    return TIMER_SYSTEM.cnt_low;
}

uint64_t system_get_time_64(void)
{
    uint64_t  ret;
    /* latch 64 bit timer value before read */
    TIMER_SYSTEM.update = 0;
    /* wait until instructions have been finished */
    __asm__ volatile ("isync");
    /* read the current timer value */
    ret  = TIMER_SYSTEM.cnt_low;
    ret += ((uint64_t)TIMER_SYSTEM.cnt_high) << 32;
    return ret;
}

/* alias for compatibility with espressif/wifi_libs */
int64_t esp_timer_get_time(void) __attribute__((alias("system_get_time_64")));

static IRAM void system_wdt_int_handler(void *arg)
{
    TIMERG0.int_clr_timers.wdt=1; /* clear interrupt */
    system_wdt_feed();
}

void IRAM system_wdt_feed(void)
{
    DEBUG("%s\n", __func__);
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_feed=1;                        /* reset MWDT */
    TIMERG0.wdt_wprotect=0;                    /* enable write protection */
}

void system_wdt_init(void)
{
    /* disable boot watchdogs */
    TIMERG0.wdt_config0.flashboot_mod_en = 0;
    RTCCNTL.wdt_config0.flashboot_mod_en = 0;

    /* enable system watchdog */
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_config0.stg0 = TIMG_WDT_STG_SEL_INT;          /* stage0 timeout: interrupt */
    TIMERG0.wdt_config0.stg1 = TIMG_WDT_STG_SEL_RESET_SYSTEM; /* stage1 timeout: sys reset */
    TIMERG0.wdt_config0.sys_reset_length = 7;  /* sys reset signal length: 3.2 us */
    TIMERG0.wdt_config0.cpu_reset_length = 7;  /* sys reset signal length: 3.2 us */
    TIMERG0.wdt_config0.edge_int_en = 0;
    TIMERG0.wdt_config0.level_int_en = 1;

    /* MWDT clock = 80 * 12,5 ns = 1 us */
    TIMERG0.wdt_config1.clk_prescale = 80;

    /* define stage timeouts */
    TIMERG0.wdt_config2 = 2 * US_PER_SEC;  /* stage 0: 2 s (interrupt) */
    TIMERG0.wdt_config3 = 4 * US_PER_SEC;  /* stage 1: 4 s (sys reset) */

    TIMERG0.wdt_config0.en = 1;   /* enable MWDT */
    TIMERG0.wdt_feed = 1;         /* reset MWDT */
    TIMERG0.wdt_wprotect = 0;     /* enable write protection */

    DEBUG("%s TIMERG0 wdt_config0=%08x wdt_config1=%08x wdt_config2=%08x\n",
          __func__, TIMERG0.wdt_config0.val, TIMERG0.wdt_config1.val,
          TIMERG0.wdt_config2);

    /* route WDT peripheral interrupt source to CPU_INUM_WDT */
    intr_matrix_set(PRO_CPU_NUM, ETS_TG0_WDT_LEVEL_INTR_SOURCE, CPU_INUM_WDT);
    /* set the interrupt handler and activate the interrupt */
    xt_set_interrupt_handler(CPU_INUM_WDT, system_wdt_int_handler, NULL);
    xt_ints_on(BIT(CPU_INUM_WDT));
}

void system_wdt_stop(void)
{
    xt_ints_off(BIT(CPU_INUM_WDT));
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_config0.en = 0;   /* disable MWDT */
    TIMERG0.wdt_feed = 1;         /* reset MWDT */
    TIMERG0.wdt_wprotect = 0;     /* enable write protection */
}

void system_wdt_start(void)
{
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;  /* disable write protection */
    TIMERG0.wdt_config0.en = 1;   /* disable MWDT */
    TIMERG0.wdt_feed = 1;         /* reset MWDT */
    TIMERG0.wdt_wprotect = 0;     /* enable write protection */
    xt_ints_on(BIT(CPU_INUM_WDT));
}
