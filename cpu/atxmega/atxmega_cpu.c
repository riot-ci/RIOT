/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2017 RWTH Aachen, Josua Arndt
 *               2018 Matthew Blue
 *               2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>

 * @}
 */

#include <avr/pgmspace.h>

#include "cpu.h"
#include "panic.h"

#define ENABLE_DEBUG 0
#include "debug.h"

extern uint8_t mcusr_mirror;

void avr8_reset_cause(void)
{
    if (mcusr_mirror & (1 << RST_PORF_bp)) {
        DEBUG("Power-on reset.\n");
    }
    if (mcusr_mirror & (1 << RST_EXTRF_bp)) {
        DEBUG("External reset!\n");
    }
    if (mcusr_mirror & (1 << RST_BORF_bp)) {
        DEBUG("Brown-out reset!\n");
    }
    if (mcusr_mirror & (1 << RST_WDRF_bp)) {
        DEBUG("Watchdog reset!\n");
    }
    if (mcusr_mirror & (1 << RST_PDIRF_bp)) {
        DEBUG("Programming and Debug Interface reset!\n");
    }
    if (mcusr_mirror & (1 << RST_SRF_bp)) {
        DEBUG("Software reset!\n");
    }
    if (mcusr_mirror & (1 << RST_SDRF_bp)) {
        DEBUG("Spike Detection reset!\n");
    }
}

/* This is a vector which is aliased to __vector_default,
 * the vector executed when an ISR fires with no accompanying
 * ISR handler. This may be used along with the ISR() macro to
 * create a catch-all for undefined but used ISRs for debugging
 * purposes.
 */
ISR(BADISR_vect)
{
    avr8_reset_cause();

#ifdef LED_PANIC
    /* Use LED light to signal ERROR. */
    LED_PANIC;
#endif

    core_panic(PANIC_GENERAL_ERROR,
               PSTR("FATAL ERROR: BADISR_vect called, unprocessed Interrupt.\n"
                    "STOP Execution.\n"));
}
