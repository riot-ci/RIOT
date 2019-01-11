/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Test application for the LTC4150 coulomb counter driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "fmt.h"
#include "led.h"
#include "ltc4150.h"
#include "thread.h"
#include "xtimer.h"

typedef struct {
    uint64_t last_usec;
    uint64_t now_usec;
    ltc4150_dir_t dir;
} test_recorder_data_t;

static void pulse_cb(ltc4150_dev_t *, ltc4150_dir_t, uint64_t, void *);
static void reset_cb(ltc4150_dev_t *, uint64_t, void *);

static ltc4150_last_minute_data_t last_minute_data;
static test_recorder_data_t test_data;
static const ltc4150_recorder_t test_recorder = {
    .pulse = pulse_cb,
    .reset = reset_cb,
};
static kernel_pid_t target_pid;
static char busy_thread_stack[THREAD_STACKSIZE_DEFAULT];
static ltc4150_dev_t ltc4150;

static const ltc4150_recorder_t *recorders[] = {
    &ltc4150_last_minute,
    &test_recorder,
    NULL
};
static void *recorder_data[] = {
    &last_minute_data,
    &test_data,
};

#define LTC4150_PARAM_RECS            (recorders)
#define LTC4150_PARAM_RECDATA         (recorder_data)

#include "ltc4150_params.h"

/**
 * @brief Callback function to reset/initialize the recorder data
 */
static void reset_cb(ltc4150_dev_t *dev, uint64_t now_usec, void *_data)
{
    (void)dev;
    test_recorder_data_t *data = _data;
    data->last_usec = data->now_usec = now_usec;
    data->dir = LTC4150_DISCHARGE;
}

/**
 * @brief Callback function to record the current pulse
 */
static void pulse_cb(ltc4150_dev_t *dev, ltc4150_dir_t dir, uint64_t now_usec,
                     void *_data)
{
    (void)dev;
    static msg_t m = { .content = { .value = 0} };

    test_recorder_data_t *data = _data;
    data->last_usec = data->now_usec;
    data->now_usec = now_usec;
    data->dir = dir;

    msg_send(&m, target_pid);
}

/**
 * @brief Busy waits for the given amount of seconds
 * @param seconds    Number of seconds to roast the CPU
 */
static void spin(uint32_t seconds)
{
    uint32_t till = xtimer_now_usec() + US_PER_SEC * seconds;
    while (xtimer_now_usec() < till) { }
}

/**
 * @brief Thread that will put three levels of CPU load on the MCU
 */
static void *busy_thread(void *arg)
{
    (void)arg;
    while (1) {
        /* one minute of ~0% CPU usage */
        LED0_OFF;
        LED1_OFF;
        xtimer_sleep(60);

        /* one minute of ~50% CPU usage */
        for (unsigned i = 0; i < 30; i++) {
            LED0_OFF;
            LED1_OFF;
            xtimer_sleep(1);
            LED0_ON;
            LED1_ON;
            spin(1);
        }

        /* one minute of 100% CPU usage */
        LED0_ON;
        LED1_ON;
        spin(60);
    }

    /* unreachable */
    return NULL;
}

/**
 * @brief Print the given number of spaces
 */
static void print_spaces(size_t number)
{
    static const char *spaces = "                ";
    while (number > 16) {
        fputs(spaces, stdout);
        number -= 16;
    }

    fputs(spaces + 16 - number, stdout);
}

/**
 * @brief Print a table column with the given number as decimal
 * @param number    Number to print in the column
 * @param width     Width of the column
 */
static void print_col_u32(uint32_t number, size_t width)
{
    char sbuf[32];
    size_t slen;

    slen = fmt_u32_dec(sbuf, number);
    sbuf[slen] = '\0';
    if (width > slen) {
        print_spaces(width - slen);
    }
    fputs(sbuf, stdout);
}

/**
 * @brief Print a table column with the given number as decimal
 * @param number    Number to print in the column
 * @param width     Width of the column
 */
