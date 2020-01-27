/*
 * Copyright (C) 2019 Nalys
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
 * @brief       Test application for the candev abstraction
 *
 * @author      Toon Stegen <tstegen@nlys-group.com>
 *
 * @}
 */

#include <debug.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "can/device.h"

#ifdef CAN_DRIVER_MCP2515
#include "candev_mcp2515.h"
candev_mcp2515_conf_t mcp2515_conf = {
    .spi = SPI_DEV(0),
    .spi_mode = SPI_MODE_0,
    .spi_clk = SPI_CLK_10MHZ,
    .cs_pin = SPI_HWCS(0),
    .rst_pin = GPIO_PIN(0, 0),
    .int_pin = GPIO_PIN(0, 1),
    .clk = (8000000ul),
};

candev_mcp2515_t mcp2515_dev = { 0 };
#else
    /* add other candev drivers here */
#endif

static candev_t *candev = NULL;

static int _send(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    struct can_frame frame = {
        .can_id = 1,
        .can_dlc = 2,
        .data[0] = 255,
        .data[1] = 50
    };

    /* add sending here */
    (void) frame;
    candev->driver->send(candev);
    return 0;
}

static int _receive(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    /* add receiving here */
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "send", "send some data", _send },
    { "receive", "receive some data", _receive },
    { NULL, NULL, NULL }
};

static void _can_event_callback(candev_t *dev, candev_event_t event, void *arg)
{
    (void) arg;

    switch (event) {
    case CANDEV_EVENT_ISR:
        DEBUG("_can_event: CANDEV_EVENT_ISR\n");
        break;
    case CANDEV_EVENT_WAKE_UP:
        DEBUG("_can_event: CANDEV_EVENT_WAKE_UP\n");
        break;
    case CANDEV_EVENT_TX_CONFIRMATION:
        DEBUG("_can_event: CANDEV_EVENT_TX_CONFIRMATION\n");
        break;
    case CANDEV_EVENT_TX_ERROR:
        DEBUG("_can_event: CANDEV_EVENT_TX_ERROR\n");
        break;
    case CANDEV_EVENT_RX_INDICATION:
        DEBUG("_can_event: CANDEV_EVENT_RX_INDICATION\n");
        break;
    case CANDEV_EVENT_RX_ERROR:
        DEBUG("_can_event: CANDEV_EVENT_RX_ERROR\n");
        break;
    case CANDEV_EVENT_BUS_OFF:
        dev->state = CAN_STATE_BUS_OFF;
        break;
    case CANDEV_EVENT_ERROR_PASSIVE:
        dev->state = CAN_STATE_ERROR_PASSIVE;
        break;
    case CANDEV_EVENT_ERROR_WARNING:
        dev->state = CAN_STATE_ERROR_WARNING;
        break;
    default:
        DEBUG("_can_event: unknown event\n");
        break;
    }
}

int main(void)
{
    puts("candev test application\n");

#ifdef CAN_DRIVER_MCP2515
    printf("Initializing MCP2515");
    candev_mcp2515_init(&mcp2515_dev, &mcp2515_conf);
    candev = (candev_t *)&mcp2515_dev;
#else
    /* add initialization for other candev drivers here */
#endif

    assert(candev);

    candev->event_callback = _can_event_callback;
    candev->isr_arg = NULL;

    candev->driver->init(candev);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
