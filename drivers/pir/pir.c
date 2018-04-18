/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pir
 * @{
 *
 * @file
 * @brief       Device driver implementation for the PIR motion sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include "pir.h"
#include "thread.h"
#include "msg.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**********************************************************************
 * internal API declaration
 **********************************************************************/

static int pir_activate_int(pir_t *dev);
static void pir_callback(void *dev);
static void pir_send_msg(pir_t *dev, pir_event_t event);

/**********************************************************************
 * public API implementation
 **********************************************************************/

int pir_init(pir_t *dev, const pir_params_t *params)
{
    dev->p.gpio = params->gpio;
    dev->p.active_high = params->active_high;
    dev->msg_thread_pid = KERNEL_PID_UNDEF;

    dev->active = false;
    dev->accum_active_time = 0;
    dev->start_active_time = 0;
    dev->last_read_time = xtimer_usec_from_ticks64(xtimer_now64());

    if (gpio_init_int(dev->p.gpio, GPIO_IN_PD, GPIO_BOTH, pir_callback, dev)) {
        return -1;
    }
    return 0;
}

pir_event_t pir_get_status(const pir_t *dev)
{
    return ((gpio_read(dev->p.gpio) == 0) ? PIR_STATUS_LO : PIR_STATUS_HI);
}

int pir_get_occupancy(const pir_t *dev, int16_t *occup) {
    pir_t* pir_dev = (pir_t*) dev;
    uint64_t now = xtimer_usec_from_ticks64(xtimer_now64());
    uint64_t total_time = now - pir_dev->last_read_time;
    if (total_time == 0) {
        return -1;
    }

    /* We were busy counting */
    if (pir_dev->start_active_time != 0xFFFFFFFFFFFFFFFF && pir_dev->active) {
        pir_dev->accum_active_time += (now - pir_dev->start_active_time);
        pir_dev->start_active_time = now;
    }
    *occup = (int16_t)((pir_dev->accum_active_time * 10000) / total_time);
    pir_dev->last_read_time = now;
    pir_dev->accum_active_time = 0;
    return 0;
}

int pir_register_thread(pir_t *dev)
{
    if (dev->msg_thread_pid != KERNEL_PID_UNDEF) {
        if (dev->msg_thread_pid != thread_getpid()) {
            DEBUG("pir_register_thread: already registered to another thread\n");
            return -2;
        }
    }
    else {
        DEBUG("pir_register_thread: activating interrupt for %p..\n", (void *)dev);
        if (pir_activate_int(dev) != 0) {
            DEBUG("\tfailed\n");
            return -1;
        }
        DEBUG("\tsuccess\n");
    }
    dev->msg_thread_pid = thread_getpid();

    return 0;
}

/**********************************************************************
 * internal API implementation
 **********************************************************************/

static void pir_send_msg(pir_t *dev, pir_event_t event)
{
    DEBUG("pir_send_msg\n");
    msg_t m = { .type = event, .content.ptr = dev, };

    int ret = msg_send_int(&m, dev->msg_thread_pid);
    DEBUG("pir_send_msg: msg_send_int: %i\n", ret);
    switch (ret) {
        case 0:
            DEBUG("pir_send_msg: msg_thread_pid not receptive, event is lost");
            break;
        case 1:
            DEBUG("pir_send_msg: OK");
            break;
        case -1:
            DEBUG("pir_send_msg: msg_thread_pid is gone, clearing it");
            dev->msg_thread_pid = KERNEL_PID_UNDEF;
            break;
    }
    DEBUG("\n");
}

static void pir_callback(void *arg)
{
    DEBUG("pir_callback: %p\n", arg);
    pir_t *dev = (pir_t*) arg;
    bool pin_now = gpio_read(dev->p.gpio);
    uint64_t now = xtimer_usec_from_ticks64(xtimer_now64());

    /* We were busy counting */
    if (dev->active && dev->start_active_time != 0xFFFFFFFFFFFFFFFF) {
        /* Add into accumulation */
        dev->accum_active_time += (now - dev->start_active_time);
    }
    /* Pin is rising */
    if (pin_now == dev->p.active_high) {
        dev->start_active_time = now;
        dev->active = true;
    /* Pin is falling */
    } else {
        dev->start_active_time = 0xFFFFFFFFFFFFFFFF;
        dev->active = false;
    }

    if (dev->msg_thread_pid != KERNEL_PID_UNDEF) {
        pir_send_msg(dev, pir_get_status(dev));
    }
}

static int pir_activate_int(pir_t *dev)
{
    return gpio_init_int(dev->p.gpio, GPIO_IN, GPIO_BOTH, pir_callback, dev);
}
