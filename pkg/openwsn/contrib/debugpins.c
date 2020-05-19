/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption of the "debugpins" bsp module
 *
 * @author      Michael Frey <michael.frey@msasafety.com>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "debugpins.h"
#include "openwsn_debugpins.h"

#include <stdint.h>
#include <string.h>

static _set_checked(gpio_t pin) {
    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        if (pin != GPIO_UNDEF){
            gpio_set(pin);
        }
    }
}

static _clear_checked(gpio_t pin) {
    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        if (pin != GPIO_UNDEF){
            gpio_clear(pin);
        }
    }
}

static _toggle_checked(gpio_t pin) {
    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        if (pin != GPIO_UNDEF){
            gpio_toggle(pin);
        }
    }
}

static _init_checked(gpio_t pin) {
    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        if (pin != GPIO_UNDEF){
            gpio_init(configuration.frame, GPIO_OUT);
        }
    }
}

/* holds the internal configuration for debugpins */
static debugpins_config_t _configuration = {
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF
};

void openwsn_debugpins_init(const debugpins_config_t *user_config)
{
    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        memset(&configuration, GPIO_UNDEF, sizeof(debugpins_config_t));

        if (user_config != NULL) {
            memcpy(&configuration, user_config, sizeof(debugpins_config_t));
            debugpins_init();
        }
    }
    else {
        (void) user_config;
    }
}

void debugpins_init(void)
{
    _init_checked(configuration.frame);
    _init_checked(configuration.slot);
    _init_checked(configuration.fsm);
    _init_checked(configuration.task);
    _init_checked(configuration.isr);
    _init_checked(configuration.radio);

    debugpins_frame_clr();
    debugpins_slot_clr();
    debugpins_fsm_clr();
    debugpins_task_clr();
    debugpins_isr_clr();
    debugpins_radio_clr();
}

void debugpins_frame_toggle(void)
{
    _toggle_checked(configuration.frame);
}

void debugpins_frame_clr(void)
{
    _clear_checked(configuration.frame);
}

void debugpins_frame_set(void)
{
    _set_checked(configuration.frame);
}

void debugpins_slot_toggle(void)
{
    _toggle_checked(configuration.slot);
}

void debugpins_slot_clr(void)
{
    _clear_checked(configuration.slot);
}

void debugpins_slot_set(void)
{
    _set_checked(configuration.slot);
}

void debugpins_fsm_toggle(void)
{
    _toggle_checked(configuration.fsm);
}

void debugpins_fsm_clr(void)
{
    _clear_checked(configuration.fsm);
}

void debugpins_fsm_set(void)
{
    _set_checked(configuration.fsm);
}

void debugpins_task_toggle(void)
{
    _toggle_checked(configuration.task);
}

void debugpins_task_clr(void)
{
    _clear_checked(configuration.task);
}

void debugpins_task_set(void)
{
    _set_checked(configuration.task);

void debugpins_isr_toggle(void)
{
    _toggle_checked(configuration.isr);
}

void debugpins_isr_clr(void)
{
    _clear_checked(configuration.isr);
}

void debugpins_isr_set(void)
{
    _set_checked(configuration.isr);
}

void debugpins_radio_toggle(void)
{
    _toggle_checked(configuration.radio);
}

void debugpins_radio_clr(void)
{
    _clear_checked(configuration.radio);
}

void debugpins_radio_set(void)
{
    _set_checked(configuration.radio);
}
