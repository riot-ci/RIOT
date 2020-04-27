/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_msg
 *
 * @{
 *
 * @file
 * @brief       Messaging Bus API for inter process message broadcast.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef MSG_BUS_H
#define MSG_BUS_H

#include <stdint.h>

#include "list.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A message bus is just a list of subscribers.
 */
typedef list_node_t msb_bus_t;

/**
 * @brief Static initializer for @ref msb_bus_t.
 */
#define MSG_BUS_INIT    {0}

/**
 * @brief Message bus subscriber entry.
 *        Should not be modified by the user.
 */
typedef struct {
    msb_bus_t next;         /**< next subscriber */
    uint32_t event_mask;    /**< Bitmask of event classes */
    kernel_pid_t pid;       /**< Subscriber PID */
} msg_bus_entry_t;

/**
 * @brief Attach a thread to a message bus.
 *
 * This attaches a message bus subscriber entry to a message bus.
 * Subscribe to events on the bus using @ref msg_bus_detach.
 * The thread will then receive events with a matching type that are
 * posted on the bus.
 *
 * Events can be received with @ref msg_receive.
 *
 * @param[in] bus           The message bus to attach to
 * @param[in] entry         Message bus subscriber entry
 */
static inline void msg_bus_attach(msb_bus_t *bus, msg_bus_entry_t *entry)
{
    entry->next.next = NULL;
    entry->event_mask = 0;
    entry->pid = sched_active_pid;

    list_add(bus, &entry->next);
}

/**
 * @brief Remove a thread from a message bus.
 *
 * This removes the calling thread from the message bus.
 *
 * @note Call this function before the thread terminates.
 *
 * @param[in] bus           The message bus from which to detach
 */
static inline void msg_bus_detach(msb_bus_t *bus) {
    for (msb_bus_t *e = bus->next; e; e = e->next) {

        msg_bus_entry_t *subscriber = container_of(e, msg_bus_entry_t, next);

        if (subscriber->pid == sched_active_pid) {
            list_remove(bus, &subscriber->next);
            break;
        }
    }
}

/**
 * @brief Subscribe to an event on the message bus.
 *
 * @pre The @p entry has been attached to a bus with @ref msg_bus_attach.
 *
 * @param[in] entry         The message bus entry
 * @param[in] type          The event type to subscribe to (range: 0_31)
 */
static inline void msg_bus_subscribe(msg_bus_entry_t *entry, uint8_t type)
{
    assert(type < 32);
    entry->event_mask |= (1 << type);
}

/**
 * @brief Unsubscribe from an event on the message bus.
 *
 * @pre The @p entry has been attached to a bus with @ref msg_bus_attach.
 *
 * @param[in] entry         The message bus entry
 * @param[in] type          The event type to unsubscribe (range: 0_31)
 */
static inline void msg_bus_unsubscribe(msg_bus_entry_t *entry, uint8_t type)
{
    assert(type < 32);
    entry->event_mask &= ~(1 << type);
}

/**
 * @brief Post a pre-assembled message to a bus.
 *
 * This function sends a message to all threads listening on the bus which are
 * listening for messages of @p event_class with sub-ID @p event_id.
 *
 * It behaves identical to @see msg_bus_post, but sends a pre-defined message.
 *
 * @param[in] m             The message to post the bus
 * @param[in] bus           The message bus to post the message on
 *
 * @return                  The number of threads the message was sent to.
 */
int msg_send_bus(msg_t *m, msb_bus_t *bus);

/**
 * @brief Post a message to a bus.
 *
 * This function sends a message to all threads listening on the bus which are
 * listening for messages of @p type.
 *
 * It is safe to call this function from interrupt context.
 *
 * @param[in] bus           The message bus to post this on
 * @param[in] type          The event type (range: 0…31)
 * @param[in] arg           Optional event parameter
 *
 * @return                  The number of threads the event was posted to.
 */
static inline int msg_bus_post(msb_bus_t *bus, uint8_t type, char *arg)
{
    assert(type < 32);

    msg_t m = {
        .type = type,
        .content.ptr = arg,
    };

    return msg_send_bus(&m, bus);
}

#ifdef __cplusplus
}
#endif

#endif /* MSG_BUS_H */
/** @} */
