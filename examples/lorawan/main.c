/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example demonstrating the use of LoRaWAN with RIOT
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "msg.h"
#include "thread.h"
#include "fmt.h"
#include "pm_layered.h"

#include "periph/rtc.h"
#include "periph/pm.h"

#include "net/loramac.h"
#include "semtech_loramac.h"
#include "sx127x.h"
#include "sx127x_params.h"

/* Use the STOP mode to ensure memory retention between each send */
#define PM_MODE             (1)

/* Messages are sent every 20s to respect the duty cycle on each channel */
#define PERIOD              (20U)

#define SENDER_PRIO         (THREAD_PRIORITY_MAIN - 1)
static kernel_pid_t sender_pid;
static char sender_stack[THREAD_STACKSIZE_MAIN / 2];

sx127x_t sx127x;

static const char *message = "This is RIOT!";

static uint8_t deveui[LORAMAC_DEVEUI_LEN];
static uint8_t appeui[LORAMAC_APPEUI_LEN];
static uint8_t appkey[LORAMAC_APPKEY_LEN];

static void rtc_cb(void *arg)
{
    (void) arg;
    msg_t msg;
    msg_send(&msg, sender_pid);
}

static void _prepare_next_alarm(void)
{
    struct tm time;
    rtc_get_time(&time);
    /* set initial alarm */
    time.tm_sec += PERIOD;
    while (time.tm_sec > 60) {
        time.tm_min++;
        time.tm_sec -= 60;
    }
    while (time.tm_min > 60) {
        time.tm_hour++;
        time.tm_min -= 60;
    }
    rtc_set_alarm(&time, rtc_cb, NULL);
}

static void _send_message(void)
{
    printf("Sending: %s\n", message);
    semtech_loramac_rx_data_t rx_data;
    /* The send call blocks until done */
    semtech_loramac_send(LORAMAC_TX_CNF, 10,
                         (uint8_t *)message, strlen(message), &rx_data);
}

static void *sender(void *arg)
{
    (void)arg;

    msg_t msg;
    msg_t msg_queue[8];
    msg_init_queue(msg_queue, 8);

    while (1) {
        msg_receive(&msg);
        /* Ensure the low-power mode is blocked during send phase */
        pm_block(PM_MODE);

        /* Trigger the message send */
        _send_message();

        /* Schedule the next wake-up alarm */
        _prepare_next_alarm();

        /* Unblock the low-power mode now => the system fall to idle thread
           that sets the low-power mode. */
        pm_unblock(PM_MODE);
    }

    /* this should never be reached */
    return NULL;
}

int main(void)
{
    /* Ensure the low-power mode is blocked during initialization phase */
    pm_block(PM_MODE);

    puts("LoRaWAN Class A low-power application");
    puts("=====================================");

    /* Convert identifiers and application key */
    fmt_hex_bytes(deveui, DEVEUI);
    fmt_hex_bytes(appeui, APPEUI);
    fmt_hex_bytes(appkey, APPKEY);

    /* Initialize the loramac stack */
    sx127x_setup(&sx127x, &sx127x_params[0]);
    semtech_loramac_init(&sx127x);
    semtech_loramac_set_deveui(deveui);
    semtech_loramac_set_appeui(appeui);
    semtech_loramac_set_appkey(appkey);

    xtimer_sleep(1);
    semtech_loramac_set_dr(LORAMAC_DR_5);
    xtimer_sleep(1);

    /* Start the Over-The-Air Activation (OTAA) procedure to retrieve the
     * generated device address and to get the network and application session
     * keys.
     */
    puts("Starting join procedure");
    if (semtech_loramac_join(LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed");
        return 1;
    }
    puts("Join procedure succeeded");

    /* start the sender thread */
    sender_pid = thread_create(sender_stack, sizeof(sender_stack),
                               SENDER_PRIO, 0, sender, NULL, "sender");

    /* trigger the first send */
    msg_t msg;
    msg_send(&msg, sender_pid);
    return 0;
}
