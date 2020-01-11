/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_ztimer ztimer high level timer abstraction layer
 * @ingroup     sys
 * @brief       High level timer abstraction layer
 *
 * # Introduction
 *
 * ztimer provides a high level abstraction of hardware timers for application
 * timing needs.
 *
 * The basic functions of the ztimer module are ztimer_now(), ztimer_sleep(),
 * ztimer_set() and ztimer_remove().
 *
 * They all take a pointer to a clock device (or virtual timer device) as first
 * parameter. RIOT provides ZTIMER_USEC, ZTIMER_MSEC, ZTIMER_SEC by default.
 * These clocks allow multiple timeouts to be scheduled. They all provide 32bit
 * range.
 *
 * ztimer_now() returns the current clock tick count as uint32_t.
 *
 * ztimer_sleep() pauses the current thread for the passed amount of clock
 * ticks. E.g., ```ztimer_sleep(ZTIMER_SEC, 5);``` will suspend the currently
 * running thread for five seconds.
 *
 * ztimer_set() takes a ztimer_t object (containing a function pointer and
 * void* argument) and an interval as arguments. After at least the interval
 * (in number of ticks for the corresponding clock) has passed, the callback
 * will be called in interrupt context.
 * A timer can be cancelled using ztimer_remove().
 *
 * Example:
 *
 * ```
 * #include "ztimer.h"
 *
 * static void callback(void *arg)
 * {
 *    puts(arg);
 * }
 *
 * int main()
 * {
 *     ztimer_t timeout = { .callback=callback, .arg="Hello ztimer!" };
 *     ztimer_set(ZTIMER_SEC, &timeout, 2);
 *
 *     ztimer_sleep(ZTIMER_SEC, 5);
 * }
 *
 *
 * # Design
 *
 * ## clocks, virtual timers, chaining
 *
 * The system is composed of clocks (virtual ztimer devices) which can be
 * chained to create an abstract view of a hardware timer/counter device. Each
 * ztimer clock acts as a filter on the next clock in the chain. At the end of
 * each ztimer chain there is always some kind of counter device object.
 *
 * Each clock device handles multiplexing (allowing multiple timers to be set)
 * and extension to full 32bit.
 *
 * Hardware interface submodules:
 *
 * - @ref ztimer_rtt_init "ztimer_rtt" interface for periph_rtt
 * - @ref ztimer_rtc_init "ztimer_rtc" interface for periph_rtc
 * - @ref ztimer_periph_init "ztimer_periph" interface for periph_timer
 *
 * Filter submodules:
 *
 * - @ref ztimer_convert_frac_init "ztimer_convert_frac" for fast frequency
 *   conversion using the frac library
 * - @ref ztimer_convert_muldiv64_init "ztimer_convert_muldiv64" for accurate
 *   but slow frequency conversion using 64bit division
 *
 *
 * A common chain could be:
 *
 * 1. ztimer_periph (e.g., on top of an 1024Hz 16bit hardware timer)
 * 2. ztimer_convert_frac (to convert 1024 to 1000Hz)
 *
 * This is how e.g., the clock ZTIMER_MSEC might be configured on a specific
 * system.
 *
 * Every clock in the chain can always be used on its own. E.g. in the example
 * above, the ztimer_periph object can be used as ztimer clock with 1024Hz
 * ticks in addition to the ztimer_convert_frac with 1000Hz.
 *
 *
 * ## Timer handling
 *
 * Timers in ztimer are stored in a linked list for which each entry stores the
 * difference to the previous entry in the timer (T[n]). The list also stores
 * the absolute time on which the relative offsets are based (B), effectively
 * storing the absolute target time for each entry (as B + sum(T[0-n])).
 * Storing the entries in this way allows all entries to use the full width of
 * the used uint32_t, compared to storing the absolute time.
 *
 * In order to prevent timer processing offset to add up, whenever a timer
 * triggers, the list's absolute base time is set to the *expected* trigger
 * time (B + T[0]). The underlying clock is then set to alarm at (now() +
 * (now() - B) + T[1]). Thus even though the list is keeping relative offsets,
 * the time keeping is done by keeping track of the absolute times.
 *
 *
 * ## Clock extension
 *
 * The API always allows setting full 32bit relative offsets for every clock.
 *
 * In some cases (e.g., a hardware timer only allowing getting/setting smaller
 * values or a conversion which would overflow uint32_t for large intervals),
 * ztimer takes care of extending timers.
 * This is enabled automatically for every ztimer clock that has a "max_value"
 * setting smaller than 2**32-1. If a ztimer_set() would overflow that value,
 * intermediate intervals of length (max_value / 2) are set until the remaining
 * interval fits into max_value.
 * If extension is enabled for a clock, ztimer_now() uses interval
 * checkpointing, storing the current time and corresponding clock tick value on
 * each call and using that information to calculate the current time.
 * This ensures correct ztimer_now() values if ztimer_now() is called at least
 * once every "max_value" ticks. This is ensured by scheduling intermediate
 * callbacks every (max_value / 2) ticks (even if no timeout is configured).
 *
 *
 * ## Reliability
 *
 * Care has been taken to avoid any unexpected behaviour of ztimer. In
 * particular, ztimer tries hard to avoid underflows (setting a backend timer
 * to a value at or behind the current time, causing the timer interrupt to
 * trigger one whole timer period too late).
 * This is done by always setting relative timeouts to backend timers, with
 * interrupts disabled and ensuring that very small values don't cause
 * underflows.
 *
 *
 * ## Configuration and convention
 *
 * As timer hardware and capabilities is diverse and ztimer allows configuring
 * and using arbitrary clock backends and conversions, it is envisioned to
 * provide default configurations that application developers can assume to be
 * available.
 *
 * These are implemented by using pointers to ztimer clocks using default names.
 *
 * For now, there are:
 *
 * ZTIMER_USEC: clock providing microsecond ticks
 *
 * ZTIMER_MSEC: clock providing millisecond ticks, using a low power timer if
 *              available on the platform
 *
 * ZTIMER_SEC:  clock providing second time, possibly using epoch semantics
 *
 * These pointers are defined in `ztimer.h` and can be used like this:
 *
 *     ztimer_now(ZTIMER_USEC);
 *
 *
 * ## Differences to xtimer
 *
 * - the addition of a "clock" parameter makes it possible to work with
 *   multiple differently clocked timers and backends
 *
 * - the API is 32bit only
 *
 * - much of ztimer can be unittested on a mock clock
 *
 * - the internal timer list uses relative timers and thus doesn't need 64bit
 *   arithmetic or storage
 *
 * - ztimer_extend extends <32bit timers in an ISR safe way
 *
 * - has much reduced number of configuration tunables (no XTIMER_BACKOFF,
 *   XTIMER_ISR_BACKOFF)
 *
 * - ztimer always executes callbacks in ISR context, whereas xtimer might
 *   trigger very short timers in ISR context
 *
 * @{
 *
 * @file
 * @brief       ztimer API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef ZTIMER_H
#define ZTIMER_H

#include <stdint.h>

#include "kernel_types.h"
#include "msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ztimer_base_t forward declaration
 */
typedef struct ztimer_base ztimer_base_t;

/**
 * @brief ztimer_clock_t forward declaration
 */
typedef struct ztimer_clock ztimer_clock_t;

/**
 * @brief   Minimum information for each alarm
 */
struct ztimer_base {
    ztimer_base_t *next;        /**< next alarm in list */
    uint32_t offset;            /**< offset from last alarm in list */
};

/**
 * @brief   ztimer structure
 *
 * This type represents an instance of an alarm, which is set on an
 * underlying clock object
 */
typedef struct {
    ztimer_base_t base;             /**< clock list entry */
    void (*callback)(void *arg);    /**< alarm callback function pointer */
    void *arg;                      /**< alarm callback argument */
} ztimer_t;

/**
 * @brief   ztimer backend method structure
 *
 * This table contains pointers to the virtual methods for a ztimer clock.
 *
 * These functions used by ztimer core to interact with the underlying clock.
 */
typedef struct {
    /**
     * @brief   Set a new timer target
     */
    void (*set)(ztimer_clock_t *clock, uint32_t val);

    /**
     * @brief   Get the current count of the timer
     */
    uint32_t (*now)(ztimer_clock_t *clock);

    /**
     * @brief   Cancel any set target
     */
    void (*cancel)(ztimer_clock_t *clock);
} ztimer_ops_t;

/**
 * @brief   ztimer device structure
 */
struct ztimer_clock {
    ztimer_base_t list;             /**< list of active timers              */
    const ztimer_ops_t *ops;        /**< pointer to methods structure       */
    ztimer_base_t *last;            /**< last timer in queue, for _is_set() */
    uint32_t adjust;                /**< will be subtracted on every set()  */
#if MODULE_ZTIMER_EXTEND || DOXYGEN
    /* values used for checkpointed intervals and 32bit extension */
    uint32_t max_value;             /**< maximum relative timer value       */
    uint32_t checkpoint;   /**< cumulated time at last now() call  */
    uint32_t lower_last;   /**< timer value at last now() call     */
#endif
};

/**
 * @brief   main ztimer callback handler
 */
void ztimer_handler(ztimer_clock_t *clock);

/* User API */
/**
 * @brief   Set an alarm on a clock
 *
 * This will place @p entry in the alarm targets queue for @p ztimer.
 *
 * @note The memory pointed to by @p entry is not copied and must
 *       remain in scope until the callback is fired or the alarm
 *       is removed via @ref ztimer_remove
 *
 * @param[in]   ztimer      ztimer clock to operate on
 * @param[in]   entry       alarm entry to enqueue
 * @param[in]   val         alarm target
 */
void ztimer_set(ztimer_clock_t *clock, ztimer_t *entry, uint32_t val);

/**
 * @brief   Remove an alarm from a clock
 *
 * This will place @p entry in the timer targets queue for @p ztimer.
 *
 * This function does nothing if @p entry is not found in the alarm queue of @p ztimer
 *
 * @param[in]   ztimer      ztimer clock to operate on
 * @param[in]   entry       alarm entry to enqueue
 * @param[in]   val         alarm target
 */
void ztimer_remove(ztimer_clock_t *clock, ztimer_t *entry);

/**
 * @brief   Post a message after a delay
 *
 * This function sets an alarm that will send a message @p offset ticks
 * from now.
 *
 * @note The memory pointed at by @p alarm and @p msg will not be copied, i.e.
 *       `*alarm` and `*msg` needs to remain valid until the alarm has occurred.
 *
 * @param[in]   ztimer          ztimer clock to operate on
 * @param[in]   alarm           pointer to alarm struct
 * @param[in]   offset          ticks from now
 * @param[in]   msg             pointer to msg that will be sent
 * @param[in]   target_pid      pid the message will be sent to
 */
void ztimer_set_msg(ztimer_clock_t *clock, ztimer_t *timer, uint32_t offset,
                    msg_t *msg, kernel_pid_t target_pid);

/**
 * @brief receive a message (blocking, with timeout)
 *
 * Similar to msg_receive(), but with a timeout parameter.
 * The function will return after waiting at most @p timeout ticks.
 *
 * @param[in]   ztimer          ztimer clock to operate on
 * @param[out]  msg             pointer to buffer which will be filled if a message is received
 * @param[in]   timeout         relative timeout, in @p ztimer time units
 *
 * @return  >=0 if a message was received
 * @return  -ETIME on timeout
 */
int ztimer_msg_receive_timeout(ztimer_clock_t *clock, msg_t *msg,
                               uint32_t timeout);

/*
 * @brief ztimer_now() for extending timers
 *
 * @internal
 *
 * @param[in]   ztimer          ztimer clock to operate on
 * @return  Current count on the clock @p ztimer
 */
uint32_t _ztimer_now_extend(ztimer_clock_t *ztimer);

/**
 * @brief   Get the current time from a clock
 *
 * @param[in]   ztimer          ztimer clock to operate on
 *
 * @return  Current count on the clock @p ztimer
 */
static inline uint32_t ztimer_now(ztimer_clock_t *ztimer)
{
#ifdef MODULE_ZTIMER_EXTEND
    if (ztimer->max_value < 0xffffffff) {
        return _ztimer_now_extend(ztimer);
#else
    if (0) {
#endif
    }
    else {
        return ztimer->ops->now(ztimer);
    }
}

/**
 * @brief Suspend the calling thread until the time (@p last_wakeup + @p period)
 *
 * This function can be used to create periodic wakeups.
 *
 * When the function returns, @p last_wakeup is set to
 * (@p last_wakeup + @p period).
 *
 * @c last_wakeup should be set to ztimer_now(@p ztimer) before first call of the
 * function.
 *
 * If the time (@p last_wakeup + @p period) has already passed, the function
 * sets @p last_wakeup to @p last_wakeup + @p period and returns immediately.
 *
 * @param[in]   ztimer          ztimer clock to operate on
 * @param[in]   last_wakeup     base time stamp for the wakeup
 * @param[in]   period          time in ticks that will be added to @p last_wakeup
 */
void ztimer_periodic_wakeup(ztimer_clock_t *ztimer, uint32_t *last_wakeup,
                            uint32_t period);

/**
 * @brief   Put the calling thread to sleep for the specified number of ticks
 *
 * @param[in]   ztimer          ztimer clock to use
 * @param[in]   duration        duration of sleep, in @p ztimer time units
 */
void ztimer_sleep(ztimer_clock_t *ztimer, uint32_t duration);

/**
 * @brief Set a timer that wakes up a thread
 *
 * This function sets a timer that will wake up a thread when the timer has
 * expired.
 *
 * @param[in] clock         ztimer clock to operate on
 * @param[in] timer         timer struct to work with.
 * @param[in] offset        clock ticks from now
 * @param[in] pid           pid of the thread that will be woken up
 */
void ztimer_set_wakeup(ztimer_clock_t *clock, ztimer_t *timer, uint32_t offset,
                       kernel_pid_t pid);

/**
 * @brief    Set timeout thread flag after @p timeout
 *
 * This function will set THREAD_FLAG_TIMEOUT on the current thread after @p
 * timeout usec have passed.
 *
 * @param[in]   ztimer_clock    ztimer clock to operate on
 * @param[in]   t               timer struct to use
 * @param[in]   timeout         timeout in ztimer_clock's ticks
 */
void ztimer_set_timeout_flag(ztimer_clock_t *ztimer_clock, ztimer_t *t,
                             uint32_t timeout);

/**
 * @brief   Measure ztimer overhead
 *
 * This function can be used to measure the overhead incurred by ztimer.
 * It will configure a callback to trigger after @p base ticks, then return the
 * number of ticks that have passed, minus @p base.
 *
 * @param[in]   base    base interval to use
 * @return  (time from ztimer_set() until callback) - base
 */
uint32_t ztimer_overhead(ztimer_clock_t *ztimer, uint32_t base);

/**
 * @brief   Update ztimer clock head list offset
 *
 * @internal
 *
 * @param[in]   ztimer  ztimer clock to work on
 */
void ztimer_update_head_offset(ztimer_clock_t *ztimer);

/**
 * @brief   Initialize the board-specific default ztimer configuration
 */
void ztimer_init(void);

/* default ztimer virtual devices */
/**
 * @brief   Default ztimer microsecond clock
 */
extern ztimer_clock_t *const ZTIMER_USEC;

/**
 * @brief   Default ztimer millisecond clock
 */
extern ztimer_clock_t *const ZTIMER_MSEC;

#ifdef __cplusplus
extern "C" {
#endif

#endif /* ZTIMER_H */
/** @} */
