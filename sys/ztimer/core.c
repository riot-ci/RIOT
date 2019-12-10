/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer
 * @{
 *
 * @file
 * @brief       ztimer core functinality
 *
 * This file contains ztimer's main API implementation and functionality
 * present in all ztimer clocks (most notably multiplexing ant extension).
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include <assert.h>
#include <stdint.h>

#include "irq.h"
#include "ztimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _add_entry_to_list(ztimer_clock_t *ztimer, ztimer_base_t *entry);
static void _del_entry_from_list(ztimer_clock_t *ztimer, ztimer_base_t *entry);
static void _ztimer_update(ztimer_clock_t *ztimer);
static void _ztimer_print(ztimer_clock_t *ztimer);

#ifdef MODULE_ZTIMER_EXTEND
static inline uint32_t _min_u32(uint32_t a, uint32_t b) {
    return a < b ? a : b;
}
#endif

static unsigned _is_set(ztimer_t *t)
{
    (void)t;
    /* TODO: implement */
    return 1;
}

void ztimer_remove(ztimer_clock_t *clock, ztimer_t *entry)
{
    unsigned state = irq_disable();

    if (_is_set(entry)) {
        ztimer_update_head_offset(clock);
        _del_entry_from_list(clock, &entry->base);

        _ztimer_update(clock);
    }

    irq_restore(state);
}

void ztimer_set(ztimer_clock_t *ztimer, ztimer_t *entry, uint32_t val)
{
    DEBUG("ztimer_set(): %p: set %p at %"PRIu32" offset %"PRIu32"\n",
            (void *)ztimer, (void *)entry, ztimer->ops->now(ztimer), val);

    unsigned state = irq_disable();

    ztimer_update_head_offset(ztimer);
    if (_is_set(entry)) {
        _del_entry_from_list(ztimer, &entry->base);
    }

    entry->base.offset = val;
    _add_entry_to_list(ztimer, &entry->base);
    if (ztimer->list.next == &entry->base) {
#ifdef MODULE_ZTIMER_EXTEND
        if (ztimer->max_value < 0xffffffff) {
            val = _min_u32(val, ztimer->max_value >> 1);
        }
        DEBUG("ztimer_set(): %p setting %"PRIu32"\n", (void *)ztimer, val);
#endif
        ztimer->ops->set(ztimer, val);
    }

    irq_restore(state);
}

static void _add_entry_to_list(ztimer_clock_t *ztimer, ztimer_base_t *entry)
{
    uint32_t delta_sum = 0;

    ztimer_base_t *list = &ztimer->list;

    /* Jump past all entries which are set to an earlier target than the new entry */
    while (list->next) {
        ztimer_base_t *list_entry = list->next;
        if ((list_entry->offset + delta_sum) > entry->offset) {
            break;
        }
        delta_sum += list_entry->offset;
        list = list->next;
    }

    /* Insert into list */
    entry->next = list->next;
    entry->offset -= delta_sum;
    if (entry->next) {
        entry->next->offset -= entry->offset;
    }
    list->next = entry;
    DEBUG("_add_entry_to_list() %p offset %"PRIu32"\n", (void *)entry, entry->offset);

}

static uint32_t _add_modulo(uint32_t a, uint32_t b, uint32_t mod)
{
    if (a < b) {
        a += mod + 1;
    }
    return a-b;
}

uint32_t ztimer_now(ztimer_clock_t *ztimer)
{
#ifdef MODULE_ZTIMER_EXTEND
    if (ztimer->max_value < 0xffffffff) {
        assert(ztimer->max_value);
        unsigned state = irq_disable();
        uint32_t lower_now = ztimer->ops->now(ztimer);
        DEBUG("ztimer_now() checkpoint=%"PRIu32" lower_last=%"PRIu32" lower_now=%"PRIu32" diff=%"PRIu32"\n",
                ztimer->checkpoint, ztimer->lower_last, lower_now,
                _add_modulo(lower_now, ztimer->lower_last, ztimer->max_value));
        ztimer->checkpoint += _add_modulo(lower_now, ztimer->lower_last, ztimer->max_value);
        ztimer->lower_last = lower_now;
        DEBUG("ztimer_now() returning %"PRIu32"\n", ztimer->checkpoint);
        irq_restore(state);
        return ztimer->checkpoint;
#else
    if (0) {
#endif
    }
    else {
        return ztimer->ops->now(ztimer);
    }
}

void ztimer_update_head_offset(ztimer_clock_t *ztimer)
{
    uint32_t old_base = ztimer->list.offset;
    uint32_t now = ztimer_now(ztimer);
    uint32_t diff = now - old_base;

    ztimer_base_t *entry = ztimer->list.next;
    DEBUG("ztimer %p: ztimer_update_head_offset(): diff=%" PRIu32 " old head %p\n",
        (void *)ztimer, diff, (void *)entry);
    if (entry) {
        do {
            if (diff <= entry->offset) {
                entry->offset -= diff;
                break;
            }
            else {
                diff -= entry->offset;
                entry->offset = 0;
                if (diff) {
                    /* skip timers with offset==0 */
                    do {
                        entry = entry->next;
                    } while (entry && (entry->offset == 0));
                }
            }
        } while (diff && entry);
        DEBUG("ztimer %p: ztimer_update_head_offset(): now=%" PRIu32 " new head %p",
            (void *)ztimer, now, (void *)entry);
        if (entry) {
            DEBUG(" offset %" PRIu32 "\n", entry->offset);
        }
        else {
            DEBUG("\n");
        }
    }

    ztimer->list.offset = now;
}

static void _del_entry_from_list(ztimer_clock_t *ztimer, ztimer_base_t *entry)
{
    DEBUG("_del_entry_from_list()\n");
    ztimer_base_t *list = &ztimer->list;

    while (list->next) {
        ztimer_base_t *list_entry = list->next;
        if (list_entry == entry) {
            list->next = entry->next;
            if (list->next) {
                list_entry = list->next;
                list_entry->offset += entry->offset;
            }
            break;
        }
        list = list->next;
    }
}

static ztimer_t *_now_next(ztimer_clock_t *ztimer)
{
    ztimer_base_t *entry = ztimer->list.next;

    if (entry && (entry->offset == 0)) {
        ztimer->list.next = entry->next;
        return (ztimer_t*)entry;
    }
    else {
        return NULL;
    }
}

static void _ztimer_update(ztimer_clock_t *ztimer)
{
#ifdef MODULE_ZTIMER_EXTEND
    if (ztimer->max_value < 0xffffffff) {
        if (ztimer->list.next) {
            ztimer->ops->set(ztimer, _min_u32(ztimer->list.next->offset, ztimer->max_value >> 1));
        }
        else {
            ztimer->ops->set(ztimer, ztimer->max_value >> 1);
        }

#else
    if (0)
#endif
    }
    else {
        if (ztimer->list.next) {
            ztimer->ops->set(ztimer, ztimer->list.next->offset);
        }
        else {
            ztimer->ops->cancel(ztimer);
        }
    }
}

void ztimer_handler(ztimer_clock_t *ztimer)
{
    DEBUG("ztimer_handler(): %p now=%"PRIu32"\n", (void *)ztimer, ztimer->ops->now(ztimer));
    if (ENABLE_DEBUG) {
        _ztimer_print(ztimer);
    }

#ifdef MODULE_ZTIMER_EXTEND
    if (ztimer->max_value < 0xffffffff) {
        /* calling now triggers checkpointing */
        uint32_t now = ztimer_now(ztimer);

        if (ztimer->list.next) {
            uint32_t target = ztimer->list.offset + ztimer->list.next->offset;
            int32_t diff = (int32_t)(target - now);
            if (diff > 0) {
                DEBUG("ztimer_handler(): %p postponing by %"PRIi32"\n", (void *)ztimer, diff);
                ztimer->ops->set(ztimer, _min_u32(diff, ztimer->max_value >> 1));
                return;
            }
            else {
                DEBUG("ztimer_handler(): %p diff=%"PRIi32"\n", (void *)ztimer, diff);
            }
        }
        else {
            DEBUG("ztimer_handler(): %p intermediate\n", (void *)ztimer);
            ztimer->ops->set(ztimer, ztimer->max_value >> 1);
            return;
        }
    }
    else {
        DEBUG("ztimer_handler(): no checkpointing\n");
    }
#endif

    ztimer->list.offset += ztimer->list.next->offset;
    ztimer->list.next->offset = 0;

    ztimer_t *entry = _now_next(ztimer);
    while (entry) {
        DEBUG("ztimer_handler(): trigger %p->%p at %"PRIu32"\n",
                (void *)entry, (void *)entry->base.next, ztimer->ops->now(ztimer));
        entry->callback(entry->arg);
        entry = _now_next(ztimer);
        if (!entry) {
            /* See if any more alarms expired during callback processing */
            /* This reduces the number of implicit calls to ztimer->ops->now() */
            ztimer_update_head_offset(ztimer);
            entry = _now_next(ztimer);
        }
    }

    _ztimer_update(ztimer);

    if (ENABLE_DEBUG) {
        _ztimer_print(ztimer);
    }
    DEBUG("ztimer_handler(): %p done.\n", (void *)ztimer);
    if (!irq_is_in()) {
        thread_yield_higher();
    }
}

static void _ztimer_print(ztimer_clock_t *ztimer)
{
    ztimer_base_t *entry = &ztimer->list;
    uint32_t last_offset = 0;
    do {
        printf("0x%08x:%" PRIu32 "(%" PRIu32 ")%s", (unsigned)entry, entry->offset, entry->offset +
                last_offset, entry->next ? "->" : "");
        last_offset += entry->offset;

    } while ((entry = entry->next));
    puts("");
}
