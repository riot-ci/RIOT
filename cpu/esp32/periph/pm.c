/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of power management functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "esp_attr.h"
#include "esp_sleep.h"
#include "syscalls.h"
#include "xtimer.h"

#include "periph/rtc.h"
#include "rom/rtc.h"
#include "rom/uart.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"

static inline void pm_set_lowest_normal(void)
{
    DEBUG ("%s enter to normal sleep @%u\n", __func__, system_get_time());

    #if !defined(QEMU)
    /* passive wait for interrupt to leave lowest power mode */
    __asm__ volatile ("waiti 0");

    /* reset system watchdog timer */
    system_wdt_feed();
    #endif

    DEBUG ("%s exit from normal sleep @%u\n", __func__, system_get_time());
}

void IRAM_ATTR pm_off(void)
{
    /* enter hibernate mode without any enabled wake-up sources */
    esp_deep_sleep_start();
}

extern void esp_restart_noos(void) __attribute__ ((noreturn));

void pm_reboot(void)
{
    DEBUG ("%s\n", __func__);

    /* suspend and flush UARTs */
    for (int i = 0; i < 3; ++i) {
        REG_SET_BIT(UART_FLOW_CONF_REG(i), UART_FORCE_XOFF);
        uart_tx_wait_idle(i);
    }

    software_reset();
}

#ifndef MODULE_PM_LAYERED

void pm_set_lowest(void)
{
    pm_set_lowest_normal();
}

#else /* MODULE_PM_LAYERED */

void pm_set(unsigned mode)
{
    if (mode == ESP_PM_MODEM_SLEEP) {
        pm_set_lowest_normal();
        return;
    }

    DEBUG ("%s enter to power mode %d @%u\n", __func__, mode, system_get_time());

    /* Labels for RTC slow memory that are defined in the linker script */
    extern int _rtc_bss_rtc_start;
    extern int _rtc_bss_rtc_end;

    /*
     * Activate the Power Domain for slow RTC memory when the .rtc.bss
     * section is used to retain uninitialized data. The Power Domain for
     * slow RTC memory is automatically activated when the .rtc.data section
     * is used to retain initialized data.
     */
    if (&_rtc_bss_rtc_end > &_rtc_bss_rtc_start) {
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
    }

    /*
     * Activate the power domain for RTC peripherals either when
     * ESP_PM_GPIO_HOLD is defined or when light sleep mode is activated.
     * As long as the RTC peripherals are active, the pad state of RTC GPIOs
     * is held in deep sleep and the pad state of all GPIOs is held in light
     * sleep.
     */
#ifdef ESP_PM_GPIO_HOLD
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
#else
    if (mode == ESP_PM_LIGHT_SLEEP) {
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    }
#endif

    /* Prepare the RTC timer if an RTC alarm is set to wake up. */
    struct tm tm_alarm;
    struct tm tm_system;

    rtc_get_time(&tm_system);
    rtc_get_alarm(&tm_alarm);

    time_t t_system = mktime(&tm_system);
    time_t t_alarm = mktime(&tm_alarm);
    int _alarm_set = 0;

    if (t_alarm > t_system) {
        _alarm_set = 1;
        esp_sleep_enable_timer_wakeup((uint64_t)(t_alarm - t_system) * US_PER_SEC);
    }

#ifdef ESP_PM_WUP_PINS
    /*
     * Prepare the wake-up pins if a single pin or a comma-separated list of
     * pins is defined for wake-up.
     */
    static const gpio_t wup_pins[] = { ESP_PM_WUP_PINS };

    uint64_t wup_pin_mask = 0;
    for (unsigned i = 0; i < ARRAY_SIZE(wup_pins); i++) {
        wup_pin_mask |= 1ULL << wup_pins[i];
    }
#ifdef ESP_PM_WUP_LEVEL
   esp_sleep_enable_ext1_wakeup(wup_pin_mask, ESP_PM_WUP_LEVEL);
#else /* ESP_PM_WUP_LEVEL */
   esp_sleep_enable_ext1_wakeup(wup_pin_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
#endif /* ESP_PM_WUP_LEVEL */
#endif /* ESP_PM_WUP_PINS */

    if (mode == ESP_PM_DEEP_SLEEP) {
        esp_deep_sleep_start();
        /* waking up from deep-sleep leads to a DEEPSLEEP_RESET */
        UNREACHABLE();
    }
    else if (mode == ESP_PM_LIGHT_SLEEP) {
        esp_light_sleep_start();
        DEBUG ("%s exit from power mode %d @%u\n", __func__, mode,
               system_get_time());
        if (_alarm_set) {
            /* call the RTC alarm handler if an RTC alarm was set */
            extern void rtc_handle_pending_alarm(void);
            rtc_handle_pending_alarm();
            _alarm_set = 0;
        }
    }
}

#endif /* MODULE_PM_LAYERED */
