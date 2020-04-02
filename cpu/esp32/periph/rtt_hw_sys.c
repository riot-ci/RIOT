/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       Low-level RTT driver implementation for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "cpu.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "irq_arch.h"
#include "log.h"
#include "periph/rtt.h"
#include "rtt_arch.h"
#include "soc/timer_group_struct.h"
#include "syscalls.h"
#include "timex.h"

#define TIMER_SYSTEM_GROUP      TIMERG0
#define TIMER_SYSTEM_INT_MASK   BIT(0)
#define TIMER_SYSTEM_INT_SRC    ETS_TG0_T0_LEVEL_INTR_SOURCE

#define RTC_BSS_ATTR __attribute__((section(".rtc.bss")))

/* variables used to save counters during sleep or reboot */
static uint64_t RTC_BSS_ATTR _rtc_counter_saved;
static uint64_t RTC_BSS_ATTR _sys_counter_saved;

/* the offset of the system time to the RTC time in microseconds */
static uint64_t _sys_counter_offset;
static uint32_t _sys_counter_alarm;

/* forward declarations of functions required from RTC counter */
extern uint64_t _rtc_get_counter(void);
extern uint64_t _rtc_counter_to_us(uint64_t raw);

static uint64_t _sys_get_counter(void)
{
    /* convert the 64-bit microsecond system time to 48-bit 32.768 kHz time */
    return (((system_get_time_64() +
              _sys_counter_offset) << 15) / US_PER_SEC) & RTT_HW_COUNTER_MAX;
}

static void _sys_set_alarm(uint32_t alarm)
{
    /* compute the time difference for 32.768 kHz as 32-bit value */
    uint32_t rtt_diff = alarm - rtt_get_counter();

    /*
     * convert the computed time difference for 32.768 kHz to a 64-bit
     * microsecond value and determine the alarm time for the 64-bit
     * microsecond system timer
     */
     uint64_t _sys_diff = ((uint64_t)rtt_diff * US_PER_SEC) >> 15;
     uint64_t _sys_time = system_get_time_64();
     uint64_t _sys_alarm = _sys_time + _sys_diff;

     DEBUG("%s alarm=%u rtt_diff=%u "
           "sys_diff=%llu sys_alarm=%llu @sys_time=%llu\n", __func__,
           alarm, rtt_diff, _sys_diff, _sys_alarm, _sys_time);

     /* save the alarm time for alarm interrupt handling */
     _sys_counter_alarm = alarm;

     /* set the timer value */
     TIMER_SYSTEM.alarm_high = (uint32_t)(_sys_alarm >> 32);
     TIMER_SYSTEM.alarm_low  = (uint32_t)(_sys_alarm & 0xffffffff);

     /* clear the bit in status and set the bit in interrupt enable */
     TIMER_SYSTEM_GROUP.int_clr_timers.val |= TIMER_SYSTEM_INT_MASK;
     TIMER_SYSTEM_GROUP.int_ena.val |= TIMER_SYSTEM_INT_MASK;

     /* enable the timer alarm */
     TIMER_SYSTEM.config.level_int_en = 1;
     TIMER_SYSTEM.config.alarm_en = 1;
}

int _sys_isr(uint32_t *alarm)
{
    if (!(TIMER_SYSTEM_GROUP.int_st_timers.val & TIMER_SYSTEM_INT_MASK)) {
        return -1;
    }

    /* disable alarms */
    TIMER_SYSTEM.config.level_int_en = 0;
    TIMER_SYSTEM.config.alarm_en = 0;

    /* clear the bit in interrupt enable and status register */
    TIMER_SYSTEM_GROUP.int_ena.val &= ~TIMER_SYSTEM_INT_MASK;
    TIMER_SYSTEM_GROUP.int_clr_timers.val |= TIMER_SYSTEM_INT_MASK;

    *alarm = _sys_counter_alarm;

    DEBUG("%s alarm=%u @rtt=%u @sys_time=%llu\n",
          __func__, *alarm, rtt_get_counter(), system_get_time_64());

    return 0;
}

static void _sys_save_counter(void)
{
    critical_enter();

    /* save counters for synchronization after wakeup or reboot */
    _rtc_counter_saved = _rtc_get_counter();
    _sys_counter_saved = system_get_time_64() + _sys_counter_offset;

    critical_exit();

    DEBUG("%s rtc_time_saved=%llu sys_time_saved=%llu\n", __func__,
          _rtc_counter_saved, _sys_counter_saved);
}

void _sys_restore_counter(bool in_init)
{
    critical_enter();

    /* synchronize RTC counter and the 64-bit microsecond system timer */
    uint64_t _rtc_time_diff = _rtc_get_counter() - _rtc_counter_saved;
    _sys_counter_offset += _rtc_counter_to_us(_rtc_time_diff & RTT_HW_COUNTER_MAX);
    _sys_counter_offset += (in_init) ? _sys_counter_saved : 0;

    critical_exit();

    DEBUG("%s rtc_time_saved=%llu rtc_time_diff=%llu "
          "sys_time_saved=%llu sys_time_offset=%llu\n", __func__,
          _rtc_counter_saved, _rtc_time_diff,
          _sys_counter_saved, _sys_counter_offset);
}

const rtt_hw_driver_t _rtt_hw_sys_driver = {
        .irq_src = TIMER_SYSTEM_INT_SRC,
        .get_counter = _sys_get_counter,
        .set_alarm = _sys_set_alarm,
        .isr = _sys_isr,
        .save_counter = _sys_save_counter,
        .restore_counter = _sys_restore_counter,
};
