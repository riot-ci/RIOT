/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for low-level PWM drivers
 *
 * This test initializes the given PWM device to run at 1KHz with a 1000 step
 * resolution.
 *
 * The PWM is then continuously oscillating it's duty cycle between 0% to 100%
 * every 1s on every channel.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "xtimer.h"
#include "irq.h"
#include "shell.h"
#include "timex.h"
#include "periph/pwm.h"

#define OSC_INTERVAL    (10LU * US_PER_MS) /* 10 ms */
#define OSC_STEP        (10)
#define OSC_MODE        PWM_LEFT
#define OSC_FREQU       (1000U)
#define OSC_STEPS       (1000U)
#define PWR_SLEEP       (1U)

bool initiated;

int init(int argc, char** argv) {
    initiated = false;

    if (argc != 5) {
        printf("usage: %s <dev> <mode> <frequency> <resolution>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", PWM_NUMOF);
        printf("\tmode:\n");
        printf("\t\t0: left aligned\n");
        printf("\t\t1: right aligned\n");
        printf("\t\t2: center aligned\n");
        printf("\tfrequency: desired frequency in Hz\n");
        printf("\tresolution: number between 2 and %d\n", UINT16_MAX);
        return 1;
    }
    errno = 0;
    pwm_t dev     = (pwm_t)strtol(argv[1], NULL, 10);
    uint16_t mode = (uint16_t)strtol(argv[2], NULL, 10);
    uint32_t freq = (uint32_t)strtol(argv[3], NULL, 10);
    uint16_t res  = (uint16_t)strtol(argv[4], NULL, 10);
    if (errno != 0) {
        printf("[pwm] Error: some input was not a number or out of bounds.\n");
        return 1;
    }

    if (dev >= PWM_NUMOF) {
        printf("[pwm] Error: device %d is unknown.\n", dev);
        return 1;
    }

    pwm_mode_t pwm_mode;
    switch(mode) {
        case(0):
            pwm_mode = PWM_LEFT;
            break;
        case(1):
            pwm_mode = PWM_RIGHT;
            break;
        case(2):
            pwm_mode = PWM_CENTER;
            break;
        default:
            printf("[pwm] Error: mode %d is not supported.\n", mode);
            return 1;
    }

    if (pwm_init(dev, pwm_mode, freq, res) != 0) {
        initiated = true;
        return 0;
    }

    return 1;
}

int set(int argc, char**argv) {
    if (argc != 4) {
        printf("usage: %s <dev> <ch> <val>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", PWM_NUMOF-1);
        printf("\tch: channel of device\n");
        printf("\tval: duty cycle\n");
        return 1;
    }

    if (!initiated) {
        printf("[pwm] Error: pwm is not initiated.\n");
        printf("Execute init function first.\n");
        return 1;
    }

    errno = 0;
    pwm_t dev   = (pwm_t)strtol(argv[1], NULL, 10);
    uint8_t ch  = (uint8_t)strtol(argv[2], NULL, 10);
    uint16_t dc = (uint16_t)strtol(argv[3], NULL, 10);
    if (errno != 0) {
        printf("[pwm] Error: some input was not a number or out of bounds.\n");
        return 1;
    }
    if (dev >= PWM_NUMOF) {
        printf("[pwm] Error: device %d is unknown.\n", dev);
        return 1;
    }
    if (ch >= pwm_channels(dev)) {
        printf("[pwm] Error: channel %s on device %d is unknown.\n", argv[2], dev);
        return 1;
    }

    printf("Setting duty cycle to %d.\n", dc);
    pwm_set(dev, ch, dc);
    return 0;
}

int oscillate(int argc, char** argv) {
    if (argc > 1) {
        printf("Parameters %s not needed.\n", argv[1]);
    }
    int state = 0;
    int step = OSC_STEP;
    xtimer_ticks32_t last_wakeup = xtimer_now();

    puts("\nRIOT PWM test");
    puts("Connect an LED or scope to PWM pins to see something.\n");

    printf("Available PWM devices: %i.\n", PWM_NUMOF);
    for (unsigned i = 0; i < PWM_NUMOF; i++) {
        uint32_t real_f = pwm_init(PWM_DEV(i), OSC_MODE, OSC_FREQU, OSC_STEPS);
        if (real_f == 0) {
            printf("[pwm] Error: initializing PWM_%u.\n", i);
            return 1;
        }
        else {
            printf("Initialized PWM_%u @ %" PRIu32 "Hz.\n", i, real_f);
        }
    }

    puts("\nLetting the PWM pins oscillate now...");
    while (1) {
        for (unsigned i = 0; i < PWM_NUMOF; i++) {
            for (uint8_t chan = 0; chan < pwm_channels(PWM_DEV(i)); chan++) {
                pwm_set(PWM_DEV(i), chan, state);
            }
        }

        state += step;
        if (state <= 0 || state >= (int)OSC_STEPS) {
            step = -step;
        }

        xtimer_periodic_wakeup(&last_wakeup, OSC_INTERVAL);
    }

    return 0;
}

int power(int argc, char** argv){
    if (argc != 3) {
        printf("usage: %s <dev> <state>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", PWM_NUMOF-1);
        printf("\tstate:\n");
        printf("\t\t0: power off\n");
        printf("\t\t1: power on\n");
        return 1;
    }

    errno = 0;
    uint32_t dev = strtol(argv[1], NULL, 10);
    if (errno != 0 || dev >= PWM_NUMOF) {
        printf("[pwm] Error: device %s is unknown.\n", argv[1]);
        return 1;
    }

    errno = 0;
    uint32_t state = strtol(argv[2], NULL, 10);
    if (errno == 0) {
        switch (state) {
            case (0):
                printf("Powering down PWM device.\n");
                pwm_poweroff(dev);
                return 0;
            case (1):
                printf("Powering up PWM device.\n");
                pwm_poweron(dev);
                return 0;
            default:
                break;
        }
    }
    printf("[pwm] Error: power state %s not available.", argv[2]);
    return 1;

}

int power_test(int argc, char** argv){
    if (argc != 2) {
        printf("usage: %s <dev>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", PWM_NUMOF-1);
        return 1;
    }

    uint32_t dev = strtol(argv[1], NULL, 10);
    if (dev >= PWM_NUMOF) {
        printf("[pwm] Error: device %ld is unknown.\n", dev);
        return 1;
    }

    printf("Powering down PWM device and sleeping for %d second(s)...\n",
           PWR_SLEEP);
    pwm_poweroff(dev);

    xtimer_sleep(PWR_SLEEP);

    printf("Powering up PWM device.\n");
    pwm_poweron(dev);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "initial pwm configuration", init },
    { "set", "set pwm duty cycle", set },
    { "power", "set pwm power", power },
    { "powertest", "test power on/off functions", power_test },
    { "osci", "blocking, default oscillation test", oscillate },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("PWM peripheral driver test\n");
    initiated = false;

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
