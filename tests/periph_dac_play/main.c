/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       DAC (audio) test application
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "board.h"
#include "thread.h"

#include "msg.h"
#include "mutex.h"
#include "periph/dac.h"
#include "shell.h"

#include "blob/hello.raw.h"

#ifndef DAC_CHAN
#define DAC_CHAN DAC_LINE(0)
#endif

#ifndef ENABLE_GREETING
#define ENABLE_GREETING (0)
#endif

#ifndef min
#define min(a, b) ((a) > (b) ? (b) : (a))
#endif

#define DAC_BUF_SIZE    (2048)

#define ISIN_PERIOD     (0x7FFF)
#define ISIN_MAX        (0x1000)

static unsigned sample_rate = 8000;

/**
 * @brief A sine approximation via a fourth-order cosine approx.
 *        source: https://www.coranac.com/2009/07/sines/
 *
 * @param x     angle (with 2^15 units/circle)
 * @return      sine value (Q12)
 */
static int32_t isin(int32_t x)
{
    int32_t c, y;
    static const int32_t qN = 13,
                         qA = 12,
                          B = 19900,
                          C = 3516;

    c = x << (30 - qN);         /* Semi-circle info into carry. */
    x -= 1 << qN;               /* sine -> cosine calc          */

    x = x << (31 - qN);         /* Mask with PI                 */
    x = x >> (31 - qN);         /* Note: SIGNED shift! (to qN)  */
    x = x * x >> (2 * qN - 14); /* x=x^2 To Q14                 */

    y = B - (x * C >> 14);      /* B - x^2*C                    */
    y = (1 << qA)               /* A - x^2*(B-x^2*C)            */
      - (x * y >> 16);

    return c >= 0 ? y : -y;
}

static void _unlock(void *arg)
{
    mutex_unlock(arg);
}

static size_t _fill_sine_samples(uint8_t *buf, uint16_t period,
                               uint16_t *x, size_t len)
{
    len = min(len, DAC_BUF_SIZE);

    for (uint16_t i = 0; i < len; ++i) {
        *x += ISIN_PERIOD / period;
        buf[i] = isin(*x & ISIN_PERIOD) >> 5;
    }

    return len;
}

static void play_sine(uint16_t period, uint32_t samples)
{
    static uint8_t buf[2][DAC_BUF_SIZE];

    mutex_t lock = MUTEX_INIT_LOCKED;
    uint8_t cur_buf = 0;
    uint16_t x = 0;

    dac_play_set_cb(DAC_CHAN, _unlock, &lock);

    while (samples) {
        samples -= _fill_sine_samples(buf[cur_buf], period, &x, samples);

        dac_play(DAC_CHAN, buf[cur_buf], DAC_BUF_SIZE);

        mutex_lock(&lock);
        cur_buf = !cur_buf;
    }
}

#if ENABLE_GREETING
static int cmd_greeting(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("Play Greeting…");
    dac_play(DAC_CHAN, hello_raw, hello_raw_len);

    return 0;
}
#endif

static int cmd_sine(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <freq> <secs>\n", argv[0]);
        return 1;
    }

    unsigned freq = atoi(argv[1]);
    unsigned secs = atoi(argv[2]);

    play_sine((sample_rate + freq/2) / freq, secs * sample_rate);

    return 0;
}

static const shell_command_t shell_commands[] = {
#if ENABLE_GREETING
    { "hello", "Play Greeting", cmd_greeting },
#endif
    { "sine", "Play Sine wave", cmd_sine },
    { NULL, NULL, NULL }
};

int main(void)
{
    printf("init DAC with 8 bit, %u samples / s\n", sample_rate);

    dac_init(DAC_CHAN);
    dac_play_init(DAC_CHAN, sample_rate, DAC_FLAG_8BIT, NULL, NULL);

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
