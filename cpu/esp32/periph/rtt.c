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
#include "syscalls.h"
#include "timex.h"

#define RTC_CLK_CAL_FRACT       19  /* fractional bits of calibration value */

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

/* we can't include soc/rtc.h because of rtc_init declaration conflicts */
extern uint32_t rtc_clk_slow_freq_get_hz(void);

/* forward declaration of functions */
void rtt_restore_counter(bool sys_time);
static void IRAM_ATTR _rtt_isr(void *arg);

/* forward declarations of driver functions */
uint64_t _rtc_get_counter(void);

/* declaration of hardware counters */
extern const rtt_hw_driver_t _rtt_hw_sys_driver;
extern const rtt_hw_driver_t _rtt_hw_rtc_driver;

/*
 * Used hardware driver (set in init depending on whether module
 * `esp_rtc_timer_32k` is enabled and the external crystal is connected)
 */
static const rtt_hw_driver_t *_rtt_hw;

void rtt_init(void)
{
    /* microsecond system timer is used by default */
    _rtt_hw = &_rtt_hw_sys_driver;

    if (IS_USED(MODULE_ESP_RTC_TIMER_32K)) {
        /* check whether the 32.678 kHz crystal is working */
        if (rtc_clk_slow_freq_get_hz() == 32768) {
            _rtt_hw = &_rtt_hw_rtc_driver;
        }
        else {
            LOG_ERROR("[rtt] 32.768 kHz crystal not used!\n");
        }
    }

    DEBUG("%s rtt_offset=%u @rtc=%llu rtc_active=%d @sys_time=%llu\n", __func__,
          _rtt_offset, _rtc_get_counter(),
          (_rtt_hw == &_rtt_hw_sys_driver) ? 1 : 0, system_get_time_64());

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
    /* route all interrupt sources to the same RTT level type interrupt */
    intr_matrix_set(PRO_CPU_NUM, _rtt_hw->irq_src, CPU_INUM_RTC);

    /* set interrupt handler and enable the CPU interrupt */
    xt_set_interrupt_handler(CPU_INUM_RTC, _rtt_isr, NULL);
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
    return _rtt_hw->get_counter() + _rtt_offset;
}

void rtt_set_counter(uint32_t counter)
{
    uint32_t rtc_time = _rtt_hw->get_counter();
    _rtt_offset = counter - rtc_time;

    DEBUG("%s counter=%u offset=%u @rtc=%u\n",
          __func__, counter, _rtt_offset, rtc_time);

    if (counter < rtt_counter.alarm) {
        /* alarm is the next interrupt event */
        _rtt_hw->set_alarm(rtt_counter.alarm);
    }
    else {
        /* set overflow as next interrupt event */
        _rtt_hw->set_alarm(0);
    }
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    DEBUG("%s alarm=%u @rtc=%u\n",
          __func__, alarm, (uint32_t)_rtt_hw->get_counter());

    rtt_counter.alarm = alarm;
    rtt_counter.alarm_cb = cb;
    rtt_counter.alarm_arg = arg;

    if (rtt_get_counter() < alarm) {
        /* alarm is the next interrupt event */
        _rtt_hw->set_alarm(alarm);
    }
    else {
        /* set overflow as next interrupt event */
        _rtt_hw->set_alarm(0);
    }
}

void rtt_clear_alarm(void)
{
    DEBUG("%s @rtc=%u\n", __func__, (uint32_t)_rtt_hw->get_counter());

    /* clear the alarm */
    rtt_counter.alarm = 0;
    rtt_counter.alarm_cb = NULL;
    rtt_counter.alarm_arg = NULL;

    /* set overflow as next interrupt event */
    _rtt_hw->set_alarm(0);
}

uint32_t rtt_get_alarm(void)
{
    return rtt_counter.alarm;
}

void rtt_save_counter(void)
{
    _rtt_hw->save_counter();
}

void rtt_restore_counter(bool in_init)
{
    _rtt_hw->restore_counter(in_init);
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
        _rtt_isr(NULL);
    }
}

/*
 * ISR for the RTT
 */
static void IRAM_ATTR _rtt_isr(void *arg)
{
    DEBUG("%s\n", __func__);

    uint32_t alarm;

    if (_rtt_alarm_wakeup) {
        _rtt_alarm_wakeup = false;
        alarm = _rtt_alarm_saved;
        DEBUG("%s wakeup alarm alarm=%u rtt_alarm=%u @rtt=%u\n",
              __func__, alarm, rtt_counter.alarm, rtt_get_counter());
    }
    else if (_rtt_hw->isr(&alarm) != 0) {
        /* no alarm to be handled found */
        return;
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
        _rtt_hw->set_alarm(rtt_counter.alarm);
        /* call the overflow handler if set */
        if (rtt_counter.overflow_cb) {
            rtt_counter.overflow_cb(rtt_counter.overflow_arg);
        }
    }
}

uint32_t _rtt_hw_to_rtt_counter(uint32_t hw_counter)
{
    return hw_counter + _rtt_offset;
}
