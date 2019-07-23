/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Test application for GPIO interrupts for the Arduino Uno
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "periph/gpio.h"
#include "arduino_pinmap.h"

#ifdef BOARD_ARDUINO_UNO
static const gpio_t pins[] = {
    ARDUINO_PIN_2,  ARDUINO_PIN_3,  ARDUINO_PIN_4,  ARDUINO_PIN_5,
    ARDUINO_PIN_6,  ARDUINO_PIN_7,  ARDUINO_PIN_8,  ARDUINO_PIN_9,
    ARDUINO_PIN_10, ARDUINO_PIN_11, ARDUINO_PIN_12, ARDUINO_PIN_13,
    ARDUINO_PIN_A0, ARDUINO_PIN_A1, ARDUINO_PIN_A2, ARDUINO_PIN_A3,
    ARDUINO_PIN_A4, ARDUINO_PIN_A5,
};

static const char *pin_names[] = {
    "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",
    "10", "11", "12", "13", "A0", "A1", "A2", "A3",
    "A4", "A5",
};

static const char *irq_types[] = {
    "INT0",    "INT1",    "PCINT20", "PCINT21",
    "PCINT22", "PCINT23", "PCINT0",  "PCINT1",
    "PCINT2",  "PCINT3",  "PCINT4",  "PCINT5",
    "PCINT8",  "PCINT9",  "PCINT10", "PCINT11",
    "PCINT12", "PCINT13",
};
#elif defined(BOARD_ARDUINO_MEGA2560)
static const gpio_t pins[] = {
    ARDUINO_PIN_2,  ARDUINO_PIN_3,  ARDUINO_PIN_5,  ARDUINO_PIN_6,
    ARDUINO_PIN_7,  ARDUINO_PIN_8,  ARDUINO_PIN_9,  ARDUINO_PIN_10,
    ARDUINO_PIN_11, ARDUINO_PIN_12, ARDUINO_PIN_13, ARDUINO_PIN_A0,
    ARDUINO_PIN_A1, ARDUINO_PIN_A2, ARDUINO_PIN_A3, ARDUINO_PIN_A4,
    ARDUINO_PIN_A5,
};

static const char *pin_names[] = {
    "21", "20", "19", "18",
    "2",  "3",  "53", "52",
    "51", "50", "10", "11",
    "12", "13", "14", "15",
    "62", "63", "64", "65",
    "66", "67", "68", "69",
};

static const char *irq_types[] = {
    "INT0",    "INT1",    "INT2",    "INT3",
    "INT4",    "INT5",    "PCINT0",  "PCINT1",
    "PCINT2",  "PCINT3",  "PCINT4",  "PCINT5",
    "PCINT6",  "PCINT7",  "PCINT10", "PCINT11",
    "PCINT16", "PCINT17", "PCINT18", "PCINT19",
    "PCINT20", "PCINT21", "PCINT22", "PCINT23",
};

#else
#error "Board not supported in this test"
#endif

static const gpio_flank_t flanks[] = {
    GPIO_FALLING, GPIO_RISING, GPIO_BOTH,
};

static const char *flank_names[] = {
    "falling", "rising", "falling and rising",
};

#define PINS_NUMOF      (sizeof(pins) / sizeof(pins[0]))
#define FLANKS_NUMOF    (sizeof(flanks) / sizeof(flanks[0]))

static void callback(void *arg)
{
    unsigned idx = (unsigned)arg;
    const char *pin = pin_names[idx];
    const char *type = irq_types[idx];
    const char *flank = flank_names[idx % FLANKS_NUMOF];

    printf("INT: Pin = %s; IRQ type = %s; flank(s) = %s\n", pin, type, flank);
}

int main(void)
{
    puts(
        "GPIO interrupt test for Arduino UNO / Arudino Mega2560\n"
        "\n"
        "For all pins supporting IRQ (except RX/TX pins) interrupts will be\n"
        "configured, which are triggered on falling edge, on rising edge, and\n"
        "on both edges (in turns). Pull the pins against ground one by one\n"
        "and check if the console output matches your actions.\n"
        "\n"
        "Installing interrupts...");
    for (unsigned i = 0; i < PINS_NUMOF; i++) {
        unsigned f = i % FLANKS_NUMOF;
        printf("Pin = %s; Flank = %s; IRQ = %s\n",
               pin_names[i], flank_names[f], irq_types[i]);
        if (gpio_init_int(pins[i], GPIO_IN_PU, flanks[i % FLANKS_NUMOF], callback,
                          (void *)i)) {
            puts("gpio_init_int() failed\n");
        }
    }


    return 0;
}
