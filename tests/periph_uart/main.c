/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Manual test application for UART peripheral drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "shell.h"
#include "thread.h"
#include "msg.h"
#include "ringbuffer.h"
#include "periph/uart.h"
#include "stdio_uart.h"
#include "xtimer.h"

#define SHELL_BUFSIZE       (128U)
#define UART_BUFSIZE        (128U)

#define PRINTER_PRIO        (THREAD_PRIORITY_MAIN - 1)
#define PRINTER_TYPE        (0xabcd)

#define POWEROFF_DELAY      (250U * US_PER_MS)      /* quarter of a second */

#ifndef STDIO_UART_DEV
#define STDIO_UART_DEV      (UART_UNDEF)
#endif

typedef struct {
    char rx_mem[UART_BUFSIZE];
    ringbuffer_t rx_buf;
} uart_ctx_t;

static uart_ctx_t ctx[UART_NUMOF];

static kernel_pid_t printer_pid;
static char printer_stack[THREAD_STACKSIZE_MAIN];

static int parse_dev(char *arg)
{
    unsigned dev = atoi(arg);
    if (dev >= UART_NUMOF) {
        printf("Error: Invalid UART_DEV device specified (%u).\n", dev);
        return -1;
    }
    else if (UART_DEV(dev) == STDIO_UART_DEV) {
        printf("Error: The selected UART_DEV(%u) is used for the shell!\n", dev);
        return -2;
    }
    return dev;
}

static int parse_databits(char *arg)
{
    unsigned databits = atoi(arg);
    int res;

    switch (databits) {
        case 5:
            res = UART_DATABITS_5;
            break;
        case 6:
            res = UART_DATABITS_6;
            break;
        case 7:
            res = UART_DATABITS_7;
            break;
        case 8:
            res = UART_DATABITS_8;
            break;
	default:
            printf("Error: Invalid number of databits (%u).\n", databits);
            res = -1;
    }

    return res;
}

static int parse_stopbits(char *arg)
{
    unsigned stopbits = atoi(arg);
    int res;

    switch (stopbits) {
        case 1:
            res = UART_STOPBITS_1;
            break;
        case 2:
            res = UART_STOPBITS_2;
            break;
	default:
            printf("Error: Invalid number of stopbits (%u).\n", stopbits);
            res = -1;
    }

    return res;
}

static int parse_parity(char *arg)
{
    int res;

    switch (arg[0]) {
        case 'n':
        case 'N':
            res = UART_PARITY_NONE;
            break;
        case 'e':
        case 'E':
            res = UART_PARITY_EVEN;
            break;
        case 'o':
        case 'O':
            res = UART_PARITY_ODD;
            break;
        case 'm':
        case 'M':
            res = UART_PARITY_MARK;
            break;
        case 's':
        case 'S':
            res = UART_PARITY_SPACE;
            break;
	default:
            printf("Error: Invalid parity (%c).\n", arg[0]);
            res = -1;
    }

    return res;
}

static void rx_cb(void *arg, uint8_t data)
{
    uart_t dev = (uart_t)arg;

    ringbuffer_add_one(&(ctx[dev].rx_buf), data);
    if (data == '\n') {
        msg_t msg;
        msg.content.value = (uint32_t)dev;
        msg_send(&msg, printer_pid);
    }
}

static void *printer(void *arg)
{
    (void)arg;
    msg_t msg;
    msg_t msg_queue[8];
    msg_init_queue(msg_queue, 8);

    while (1) {
        msg_receive(&msg);
        uart_t dev = (uart_t)msg.content.value;
        char c;

        printf("Success: UART_DEV(%i) RX: [", dev);
        do {
            c = (int)ringbuffer_get_one(&(ctx[dev].rx_buf));
            if (c == '\n') {
                puts("]\\n");
            }
            else if (c >= ' ' && c <= '~') {
                printf("%c", c);
            }
            else {
                printf("0x%02x", (unsigned char)c);
            }
        } while (c != '\n');
    }

    /* this should never be reached */
    return NULL;
}

