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
#include "soc/dport_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/timer_group_struct.h"
#include "syscalls.h"
#include "timex.h"

#define TIMER_SYSTEM_GROUP      TIMERG0
#define TIMER_SYSTEM_INT_MASK   BIT(0)
#define TIMER_SYSTEM_INT_SRC    ETS_TG0_T0_LEVEL_INTR_SOURCE

#define RTC_TIME_MAX            ((1ULL << 48) - 1) /* 48 bit counter */
#define RTC_CLK_CAL_FRACT       19  /* fractional bits of calibration value */

#define RTC_BSS_ATTR __attribute__((section(".rtc.bss")))

/* contains the values as given at the interface */
typedef struct {
    uint32_t alarm;         /**< alarm */
    rtt_cb_t alarm_cb;      /**< alarm callback */
    rtt_cb_t overflow_cb;   /**< overflow callback */
    void *alarm_arg;        /**< argument for alarm callback */
    void *overflow_arg;     /**< argument for overflow callback */
} rtt_counter_t;

static rtt_counter_t rtt_counter;

static uint32_t RTC_BSS_ATTR _rtt_offset;

/* variables used to save counters during sleep or reboot */
static uint64_t RTC_BSS_ATTR _rtc_time_saved;
static uint64_t RTC_BSS_ATTR _sys_time_saved;
static uint8_t  RTC_BSS_ATTR _rtc_time_used;

/* the offset of the system time to the RTC time in microseconds */
static uint64_t _sys_time_offset;
static uint32_t _sys_time_alarm;

static uint64_t _rtc_time_to_us(uint64_t raw);

/* we can't include soc/rtc.h because of rtc_init declaration conflicts */
extern uint64_t rtc_time_get(void);
extern uint32_t rtc_clk_slow_freq_get_hz(void);
extern uint32_t esp_clk_slowclk_cal_get(void);

static void _rtt_hal_set_alarm(uint32_t alarm);
static uint64_t _rtt_hal_get_counter(void);
static uint64_t _rtc_get_time(void);
static void IRAM_ATTR _rtt_cb(void *arg);

void rtt_restore_counter(bool sys_time);

void rtt_init(void)
{
    if (IS_USED(MODULE_ESP_RTC_TIMER_32K)) {
        /* test whether the 32.678 kHz crystal is working */
        _rtc_time_used = (rtc_clk_slow_freq_get_hz() == 32768) ? 1 : 0;
        if (!_rtc_time_used) {
            LOG_ERROR("[rtt] 32.768 kHz crystal not used!\n");
        }
    }
    else {
        _rtc_time_used = 0;
    }

    DEBUG("%s rtt_offset=%u @rtc=%llu rtc_active=%d @sys_time=%llu\n", __func__,
          _rtt_offset, _rtc_get_time(), _rtc_time_used, system_get_time_64());

    /* restore counter from RTC after deep sleep or reboot */
    rtt_restore_counter(true);

    /* clear alarm including setting overflow as next interrupt event */
    rtt_clear_alarm();
    rtt_clear_overflow_cb();

    /* enable interrupts */
    rtt_poweron();
}

void rtt_poweron(void)
{
    if (_rtc_time_used) {
        /* route all RTC interrupt sources to the same level type interrupt */
        intr_matrix_set(PRO_CPU_NUM, ETS_RTC_CORE_INTR_SOURCE, CPU_INUM_RTC);
    }
    else {
        /* route all timer interrupt sources to the same level type interrupt */
        intr_matrix_set(PRO_CPU_NUM, TIMER_SYSTEM_INT_SRC, CPU_INUM_RTC);
    }

    /* set interrupt handler and enable the CPU interrupt */
    xt_set_interrupt_handler(CPU_INUM_RTC, _rtt_cb, NULL);
    xt_ints_on(BIT(CPU_INUM_RTC));
}

void rtt_poweroff(void)
{
    /* reset interrupt handler and disable the CPU interrupt */
    xt_ints_off(BIT(CPU_INUM_RTC));
    xt_set_interrupt_handler(CPU_INUM_RTC, NULL, NULL);
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    /* there is no overflow interrupt, we emulate */
    rtt_counter.overflow_cb = cb;
    rtt_counter.overflow_arg = arg;
}

void rtt_clear_overflow_cb(void)
{
    /* there is no overflow interrupt, we emulate */
    rtt_counter.overflow_cb = NULL;
    rtt_counter.overflow_arg = NULL;
}

uint32_t rtt_get_counter(void)
{
    /* we use only the lower 32 bit of the 48-bit RTC counter */
    return _rtt_hal_get_counter() + _rtt_offset;
}

