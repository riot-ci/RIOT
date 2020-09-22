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

static bool res_16b = 0;
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

typedef uint16_t (*sample_gen_t)(uint8_t *dst, size_t len,
                                 uint16_t period, uint16_t x);

static uint16_t _fill_sine_samples_8(uint8_t *buf, size_t len,
                                     uint16_t period, uint16_t x)
{
    for (uint16_t i = 0; i < len; ++i) {
        x += ISIN_PERIOD / period;
        buf[i] = isin(x & ISIN_PERIOD) >> 5;
    }

    return x;
}

static uint16_t _fill_sine_samples_16(uint8_t *buf, size_t len,
                                      uint16_t period, uint16_t x)
{
    for (uint16_t i = 0; i < len; ++i) {
        x += ISIN_PERIOD / period;

        uint16_t y = isin(x & ISIN_PERIOD);
        buf[i]   = y;
        buf[++i] = y >> 8;
    }

    return x;
}

static void play_function(uint16_t period, uint32_t samples, sample_gen_t fun)
{
    static uint8_t buf[2][DAC_BUF_SIZE];

    mutex_t lock = MUTEX_INIT_LOCKED;
    uint8_t cur_buf = 0;
    uint16_t x = 0;

    dac_play_set_cb(DAC_CHAN, _unlock, &lock);

    while (samples) {
        size_t len = min(samples, DAC_BUF_SIZE);
        samples -= len;
        x = fun(buf[cur_buf], len, period, x);

        dac_play(DAC_CHAN, buf[cur_buf], len);

        mutex_lock(&lock);
        cur_buf = !cur_buf;
    }
}

#if ENABLE_GREETING
static int cmd_greeting(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    if (sample_rate != 8000 || res_16b) {
        puts("Warning: audio clip was recoded with 8bit/8000 Hz");
    }

    puts("Play Greeting…");
    dac_play(DAC_CHAN, hello_raw, hello_raw_len);

    return 0;
}
#endif

static void _dac_init(void)
{
    printf("init DAC with %u bit, %u Hz\n", res_16b ? 16 : 8, sample_rate);
    dac_play_init(DAC_CHAN, sample_rate,
                  res_16b ? DAC_FLAG_16BIT : DAC_FLAG_8BIT, NULL, NULL);
}

static int cmd_init(int argc, char **argv)
{
    printf("argc: %d\n", argc);

    if (argc < 2) {
        printf("usage: %s <freq> <bit>\n", argv[0]);
        return 1;
    }

    if (argc > 2) {
        unsigned bit = atoi(argv[2]);

        if (bit != 8 && bit != 16) {
            printf("Only 8 and 16 bit samples supported.\n");
            return 1;
        }

        res_16b = bit == 16;
    }

    sample_rate = atoi(argv[1]);

    _dac_init();

    return 0;
}

static int cmd_sine(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <freq> <secs>\n", argv[0]);
        return 1;
    }

    unsigned freq = atoi(argv[1]);
    unsigned secs = atoi(argv[2]);

    play_function((sample_rate + freq/2) / freq, secs * sample_rate,
                  res_16b ? _fill_sine_samples_16 : _fill_sine_samples_8);

    return 0;
}

static const shell_command_t shell_commands[] = {
#if ENABLE_GREETING
    { "hello", "Play Greeting", cmd_greeting },
#endif
    { "init", "Initialize DAC", cmd_init },
    { "sine", "Play Sine wave", cmd_sine },
    { NULL, NULL, NULL }
};

int main(void)
{
    dac_init(DAC_CHAN);
    _dac_init();

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
