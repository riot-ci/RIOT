// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* PLEASE NOTE: This file is a collection of required functions from
   different files in ESP-IDF */

#define ENABLE_DEBUG  0
#include "debug.h"
#include "common.h"

#include <stdarg.h>
#include <string.h>

#include "esp_attr.h"
#include "sdkconfig.h"

#include "driver/periph_ctrl.h"
#include "esp32/esp_spiram.h"
#include "freertos/FreeRTOS.h"
#include "heap/esp_heap_caps_init.h"
#include "log/esp_log.h"
#include "rom/rtc.h"
#include "rom/cache.h"
#include "rom/efuse.h"
#include "rom/uart.h"
#include "soc/cpu.h"
#include "soc/efuse_reg.h"
#include "soc/gpio_reg.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "xtensa/core-macros.h"
#include "xtensa/xtensa_api.h"

#include "syscalls.h"

/* This function is not part on newlib API, it is defined in libc/stdio/local.h
 * There is no nice way to get __cleanup member populated while avoiding __sinit,
 * so extern declaration is used here.
 */
extern void _cleanup_r(struct _reent* r);

/**
 * This is the replacement for newlib's _REENT_INIT_PTR and __sinit.
 * The problem with __sinit is that it allocates three FILE structures
 * (stdin, stdout, stderr). Having individual standard streams for each task
 * is a bit too much on a small embedded system. So we point streams
 * to the streams of the global struct _reent, which are initialized in
 * startup code.
 */
void IRAM_ATTR esp_reent_init(struct _reent* r)
{
    memset(r, 0, sizeof(*r));
    r->_stdout = _GLOBAL_REENT->_stdout;
    r->_stderr = _GLOBAL_REENT->_stderr;
    r->_stdin  = _GLOBAL_REENT->_stdin;
    r->__cleanup = &_cleanup_r;
    r->__sdidinit = 1;
    r->__sglue._next = NULL;
    r->__sglue._niobs = 0;
    r->__sglue._iobs = NULL;
    r->_current_locale = "C";
}

/* source: /path/to/esp-idf/components/esp32/esp_panic.c */
void IRAM_ATTR esp_panic_wdt_stop (void)
{
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    WRITE_PERI_REG(RTC_CNTL_WDTFEED_REG, 1);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG0, RTC_WDT_STG_SEL_OFF);
    REG_CLR_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN);
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, 0);
}

/* source: /path/to/esp-idf/components/esp32/esp_panic.c */
void _esp_error_check_failed(esp_err_t rc, const char *file, int line,
                             const char *function, const char *expression)
{
    ets_printf("ESP_ERROR_CHECK failed: esp_err_t 0x%x at 0x%08x\n",
               rc, (intptr_t)__builtin_return_address(0) - 3);
    #if 0 /* TODO */
    if (spi_flash_cache_enabled()) { /* strings may be in flash cache */
        ets_printf("file: \"%s\" line %d\nfunc: %s\nexpression: %s\n",
                   file, line, function, expression);
    }
    invoke_abort();
    #endif
    exit(1);

    while (1) {}
}

/* source: /path/to/esp-idf/component/log/log.c */
uint32_t IRAM_ATTR esp_log_timestamp(void)
{
    return system_get_time() / USEC_PER_MSEC;
}

/* source: /path/to/esp-idf/component/log/log.c */
void IRAM_ATTR esp_log_write(esp_log_level_t level,
                             const char* tag, const char* format, ...)
{
    (void)level;

    va_list list;
    va_start(list, format);
    ets_printf(format, list);
    va_end(list);
}

static bool _spi_ram_initialized = false;

/* source: /path/to/esp-idf/component/esp32/cpu_start.c */
void spi_ram_init(void)
{
    _spi_ram_initialized = false;

    #if CONFIG_SPIRAM_SUPPORT
    esp_spiram_init_cache();
    if (esp_spiram_init() == ESP_OK) {
        _spi_ram_initialized = true;
    }
    else {
        ets_printf("Failed to init external SPI RAM\n");
        _spi_ram_initialized = false;
    }
    #else
    ets_printf("External SPI RAM functions not enabled\n");
    #endif
}