void rtt_set_counter(uint32_t counter)
{
    uint32_t rtc_time = _rtt_hal_get_counter();
    _rtt_offset = counter - rtc_time;

    DEBUG("%s counter=%u offset=%u @rtc=%u\n",
          __func__, counter, _rtt_offset, rtc_time);

    if (counter < rtt_counter.alarm) {
        /* alarm is the next interrupt event */
        _rtt_hal_set_alarm(rtt_counter.alarm);
    }
    else {
        /* set overflow as next interrupt event */
        _rtt_hal_set_alarm(0);
    }
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    DEBUG("%s alarm=%u @rtc=%u\n",
          __func__, alarm, (uint32_t)_rtt_hal_get_counter());

    rtt_counter.alarm = alarm;
    rtt_counter.alarm_cb = cb;
    rtt_counter.alarm_arg = arg;

    if (rtt_get_counter() < alarm) {
        /* alarm is the next interrupt event */
        _rtt_hal_set_alarm(alarm);
    }
    else {
        /* set overflow as next interrupt event */
        _rtt_hal_set_alarm(0);
    }
}

void rtt_clear_alarm(void)
{
    DEBUG("%s @rtc=%u\n", __func__, (uint32_t)_rtt_hal_get_counter());

    /* clear the alarm */
    rtt_counter.alarm = 0;
    rtt_counter.alarm_cb = NULL;
    rtt_counter.alarm_arg = NULL;

    /* set overflow as next interrupt event */
    _rtt_hal_set_alarm(0);
}

uint32_t rtt_get_alarm(void)
{
    return rtt_counter.alarm;
}

void rtt_save_counter(void)
{
    critical_enter();

    /* save counters for synchronization after wakeup or reboot */
    _rtc_time_saved = _rtc_get_time();
    _sys_time_saved = system_get_time_64() + _sys_time_offset;

    critical_exit();

    DEBUG("%s rtc_time_saved=%llu sys_time_saved=%llu\n", __func__,
          _rtc_time_saved, _sys_time_saved);
}

void rtt_restore_counter(bool sys_time)
{
    critical_enter();

    /* synchronize RTC counter and the 64-bit microsecond system timer */
    uint64_t _rtc_time_diff = (_rtc_get_time() - _rtc_time_saved) & RTC_TIME_MAX;
    _sys_time_offset += _rtc_time_to_us(_rtc_time_diff);
    _sys_time_offset += (sys_time) ? _sys_time_saved : 0;

    critical_exit();

    DEBUG("%s rtc_time_saved=%llu rtc_time_diff=%llu "
          "sys_time_saved=%llu sys_time_offset=%llu\n", __func__,
          _rtc_time_saved, _rtc_time_diff, _sys_time_saved, _sys_time_offset);
}

uint32_t _rtt_alarm_saved;
bool _rtt_alarm_wakeup = false;

uint64_t rtt_pm_sleep_enter(unsigned mode)
{
    rtt_save_counter();

    _rtt_alarm_saved = rtt_get_alarm();
    uint64_t t_diff = RTT_TICKS_TO_US(_rtt_alarm_saved - rtt_get_counter());

    if (t_diff) {
        _rtt_alarm_wakeup = true;
        esp_sleep_enable_timer_wakeup(t_diff);
    }
    else {
        _rtt_alarm_wakeup = false;
    }

    DEBUG("%s t_diff=%llu\n", __func__, t_diff);
    return t_diff;
}

void rtt_pm_sleep_exit(uint32_t cause)
{
    rtt_restore_counter(false);

    if (cause == ESP_SLEEP_WAKEUP_TIMER) {
        _rtt_cb(NULL);
    }
}

static uint64_t _rtc_time_to_us(uint64_t raw)
{
    const uint32_t cal = esp_clk_slowclk_cal_get();
    return ((((raw >> 32) * cal) << (32 - RTC_CLK_CAL_FRACT)) + /* high part */
            (((raw & 0xffffffff) * cal) >> RTC_CLK_CAL_FRACT)); /* low part */
}

static uint64_t _rtc_get_time(void)
{
    /* trigger timer register update */
    RTCCNTL.time_update.update = 1;
    /* wait until values in registers are valid */
    while (!RTCCNTL.time_update.valid) {
        ets_delay_us(1);
    }
    /* read the time from 48-bit counter and return */
    return (((uint64_t)RTCCNTL.time1.val) << 32) + RTCCNTL.time0;
}

static uint64_t _rtt_hal_get_counter(void)
{
    /*
     * Regardless of whether the 48-bit RTC counter with 32.678 kHz crystal
     * or the 64-bit microsecond system time is used, the function always
     * returns a 48-bit value with a frequency of 32.678 kHz.
     */
    if (_rtc_time_used) {
        return _rtc_get_time();
    }
    else {
        /* convert the 64-bit microsecond system time to 48-bit 32.768 kHz time */
        return (((system_get_time_64() + _sys_time_offset) << 15) / US_PER_SEC) & RTC_TIME_MAX;
    }
}