static void sleep_test(int num, uart_t uart)
{
    printf("UARD_DEV(%i): test uart_poweron() and uart_poweroff()  ->  ", num);
    uart_poweroff(uart);
    xtimer_usleep(POWEROFF_DELAY);
    uart_poweron(uart);
    puts("[OK]");
}

static int cmd_init(int argc, char **argv)
{
    int dev, res;
    uint32_t baud;

    if (argc < 3) {
        printf("usage: %s <dev> <baudrate>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }
    baud = strtol(argv[2], NULL, 0);

    /* initialize UART */
    res = uart_init(UART_DEV(dev), baud, rx_cb, (void *)dev);
    if (res == UART_NOBAUD) {
        printf("Error: Given baudrate (%u) not possible\n", (unsigned int)baud);
        return 1;
    }
    else if (res != UART_OK) {
        puts("Error: Unable to initialize UART device\n");
        return 1;
    }
    printf("Success: Initialized UART_DEV(%i) at BAUD %"PRIu32"\n", dev, baud);

    /* also test if poweron() and poweroff() work (or at least don't break
     * anything) */
    sleep_test(dev, UART_DEV(dev));

    return 0;
}

static int cmd_mode(int argc, char **argv)
{
    int dev, res;
    int databits, stopbits, parity;

    if (argc < 5) {
        printf("usage: %s <dev> <databits> <parity> <stopbits>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }
    databits = parse_databits(argv[2]);
    if (databits < 0) {
        return 1;
    }
    parity = parse_parity(argv[3]);
    if (parity < 0) {
        return 1;
    }
    stopbits = parse_stopbits(argv[4]);
    if (stopbits < 0) {
        return 1;
    }
    
    res = uart_mode(dev, databits, parity, stopbits);
    if (res != UART_OK) {
        puts("Error: Unable to apply UART settings\n");
        return 1;
    }
    printf("Success: Successfully applied UART_DEV(%i) settings\n", dev);

    return 0;
}

static int cmd_send(int argc, char **argv)
{
    int dev;
    uint8_t endline = (uint8_t)'\n';

    if (argc < 3) {
        printf("usage: %s <dev> <data (string)>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }

    printf("UART_DEV(%i) TX: %s\n", dev, argv[2]);
    uart_write(UART_DEV(dev), (uint8_t *)argv[2], strlen(argv[2]));
    uart_write(UART_DEV(dev), &endline, 1);
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "Initialize a UART device with a given baudrate", cmd_init },
    { "mode", "Setup databits, stopbits and parity for a given UART device", cmd_mode },
    { "send", "Send a string through given UART device", cmd_send },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\nManual UART driver test application");
    puts("===================================");
    puts("This application is intended for testing additional UART\n"
         "interfaces, that might be defined for a board. The 'primary' UART\n"
         "interface is tested implicitly, as it is running the shell...\n\n"
         "When receiving data on one of the additional UART interfaces, this\n"
         "data will be outputted via STDIO. So the easiest way to test an \n"
         "UART interface, is to simply connect the RX with the TX pin. Then \n"
         "you can send data on that interface and you should see the data \n"
         "being printed to STDOUT\n\n"
         "NOTE: all strings need to be '\\n' terminated!\n");

    /* do sleep test for UART used as STDIO. There is a possibility that the
     * value given in STDIO_UART_DEV is not a numeral (depends on the CPU
     * implementation), so we rather break the output by printing a
     * non-numerical value instead of breaking the UART device descriptor */
    sleep_test(STDIO_UART_DEV, STDIO_UART_DEV);

    puts("\nUART INFO:");
    printf("Available devices:               %i\n", UART_NUMOF);
    printf("UART used for STDIO (the shell): UART_DEV(%i)\n\n", STDIO_UART_DEV);

    /* initialize ringbuffers */
    for (unsigned i = 0; i < UART_NUMOF; i++) {
        ringbuffer_init(&(ctx[i].rx_buf), ctx[i].rx_mem, UART_BUFSIZE);
    }

    /* start the printer thread */
    printer_pid = thread_create(printer_stack, sizeof(printer_stack),
                                PRINTER_PRIO, 0, printer, NULL, "printer");

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
