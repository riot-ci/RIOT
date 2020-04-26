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
 * @brief Message bus internal data structure.
 *        Should not be modified by the user.
 */
struct msg_bus_internal {
    list_node_t next;       /**< next subscriber */
    uint32_t event_mask;    /**< Bitmask of event classes */
    kernel_pid_t pid;       /**< Subscriber PID */
};

/**
 * @brief Message bus subscriber entry.
 *        This contains a list of all events that the thread will
 *        subscribe to.
 */
typedef struct {
    struct msg_bus_internal _internal;  /**< Internal data structure  */
    const uint16_t *events;             /**< Pointer to the array of events to subscribe to */
    uint16_t num_events;                /**< Number of events in the array */
} msg_bus_entry_t;

/**
 * @brief Generates a message type from Event Class and Event Type.
 *
 * @param[in] c     The Event Class (5 bit)
 * @param[in] s     The Event Type  (11 bit)
 */
#define MSG_BUS_ID(c, s)    (((c) & 0x1F) | (((s) & 0x7FF) << 5))

/**
 * @brief Generates the bit mask for an Event Class
 *
 * @param[in] c     The Event Class (5 bit)
 */
#define MSG_BUS_ID_MASK(c)  (1 << ((c) & 0x1F))

/**
 * @brief Get the Event Class from a message.
 *
 * @param[in] m     The message to read from.
 * @return          The Event Class of the message.
 */
static inline uint8_t msg_bus_class(msg_t *m) {
    return m->type & 0x1F;
}

/**
 * @brief Get the Event Type from a message.
 *
 * @param[in] m     The message to read from.
 * @return          The Event Type of the message.
 */
static inline uint8_t msg_bus_type(msg_t *m) {
    return m->type >> 5;
}

/**
 * @brief Subscribe to a message bus.
 *
 * This function subscribes the calling thread to the message bus @p bus.
 * When the thread calls @see msg_receive afterwards, it will receive
 * all events postet on the bus that match one of the message types in @p entry.
 *
 * @param[in] bus           The message bus to listen on
 * @param[in] entry         Subscriber entry, contains array of all events that
 *                          the thread should be woken for.
 */
static inline void msg_bus_subscribe(list_node_t *bus, msg_bus_entry_t *entry) {
    entry->_internal.next.next = NULL;
    entry->_internal.pid = sched_active_pid;
    entry->_internal.event_mask = 0;

    for (int i = 0; i < entry->num_events; ++i) {
        entry->_internal.event_mask |= MSG_BUS_ID_MASK(entry->events[i]);
    }

    list_add(bus, &entry->_internal.next);
}

/**
 * @brief Unsubscribe from a message bus.
 *
 * This removes the calling thread from the message bus.
 *
 * @note Call this function before the thread terminates.
 *
 * @param[in] bus           The message bus from which to unsubscribe
 */
static inline void msg_bus_unsubscribe(list_node_t *bus) {
    for (list_node_t *e = bus->next; e; e = e->next) {

        msg_bus_entry_t *subscriber = container_of(e, msg_bus_entry_t, _internal.next);

        if (subscriber->_internal.pid == sched_active_pid) {
            list_remove(bus, &subscriber->_internal.next);
            break;
        }
    }
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
int msg_send_bus(msg_t *m, list_node_t *bus);

/**
 * @brief Post a message to a bus.
 *
 * This function sends a message to all threads listening on the bus which are
 * listening for messages of @p event_class with sub-ID @p event_id.
 *
 * It is safe to call this function from interrupt context.
 *
 * @param[in] bus           The message bus to post this on
 * @param[in] event_class   The class of the event (major event ID)
 * @param[in] event_type    The type of the event (minor event ID)
 * @param[in] arg           Optional event parameter
 *
 * @return                  The number of threads the event was posted to.
 */
static inline int msg_bus_post(list_node_t *bus, uint8_t event_class,
                               uint16_t event_type, char *arg)
{
    msg_t m = {
        .type = MSG_BUS_ID(event_class, event_id),
        .content.ptr = arg,
    };

    return msg_send_bus(&m, bus);
}

#ifdef __cplusplus
}
#endif

#endif /* MSG_BUS_H */
/** @} */