static void print_col_i32(int32_t number, size_t width)
{
    char sbuf[32];
    size_t slen;
    char *pos = sbuf;

    if (number < 0) {
        *pos++ = '-';
        number = -number;
        width--;
    }
    slen = fmt_u32_dec(sbuf, (uint32_t)number);
    sbuf[slen] = '\0';
    if (width > slen) {
        print_spaces(width - slen);
    }
    fputs(sbuf, stdout);
}

/**
 * @brief Print a table column with the given current as E-01
 * @param current   Value to print in the column (as E-01)
 * @param width     Width of the column
 */
static void print_current(int32_t current, size_t width)
{
    char sbuf[3];

    print_col_i32(current/10, width - 2);
    sbuf[0] = '.';
    sbuf[1] = '0' + current % 10;
    sbuf[2] = '\0';
    fputs(sbuf, stdout);
}

int main(void)
{
    target_pid = thread_getpid();
    uint32_t ten_uc_per_pulse;
    msg_t m;
    int retval;

    retval = ltc4150_init(&ltc4150, &ltc4150_params[0]);

    /* Pre-compute the charge corresponding to one pulse */
    ltc4150_pulses2c(&ltc4150, &ten_uc_per_pulse, NULL, 10000, 0);

    if (retval) {
        fputs("Failed to initialize LTC4150 driver:", stdout);
        switch (retval) {
            case -EINVAL:
                puts("Invalid parameter");
                break;
            case -EIO:
                puts("GPIO or interrupt configuration failed");
                break;
            default:
                puts("Unknown (should no happen, file a bug)");
                break;
        }
        return -1;
    }

    /* Start the thread that will keep the MCU busy */
    thread_create(busy_thread_stack, sizeof(busy_thread_stack),
                  THREAD_PRIORITY_MAIN + 1, THREAD_CREATE_STACKTEST,
                  busy_thread, NULL, "busy_thread");

    puts("This test will put three levels of load on the MCU:\n"
         " 1. One minute of little to no load (LEDs off)\n"
         " 2. One minute of about 50% CPU load (LEDs blinking)\n"
         " 3. One minute of 100% CPU load (LEDs constantly on)\n"
         "\n"
         "During this time the charge drawn is measured and printed on every\n"
         "pulse the LTC4150 generates. The expected result is that the\n"
         "required current increases with the system load");

    LED0_OFF;

    puts("+-------------------------------+-----------------------------------+\n"
         "| Total Transferred Charge [mC] |   Current from Power Supply [mA]  |\n"
         "| Charging      | Discharging   | Average | Last Minute | Currently |\n"
         "+---------------+---------------+---------+-------------+-----------+");

    while (1) {
        /* Wait for the next pulse of the LTC4150 */
        msg_receive(&m);
        uint32_t charged, discharged;
        int16_t avg_current;
        int32_t current;

        /* Get & print total charge transferred */
        if (ltc4150_charge(&ltc4150, &charged, &discharged)) {
            puts("ltc4150_charge() failed!");
            return -1;
        }
        fputs("| ", stdout);
        print_col_u32(discharged, 13);
        fputs(" | ", stdout);
        print_col_u32(charged, 13);
        fputs(" | ", stdout);

        /* Get & print avg current */
        if (ltc4150_avg_current(&ltc4150, &avg_current)) {
            puts("ltc4150_avg_current() failed!");
            return -1;
        }
        print_current(avg_current, 7);
        fputs(" | ", stdout);

        /* Get & print last minute current */
        if (ltc4150_last_minute_charge(&ltc4150, &last_minute_data,
                                       &charged, &discharged)
            ) {
            puts("ltc4150_last_minute_charge() failed!");
            return -1;
        }
        current = (int32_t)discharged - (int32_t)charged;
        current /= 60;
        print_col_i32(current, 11);
        fputs(" | ", stdout);

        /* Calculate & print the current between the last two pulses */
        current = (int32_t)((test_data.now_usec - test_data.last_usec) / MS_PER_SEC);
        current = ten_uc_per_pulse / current;
        if (test_data.dir == LTC4150_CHARGE) {
            current = -current;
        }
        print_current(current, 9);
        puts(" |");
    }

    return 0;
}
