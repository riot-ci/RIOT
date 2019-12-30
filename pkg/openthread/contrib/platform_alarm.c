/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread alarm platform abstraction
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Hyung-Sin Kim <hs.kim@berkeley.edu>
 * @}
 */

#include <stdint.h>

#include "msg.h"
#include "openthread/platform/alarm-milli.h"
#include "openthread/platform/alarm-micro.h"
#include "ot.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static xtimer_t ot_millitimer;
static xtimer_t ot_microtimer;
static msg_t ot_millitimer_msg;
static msg_t ot_microtimer_msg;

void ot_timer_init(void)
{
    ot_millitimer_msg.type = OPENTHREAD_MILLITIMER_MSG_TYPE_EVENT;
    ot_microtimer_msg.type = OPENTHREAD_MICROTIMER_MSG_TYPE_EVENT;
}

/**
 * Set the alarm to fire at @p aDt milliseconds after @p aT0.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 * @param[in] aT0        The reference time.
 * @param[in] aDt        The time delay in milliseconds from @p aT0.
 */
void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    (void)aInstance;
    (void)aT0;
    DEBUG("openthread: otPlatAlarmMilliStartAt: aT0: %" PRIu32 ", aDT: %" PRIu32 "\n", aT0, aDt);

    uint32_t dt = aDt * US_PER_MS;
    xtimer_set_msg(&ot_millitimer, dt, &ot_millitimer_msg, openthread_get_pid());
}

/* OpenThread will call this to stop alarms */
void otPlatAlarmMilliStop(otInstance *aInstance)
{
    (void)aInstance;
    DEBUG("openthread: otPlatAlarmMilliStop\n");
    xtimer_remove(&ot_millitimer);
}

/* OpenThread will call this for getting running time in millisecs */
uint32_t otPlatAlarmMilliGetNow(void)
{
    uint32_t now = (uint32_t)(xtimer_now_usec64() / US_PER_MS);
    DEBUG("openthread: otPlatAlarmMilliGetNow: %" PRIu32 "\n", now);
    return now;
}

/**
 * Set the alarm to fire at @p aDt microseconds after @p aT0.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 * @param[in] aT0        The reference time.
 * @param[in] aDt        The time delay in microseconds from @p aT0.
 */
void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    (void)aInstance;
    (void)aT0;
    DEBUG("openthread: otPlatAlarmMicroStartAt: aT0: %" PRIu32 ", aDT: %" PRIu32 "\n", aT0, aDt);

    xtimer_set_msg(&ot_microtimer, aDt, &ot_microtimer_msg, openthread_get_pid());
}

/* OpenThread will call this to stop alarms */
void otPlatAlarmMicroStop(otInstance *aInstance)
{
    (void)aInstance;
    DEBUG("openthread: otPlatAlarmMicroStop\n");
    xtimer_remove(&ot_microtimer);
}

/* OpenThread will call this for getting running time in microsecs */
uint32_t otPlatAlarmMicroGetNow(void)
{
    uint32_t now = xtimer_now_usec();
    DEBUG("openthread: otPlatAlarmMicroGetNow: %" PRIu32 "\n", now);
    return now;
}
