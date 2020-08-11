/*
 * Copyright (C) 2014,2017,2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for GPIO peripheral drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "irq.h"
#include "shell.h"
#include "benchmark.h"
#include "periph/gpio.h"

#define BENCH_RUNS_DEFAULT      (1000UL * 100)

#ifdef MODULE_PERIPH_GPIO_IRQ
static void cb(void *arg)
{
    printf("INT: external interrupt from pin %i\n", (int)arg);
}
#endif

static int init_pin(int argc, char **argv, gpio_mode_t mode)
{
    int port, pin;

    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);
    pin  = atoi(argv[2]);

    if (gpio_init(GPIO_PIN(port, pin), mode)) {
        printf("Error to initialize GPIO_PIN(%i, %02i)\n", port, pin);
        return 1;
    }

    return 0;
}

static int init_pin_hl(int argc, char **argv, bool high)
{
    int port, pin, res;

    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);
    pin  = atoi(argv[2]);

    if (high) {
        res = gpio_init_high(GPIO_PIN(port, pin), GPIO_OUT);
    }
    else {
        res = gpio_init_low(GPIO_PIN(port, pin), GPIO_OUT);
    }

    if (res) {
        printf("Error to initialize GPIO_PIN(%i, %02i)\n", port, pin);
        return 1;
    }

    return 0;
}

static int init_out(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_OUT);
}

static int init_out_h(int argc, char **argv)
{
    return init_pin_hl(argc, argv, true);
}

static int init_out_l(int argc, char **argv)
{
    return init_pin_hl(argc, argv, false);
}

static int init_in(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_IN);
}

static int init_in_pu(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_IN_PU);
}

static int init_in_pd(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_IN_PD);
}

static int init_od(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_OD);
}

static int init_od_pu(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_OD_PU);
}

#ifdef MODULE_PERIPH_GPIO_IRQ
static int init_int(int argc, char **argv)
{
    int po, pi;
    gpio_mode_t mode = GPIO_IN;
    gpio_flank_t flank;
    int fl;

    if (argc < 4) {
        printf("usage: %s <port> <pin> <flank> [pull_config]\n", argv[0]);
        puts("\tflank:\n"
             "\t0: falling\n"
             "\t1: rising\n"
             "\t2: both\n"
             "\tpull_config:\n"
             "\t0: no pull resistor (default)\n"
             "\t1: pull up\n"
             "\t2: pull down");
        return 1;
    }

    po = atoi(argv[1]);
    pi = atoi(argv[2]);

    fl = atoi(argv[3]);
    switch (fl) {
        case 0:
            flank = GPIO_FALLING;
            break;
        case 1:
            flank = GPIO_RISING;
            break;
        case 2:
            flank = GPIO_BOTH;
            break;
        default:
            puts("error: invalid value for active flank");
            return 1;
    }

    if (argc >= 5) {
        int pr = atoi(argv[4]);
        switch (pr) {
            case 0:
                mode = GPIO_IN;
                break;
            case 1:
                mode = GPIO_IN_PU;
                break;
            case 2:
                mode = GPIO_IN_PD;
                break;
            default:
                puts("error: invalid pull resistor option");
                return 1;
        }
    }

    if (gpio_init_int(GPIO_PIN(po, pi), mode, flank, cb, (void *)pi) < 0) {
        printf("error: init_int of GPIO_PIN(%i, %i) failed\n", po, pi);
        return 1;
    }
    printf("GPIO_PIN(%i, %i) successfully initialized as ext int\n", po, pi);

    return 0;
}

static int enable_int(int argc, char **argv)
{
    int po, pi;
    int status;

    if (argc < 4) {
        printf("usage: %s <port> <pin> <status>\n", argv[0]);
        puts("\tstatus:\n"
             "\t0: disable\n"
             "\t1: enable\n");
        return 1;
    }

    po = atoi(argv[1]);
    pi = atoi(argv[2]);

    status = atoi(argv[3]);

    switch (status) {
        case 0:
            puts("disabling GPIO interrupt");
            gpio_irq_disable(GPIO_PIN(po, pi));
            break;
        case 1:
            puts("enabling GPIO interrupt");
            gpio_irq_enable(GPIO_PIN(po, pi));
            break;
        default:
            puts("error: invalid status");
            return 1;
    }

    return 0;
}
#endif

static int cmd_read(int argc, char **argv)
{
    int port, pin;

    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);
    pin = atoi(argv[2]);

    if (gpio_read(GPIO_PIN(port, pin))) {
        printf("GPIO_PIN(%i.%02i) is HIGH\n", port, pin);
    }
    else {
        printf("GPIO_PIN(%i.%02i) is LOW\n", port, pin);
    }

    return 0;
}

static int cmd_set(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    gpio_set(GPIO_PIN(atoi(argv[1]), atoi(argv[2])));

    return 0;
}

static int cmd_clear(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    gpio_clear(GPIO_PIN(atoi(argv[1]), atoi(argv[2])));

    return 0;
}

static int cmd_toggle(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    gpio_toggle(GPIO_PIN(atoi(argv[1]), atoi(argv[2])));

    return 0;
}

static int bench(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port> <pin> [# of runs]\n", argv[0]);
        return 1;
    }

    gpio_t pin = GPIO_PIN(atoi(argv[1]), atoi(argv[2]));
    unsigned long runs = BENCH_RUNS_DEFAULT;
    if (argc > 3) {
        runs = (unsigned long)atol(argv[3]);
    }

    puts("\nGPIO driver run-time performance benchmark\n");
    BENCHMARK_FUNC("nop loop", runs, __asm__ volatile("nop"));
    BENCHMARK_FUNC("gpio_set", runs, gpio_set(pin));
    BENCHMARK_FUNC("gpio_clear", runs, gpio_clear(pin));
    BENCHMARK_FUNC("gpio_toggle", runs, gpio_toggle(pin));
    BENCHMARK_FUNC("gpio_read", runs, (void)gpio_read(pin));
    BENCHMARK_FUNC("gpio_write", runs, gpio_write(pin, 1));
    puts("\n --- DONE ---");
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init_out", "init as output (push-pull mode)", init_out },
    { "init_out_h", "init as output (push-pull mode, high after init)", init_out_h },
    { "init_out_l", "init as output (push-pull mode, low after init)", init_out_l },
    { "init_in", "init as input w/o pull resistor", init_in },
    { "init_in_pu", "init as input with pull-up", init_in_pu },
    { "init_in_pd", "init as input with pull-down", init_in_pd },
    { "init_od", "init as output (open-drain without pull resistor)", init_od },
    { "init_od_pu", "init as output (open-drain with pull-up)", init_od_pu },
#ifdef MODULE_PERIPH_GPIO_IRQ
    { "init_int", "init as external INT w/o pull resistor", init_int },
    { "enable_int", "enable or disable gpio interrupt", enable_int },
#endif
    { "read", "read pin status", cmd_read },
    { "set", "set pin to HIGH", cmd_set },
    { "clear", "set pin to LOW", cmd_clear },
    { "toggle", "toggle pin", cmd_toggle },
    { "bench", "run a set of predefined benchmarks", bench },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("GPIO peripheral driver test\n");
    puts("In this test, pins are specified by integer port and pin numbers.\n"
         "So if your platform has a pin PA01, it will be port=0 and pin=1,\n"
         "PC14 would be port=2 and pin=14 etc.\n\n"
         "NOTE: make sure the values you use exist on your platform! The\n"
         "      behavior for not existing ports/pins is not defined!");

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
