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

#include <avr/interrupt.h>
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
    ARDUINO_PIN_21,  ARDUINO_PIN_20,
#ifdef INT2_vect
    ARDUINO_PIN_19,
#endif
#ifdef INT3_vect
    ARDUINO_PIN_18,
#endif
#ifdef INT4_vect
    ARDUINO_PIN_2,
#endif
#ifdef INT5_vect
    ARDUINO_PIN_3,
#endif
    ARDUINO_PIN_53,      ARDUINO_PIN_52,
    ARDUINO_PIN_51,      ARDUINO_PIN_50,
    ARDUINO_PIN_10,      ARDUINO_PIN_11,
    ARDUINO_PIN_12,      ARDUINO_PIN_13,
    ARDUINO_PIN_15,      ARDUINO_PIN_14,
    GPIO_PIN(PORT_J, 2), GPIO_PIN(PORT_J, 3),
    GPIO_PIN(PORT_J, 4), GPIO_PIN(PORT_J, 5),
    GPIO_PIN(PORT_J, 6), ARDUINO_PIN_A8,
    ARDUINO_PIN_A9,      ARDUINO_PIN_A10,
    ARDUINO_PIN_A11,     ARDUINO_PIN_A12,
    ARDUINO_PIN_A13,     ARDUINO_PIN_A14,
    ARDUINO_PIN_A15,
};

static const char *pin_names[] = {
    "21/PD0", "20/PD1",
#ifdef INT2_vect
    "19/PD2",
#endif
#ifdef INT3_vect
    "18/PD3",
#endif
#ifdef INT4_vect
    "2/PE4",
#endif
#ifdef INT5_vect
    "3/PE5",
#endif
    "53/PB0",  "52/PB1",  "51/PB2",  "50/PB3",
    "10/PB4",  "11/PB5",  "12/PB6",  "13/PB7",
    "15/PJ0",  "14/PJ1",  "--/PJ2",  "--/PJ3",
    "--/PJ4",  "--/PJ5",  "--/PJ6",  "A8/PK0",
    "A9/PK1",  "A10/PK2", "A11/PK3", "A12/PK4",
    "A13/PK5", "A14/PK6", "A15/PK7",
};


static const char *irq_types[] = {
    "INT0",    "INT1",
#ifdef INT2_vect
    "INT2",
#endif
#ifdef INT3_vect
    "INT3",
#endif
#ifdef INT4_vect
    "INT4",
#endif
#ifdef INT5_vect
    "INT5",
#endif
    "PCINT0",  "PCINT1",  "PCINT2",  "PCINT3",
    "PCINT4",  "PCINT5",  "PCINT6",  "PCINT7",
    "PCINT9",  "PCINT10", "PCINT11", "PCINT12",
    "PCINT13", "PCINT14", "PCINT15", "PCINT16",
    "PCINT17", "PCINT18", "PCINT19", "PCINT20",
    "PCINT21", "PCINT22", "PCINT23",
};

#else
#error "Board not supported in this test"
#endif

#define PINS_NUMOF          (sizeof(pins) / sizeof(pins[0]))
#define PIN_NAMES_NUMOF     (sizeof(pin_names) / sizeof(pin_names[0]))
#define IRQ_TYPES_NUMOF     (sizeof(irq_types) / sizeof(irq_types[0]))

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
    assert((PINS_NUMOF == PIN_NAMES_NUMOF) && (PINS_NUMOF == IRQ_TYPES_NUMOF));

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
