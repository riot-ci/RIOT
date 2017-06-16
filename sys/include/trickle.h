/*
 * Trickle constants and prototypes
 *
 * Copyright (C) 2013, 2014  INRIA.
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup sys_trickle Trickle Timer
 * @ingroup sys
 * @{
 * @file
 * @brief   Implementation of a generic Trickle Algorithm (RFC 6206)
 *
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#ifndef TRICKLE_H
#define TRICKLE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "xtimer.h"
#include "thread.h"

/**
 * @brief a generic callback function with arguments that is called by
 *        trickle periodically
 */
typedef struct {
    void (*func)(void *);       /**< callback function pointer */
    void *args;                 /**< callback function arguments */
} trickle_callback_t;

/** @brief all state variables for a trickle timer */
typedef struct {
    uint8_t k;                      /**< redundancy constant */
    uint8_t Imax;                   /**< maximum interval size, described as doublings */
    uint16_t c;                     /**< counter */
    uint32_t Imin;                  /**< minimum interval size */
    uint32_t I;                     /**< current interval size */
    uint32_t t;                     /**< time within the current interval */
    kernel_pid_t pid;               /**< pid of trickles target thread */
    trickle_callback_t callback;    /**< the callback function and parameter that trickle is calling
                                         after each interval */
    msg_t msg;                      /**< the msg_t to use for intervals */
    uint64_t msg_time;              /**< interval in ms */
    xtimer_t msg_timer;             /**< xtimer to send a msg_t to the target thread
                                         for a new interval */
} trickle_t;

/**
 * @brief resets the trickle timer
 *
 * @pre `trickle->I > trickle->Imin`
 * @see https://tools.ietf.org/html/rfc6206#section-4.2, number 6
 *
 * @param[in] trickle   the trickle timer
 */
void trickle_reset_timer(trickle_t *trickle);

/**
 * @brief start the trickle timer
 *
 * @pre `Imin > 0`
 * @pre `(Imin << Imax) < (UINT32_MAX / 2)` to avoid overflow of uint32_t
 *
 * @param[in] pid                   target thread
 * @param[in] trickle               trickle timer
 * @param[in] msg_type              msg_t.type for messages
 * @param[in] Imin                  minimum interval
 * @param[in] Imax                  maximum interval
 * @param[in] k                     redundancy constant
 */
void trickle_start(kernel_pid_t pid, trickle_t *trickle, uint16_t msg_type,
                   uint32_t Imin, uint8_t Imax, uint8_t k);

/**
 * @brief stops the trickle timer
 *
 * @param[in] trickle   trickle timer
 */
void trickle_stop(trickle_t *trickle);

/**
 * @brief increments the counter by one
 *
 * @param[in] trickle   trickle timer
 */
void trickle_increment_counter(trickle_t *trickle);

/**
 * @brief is called after the interval is over and calculates the next interval
 *
 * @pre `(trickle->I > 0)` required for trickle algorithm to work
 *
 * @param[in] trickle   trickle timer
 */
void trickle_interval(trickle_t *trickle);

/**
 * @brief is called after the callback interval is over and calls the callback function
 *
 * @param[in] trickle   trickle timer
 */
void trickle_callback(trickle_t *trickle);

#ifdef __cplusplus
}
#endif

#endif /* TRICKLE_H */
/** @} */
