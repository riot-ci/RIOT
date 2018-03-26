
#include <stdio.h>
#include <avr/io.h>

#include "uart_stdio.h"

static int _uart_putchar(char c, FILE *stream);
static int _uart_getchar(FILE *stream);
static FILE _uart_stdout = FDEV_SETUP_STREAM(_uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE _uart_stdin = FDEV_SETUP_STREAM(NULL, _uart_getchar, _FDEV_SETUP_READ);

static int _uart_putchar(char c, FILE *stream)
{
    (void) stream;
    uart_stdio_write(&c, 1);
    return 0;
}

static int _uart_getchar(FILE *stream)
{
    (void) stream;
    char c;
    uart_stdio_read(&c, 1);
    return (int)c;
}

void atmega_stdio_init(void)
{
    uart_stdio_init();

    stdout = &_uart_stdout;
    stdin = &_uart_stdin;

    /* Flush stdout */
    puts("\f");
}
