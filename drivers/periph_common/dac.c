/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_dac
 * @{
 *
 * @file
 * @brief       Common DAC function fallback implementations
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include "board.h"
#include "kernel_defines.h"
#include "macros/units.h"
#include "periph/dac.h"
#include "periph/timer.h"

#if !defined(PERIPH_DAC_PROVIDES_PLAY) && IS_ACTIVE(MODULE_PERIPH_DAC)

#ifndef DAC0_TIMER
#define DAC0_TIMER   (TIMER_NUMOF - 1)
#endif
#ifndef DAC1_TIMER
#define DAC1_TIMER   (TIMER_NUMOF - 2)
#endif

#ifndef DAC_TIMER_FREQ
#define DAC_TIMER_FREQ  MHZ(1)
#endif

static struct dac_ctx {
    const uint8_t *buffers[2];  /* The two sample buffers                   */
    size_t buffer_len[2];       /* Size of the sample buffers               */
    size_t idx;                 /* Current position in the current buffer   */
    dac_cb_t cb;                /* Called when the current buffer is done   */
    void *cb_arg;               /* Callback argument                        */
    tim_t timer;                /* Timer used for DAC                       */
    uint16_t sample_ticks;      /* Timer ticks per sample                   */
    uint8_t cur;                /* Active sample buffer                     */
    uint8_t playing;            /* DAC is playing                           */
    uint8_t is_16bit;           /* Sample size is 16 instead of 8 bit       */
} _ctx[DAC_NUMOF];

static void _timer_cb(void *arg, int chan)
{
    struct dac_ctx *ctx = arg;

    const uint8_t cur  = ctx->cur;
    const uint8_t *buf = ctx->buffers[cur];
    const size_t len   = ctx->buffer_len[cur];

    if (ctx->is_16bit) {
        size_t idx_real = 2 * ctx->idx;
        dac_set(0, (buf[idx_real + 1] << 8) | buf[idx_real]);
    } else {
        dac_set(0, buf[ctx->idx] << 8);
    }

    if (++ctx->idx >= len) {

        /* invalidate old buffer */
        ctx->buffer_len[cur] = 0;

        ctx->idx = 0;
        ctx->cur = !cur;

        if (ctx->buffer_len[!cur] == 0) {
            ctx->playing = 0;
            timer_clear(ctx->timer, chan);
        } else if (ctx->cb) {
            ctx->cb(ctx->cb_arg);
        }
    }
}

void dac_play_init(dac_t dac, uint16_t sample_rate, uint8_t flags,
                   dac_cb_t cb, void *cb_arg)
{
    assert(dac < DAC_NUMOF);

    if (dac == 0) {
        _ctx[dac].timer = DAC0_TIMER;
    } else if (dac == 1) {
        _ctx[dac].timer = DAC1_TIMER;
    }

    _ctx[dac].cb           = cb;
    _ctx[dac].cb_arg       = cb_arg;
    _ctx[dac].sample_ticks = DAC_TIMER_FREQ / sample_rate;
    _ctx[dac].is_16bit     = flags & DAC_FLAG_16BIT;

    timer_init(_ctx[dac].timer, DAC_TIMER_FREQ, _timer_cb, &_ctx[dac]);
}

void dac_play_set_cb(dac_t dac, dac_cb_t cb, void *cb_arg)
{
    /* allow to update cb_arg independent of cb */
    if (cb || cb_arg == NULL) {
        _ctx[dac].cb     = cb;
    }
    _ctx[dac].cb_arg = cb_arg;
}

void dac_play(dac_t dac, const void *buf, size_t len)
{
    uint8_t next = !_ctx[dac].cur;

    _ctx[dac].buffers[next]    = buf;
    _ctx[dac].buffer_len[next] = len;

    if (_ctx[dac].playing) {
        return;
    }

    _ctx[dac].playing = 1;

    timer_set_periodic(_ctx[dac].timer, 0, _ctx[dac].sample_ticks,
                       TIM_FLAG_RESET_ON_MATCH | TIM_FLAG_RESET_ON_SET);
}

#endif