/* source: /path/to/esp-idf/component/esp32/cpu_start.c */
void spi_ram_heap_init(void)
{
    #if CONFIG_SPIRAM_SUPPORT

    #if CONFIG_SPIRAM_MEMTEST
    if (!esp_spiram_test()) {
        return;
    }
    #endif /* CONFIG_SPIRAM_MEMTEST */

    #if CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC
    esp_err_t r=esp_spiram_add_to_heapalloc();
    if (r != ESP_OK) {
        ets_printf("External SPI RAM could not be added to heap!\n");
        abort();
    }

    #if CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL
    r=esp_spiram_reserve_dma_pool(CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL);
    if (r != ESP_OK) {
        ets_printf("Could not reserve internal/DMA pool!\n");
        abort();
    }
    #endif /* CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL */

    #if CONFIG_SPIRAM_USE_MALLOC
    heap_caps_malloc_extmem_enable(CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL);
    #endif /* CONFIG_SPIRAM_USE_MALLOC */

    #endif /* CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC */

    #else  /* CONFIG_SPIRAM_SUPPORT */
    ets_printf("External SPI RAM functions not enabled\n");
    #endif /* CONFIG_SPIRAM_SUPPORT */
}

/*
 * If CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST is enabled. Prefer to
 * allocate a chunk of memory in SPIRAM firstly. If failed, try to allocate
 * it in internal memory then.
 *
 * source: /path/to/esp-idf/component/esp32/wifi_internal.c
 */
