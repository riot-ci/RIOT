/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lvgl
 * @{
 *
 * @file
 * @brief       LittlevGL glue code
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "thread.h"

#include "xtimer.h"
#include "log.h"

#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include "lvgl_riot.h"
#include "disp_dev.h"

#ifndef LVGL_THREAD_PRIO
#define LVGL_THREAD_PRIO        (THREAD_PRIORITY_MAIN + 1)
#endif

#ifndef LVGL_COLOR_BUF_SIZE
#define LVGL_COLOR_BUF_SIZE     (LV_HOR_RES_MAX * 5)
#endif

#ifndef LVGL_ACTIVITY_PERIOD
#define LVGL_ACTIVITY_PERIOD    (5 * MS_PER_SEC)
#endif

#ifndef LVGL_TASK_HANDLER_DELAY
#define LVGL_TASK_HANDLER_DELAY (5 * US_PER_MS)
#endif

static char _task_thread_stack[THREAD_STACKSIZE_MAIN];
static kernel_pid_t _task_thread_pid;

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LVGL_COLOR_BUF_SIZE];
static disp_dev_t *_dev = NULL;

void *_task_thread(void *arg)
{
    (void)arg;

    while (1) {
        /* Normal operation (no sleep) in < LVGL_ACTIVITY_PERIOD msec
           inactivity */
        if (lv_disp_get_inactive_time(NULL) < LVGL_ACTIVITY_PERIOD) {
            lv_task_handler();
        }
        else { /* Block after LVGL_ACTIVITY_PERIOD msec inactivity */
            thread_flags_wait_any(0x1);

            /* trigger an activity so the task handler is called on the next loop */
            lv_disp_trig_activity(NULL);
        }

        xtimer_usleep(LVGL_TASK_HANDLER_DELAY);
    }

    return NULL;
}

void _disp_map(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    if (!_dev) {
        return;
    }

    _dev->driver->map(_dev,
                      (uint16_t)area->x1, (uint16_t)area->x2,
                      (uint16_t)area->y1, (uint16_t)area->y2,
                      (const uint16_t *)color_p);

    LOG_DEBUG("[lvgl] flush display\n");

    lv_disp_flush_ready(drv);
}

void lvgl_init(disp_dev_t *dev)
{
    _dev = dev;
    lv_init();
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = _disp_map;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);
    lv_disp_buf_init(&disp_buf, buf, NULL, LVGL_COLOR_BUF_SIZE);

    _task_thread_pid = thread_create(_task_thread_stack, sizeof(_task_thread_stack),
                                     LVGL_THREAD_PRIO, THREAD_CREATE_STACKTEST,
                                     _task_thread, NULL, "_task_thread");
}

void lvgl_wakeup(void)
{
    thread_t *tcb = (thread_t *)sched_threads[_task_thread_pid];
    thread_flags_set(tcb, 0x1);
}
