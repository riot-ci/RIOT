/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef CONGURE_IMPL_H
#define CONGURE_IMPL_H

#include "congure.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    congure_snd_t super;
    uint8_t init_calls;
    uint8_t inter_msg_interval_calls;
    uint8_t report_msg_sent_calls;
    uint8_t report_msg_discarded_calls;
    uint8_t report_msgs_timeout_calls;
    uint8_t report_msgs_lost_calls;
    uint8_t report_msg_acked_calls;
    uint8_t report_ecn_ce_calls;
    struct {
        congure_snd_t *c;
        void *ctx;
    } init_args;
    struct {
        congure_snd_t *c;
        unsigned msg_size;
    } inter_msg_interval_args;
    struct {
        congure_snd_t *c;
        unsigned msg_size;
    } report_msg_sent_args;
    struct {
        congure_snd_t *c;
        unsigned msg_size;
    } report_msg_discarded_args;
    struct {
        congure_snd_t *c;
        congure_snd_msg_t *msgs;
    } report_msgs_timeout_args;
    struct {
        congure_snd_t *c;
        congure_snd_msg_t *msgs;
    } report_msgs_lost_args;
    struct {
        congure_snd_t *c;
        congure_snd_msg_t *msg;
        congure_snd_ack_t *ack;
    } report_msg_acked_args;
    struct {
        congure_snd_t *c;
        ztimer_now_t time;
    } report_ecn_ce_args;
} congure_test_snd_t;

int congure_test_snd_setup(congure_test_snd_t *c, unsigned id);

#ifdef __cplusplus
}
#endif

#endif /* CONGURE_IMPL_H */
/** @} */