static void _rtt_hal_set_alarm(uint32_t alarm)
{
    /* compute the time difference for 32.768 kHz as 32-bit value */
    uint64_t rtc_time = _rtt_hal_get_counter();
    uint32_t rtt_counter = rtc_time + _rtt_offset;
    uint32_t rtt_diff = alarm - rtt_counter;

    if (_rtc_time_used) {
        /* use computed time difference directly to set the RTC counter alarm */
        uint64_t rtc_alarm = (rtc_time + rtt_diff) & RTC_TIME_MAX;

        DEBUG("%s alarm=%u rtt_diff=%u rtc_alarm=%llu @rtc=%llu\n",
              __func__, alarm, rtt_diff, rtc_alarm, rtc_time);

        /* set the timer value */
        RTCCNTL.slp_timer0 = rtc_alarm & 0xffffffff;
        RTCCNTL.slp_timer1.slp_val_hi = rtc_alarm >> 32;

        DEBUG("%s %08x%08x \n", __func__,
              RTCCNTL.slp_timer1.slp_val_hi, RTCCNTL.slp_timer0);

        /* enable RTC timer alarm */
        RTCCNTL.slp_timer1.main_timer_alarm_en = 1;

        /* clear and enable RTC timer interrupt */
        RTCCNTL.int_clr.rtc_main_timer = 1;
        RTCCNTL.int_ena.rtc_main_timer = 1;
    }
    else {
        /*
         * convert the computed time difference for 32.768 kHz to a 64-bit
         * microsecond value and determine the alarm time for the 64-bit
         * microsecond system timer
         */
        uint64_t _sys_diff = ((uint64_t)rtt_diff * US_PER_SEC) >> 15;
        uint64_t _sys_time = system_get_time_64();
        uint64_t _sys_alarm = _sys_time + _sys_diff;

        DEBUG("%s alarm=%u rtt_diff=%u @rtc=%llu "
              "sys_diff=%llu sys_alarm=%llu @sys_time=%llu\n", __func__,
              alarm, rtt_diff, rtc_time, _sys_diff, _sys_alarm, _sys_time);

        /* save the alarm time for alarm interrupt handling */
        _sys_time_alarm = alarm;

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
}

static void IRAM_ATTR _rtt_cb(void *arg)
{
    DEBUG("%s\n", __func__);

    uint32_t alarm;

    if (_rtt_alarm_wakeup) {
        _rtt_alarm_wakeup = false;
        alarm = _rtt_alarm_saved;
        DEBUG("%s wakeup alarm alarm=%u rtt_alarm=%u @rtt=%u\n",
              __func__, alarm, rtt_counter.alarm, rtt_get_counter());
    }
    else if (_rtc_time_used) {
        /* lower 32 bit of triggered alarm */
        alarm = RTCCNTL.slp_timer0 + _rtt_offset;
        DEBUG("%s alarm=%u rtt_alarm=%u @rtt=%u\n",
              __func__, alarm, rtt_counter.alarm, rtt_get_counter());
    }
    else {
        if (!(TIMER_SYSTEM_GROUP.int_st_timers.val & TIMER_SYSTEM_INT_MASK)) {
            return;
        }

        /* disable alarms */
        TIMER_SYSTEM.config.level_int_en = 0;
        TIMER_SYSTEM.config.alarm_en = 0;

        /* clear the bit in interrupt enable and status register */
        TIMER_SYSTEM_GROUP.int_ena.val &= ~TIMER_SYSTEM_INT_MASK;
        TIMER_SYSTEM_GROUP.int_clr_timers.val |= TIMER_SYSTEM_INT_MASK;

        alarm = _sys_time_alarm;

        DEBUG("%s alarm=%u rtt_alarm=%u @rtt=%u @sys_time=%llu\n",
              __func__, alarm, rtt_counter.alarm, rtt_get_counter(),
              system_get_time_64());
    }

    if (alarm == rtt_counter.alarm) {
        DEBUG("%s alarm\n", __func__);
        rtt_cb_t alarm_cb = rtt_counter.alarm_cb;
        void * alarm_arg = rtt_counter.alarm_arg;
        /* clear the alarm first, includes setting next alarm to overflow */
        rtt_clear_alarm();
        /* call the alarm handler afterwards if a callback is set */
        if (alarm_cb) {
            alarm_cb(alarm_arg);
        }
    }

    if (alarm == 0) {
        DEBUG("%s overflow\n", __func__);
        /* set next alarm which is either an alarm if configured or overflow */
        _rtt_hal_set_alarm(rtt_counter.alarm);
        /* call the overflow handler if set */
        if (rtt_counter.overflow_cb) {
            rtt_counter.overflow_cb(rtt_counter.overflow_arg);
        }
    }
}