IRAM_ATTR void *wifi_malloc( size_t size )
{
#if CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST
    return heap_caps_malloc_prefer(size, 2,
                                   MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM,
                                   MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return malloc(size);
#endif
}

/*
 * If CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST is enabled. Prefer to
 * allocate a chunk of memory in SPIRAM firstly. If failed, try to allocate
 * it in internal memory then.
 *
 * source: /path/to/esp-idf/component/esp32/wifi_internal.c
 */
IRAM_ATTR void *wifi_calloc( size_t n, size_t size )
{
#if CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST
    return heap_caps_calloc_prefer(n, size, 2,
                                   MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM,
                                   MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return calloc(n, size);
#endif
}

static const char* TAG = "system_api";

/*
 * source: /path/to/esp-idf/component/esp32/system_api.c
 */
esp_err_t esp_efuse_mac_get_default(uint8_t* mac)
{
    uint32_t mac_low;
    uint32_t mac_high;
    uint8_t efuse_crc;
    uint8_t calc_crc;

    mac_low = REG_READ(EFUSE_BLK0_RDATA1_REG);
    mac_high = REG_READ(EFUSE_BLK0_RDATA2_REG);

    mac[0] = mac_high >> 8;
    mac[1] = mac_high;
    mac[2] = mac_low >> 24;
    mac[3] = mac_low >> 16;
    mac[4] = mac_low >> 8;
    mac[5] = mac_low;

    efuse_crc = mac_high >> 16;

    calc_crc = esp_crc8(mac, 6);

    if (efuse_crc != calc_crc) {
         // Small range of MAC addresses are accepted even if CRC is invalid.
         // These addresses are reserved for Espressif internal use.
        if ((mac_high & 0xFFFF) == 0x18fe) {
            if ((mac_low >= 0x346a85c7) && (mac_low <= 0x346a85f8)) {
                return ESP_OK;
            }
        } else {
            ESP_LOGE(TAG, "Base MAC address from BLK0 of EFUSE CRC error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
            abort();
        }
    }
    return ESP_OK;
}

/*
 * source: /path/to/esp-idf/component/esp32/system_api.c
 */
/* "inner" restart function for after RTOS, interrupts & anything else on this
 * core are already stopped. Stalls other core, resets hardware,
 * triggers restart.
*/
void IRAM_ATTR esp_restart_noos(void)
{
    // Disable interrupts
    xt_ints_off(0xFFFFFFFF);

    // Enable RTC watchdog for 1 second
    REG_WRITE(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    REG_WRITE(RTC_CNTL_WDTCONFIG0_REG,
            RTC_CNTL_WDT_FLASHBOOT_MOD_EN_M |
            (RTC_WDT_STG_SEL_RESET_SYSTEM << RTC_CNTL_WDT_STG0_S) |
            (RTC_WDT_STG_SEL_RESET_RTC << RTC_CNTL_WDT_STG1_S) |
            (1 << RTC_CNTL_WDT_SYS_RESET_LENGTH_S) |
            (1 << RTC_CNTL_WDT_CPU_RESET_LENGTH_S) );
    REG_WRITE(RTC_CNTL_WDTCONFIG1_REG, rtc_clk_slow_freq_get_hz() * 1);

    // Reset and stall the other CPU.
    // CPU must be reset before stalling, in case it was running a s32c1i
    // instruction. This would cause memory pool to be locked by arbiter
    // to the stalled CPU, preventing current CPU from accessing this pool.
    const uint32_t core_id = 0;
    const uint32_t other_core_id = (core_id == 0) ? 1 : 0;
    esp_cpu_reset(other_core_id);
    esp_cpu_stall(other_core_id);

    // Other core is now stalled, can access DPORT registers directly
    esp_dport_access_int_abort();

    // Disable TG0/TG1 watchdogs
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_config0.en = 0;
    TIMERG0.wdt_wprotect=0;
    TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG1.wdt_config0.en = 0;
    TIMERG1.wdt_wprotect=0;

    // Flush any data left in UART FIFOs
    uart_tx_wait_idle(0);
    uart_tx_wait_idle(1);
    uart_tx_wait_idle(2);

    // Disable cache
    Cache_Read_Disable(0);
    Cache_Read_Disable(1);

    // 2nd stage bootloader reconfigures SPI flash signals.
    // Reset them to the defaults expected by ROM.
    WRITE_PERI_REG(GPIO_FUNC0_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC1_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC2_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC3_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC4_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC5_IN_SEL_CFG_REG, 0x30);

    // Reset wifi/bluetooth/ethernet/sdio (bb/mac)
    DPORT_SET_PERI_REG_MASK(DPORT_CORE_RST_EN_REG, 
         DPORT_BB_RST | DPORT_FE_RST | DPORT_MAC_RST |
         DPORT_BT_RST | DPORT_BTMAC_RST | DPORT_SDIO_RST |
         DPORT_SDIO_HOST_RST | DPORT_EMAC_RST | DPORT_MACPWR_RST | 
         DPORT_RW_BTMAC_RST | DPORT_RW_BTLP_RST);
    DPORT_REG_WRITE(DPORT_CORE_RST_EN_REG, 0);

    // Reset timer/spi/uart
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,
            DPORT_TIMERS_RST | DPORT_SPI_RST_1 | DPORT_UART_RST);
    DPORT_REG_WRITE(DPORT_PERIP_RST_EN_REG, 0);

    // Set CPU back to XTAL source, no PLL, same as hard reset
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_XTAL);

    // Clear entry point for APP CPU
    DPORT_REG_WRITE(DPORT_APPCPU_CTRL_D_REG, 0);

    // Reset CPUs
    if (core_id == 0) {
        // Running on PRO CPU: APP CPU is stalled. Can reset both CPUs.
        esp_cpu_reset(1);
        esp_cpu_reset(0);
    } else {
        // Running on APP CPU: need to reset PRO CPU and unstall it,
        // then reset APP CPU
        esp_cpu_reset(0);
        esp_cpu_unstall(0);
        esp_cpu_reset(1);
    }
    while(true) {
        ;
    }
}
