/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2/CC13x2 RF Core common functions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "cpu.h"
#include "cc26x2_cc13x2_rfc.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/**
 * @brief   Driver FSM states
 */
typedef enum {
    FSM_INVALID = 0, /**< Invalid state, an error happened */
    FSM_POWER_UP, /**< Power up state, not ready to receive commands */
    FSM_ON, /**< RF Core is on and ready to receive commands */
    FSM_OFF, /**< RF Core is off */
} fsm_state_t;

static void rfc_power_on(void);
static void rfc_power_off(void);
static void fsm_power_up(void);
static void fsm_on(void);

static rfc_cmd_sync_start_rat_t _start_rat = {
    .commandNo = CMD_SYNC_START_RAT,
    .status = IDLE, /* set by RF Core */
    .pNextOp = NULL, /* set by us */
    .startTime = 0,
    .startTrigger.triggerType = TRIG_NOW,
    .startTrigger.bEnaCmd = 0,
    .startTrigger.triggerNo = 0,
    .startTrigger.pastTrig =0,
    .condition.rule = COND_STOP_ON_FALSE,
    .condition.nSkip = 0,
    .__dummy0 = 0,
    .rat0 = 0, /* set by us */
};

static rfc_radio_setup_t *_radio_setup; /**< PHY radio setup command */
static fsm_state_t _state; /**< FSM current state */
static bool _switch_sclk_hf; /**< Switch SCLK_HF to XOSC_HF? */
static ratmr_t _rat_offset; /**< Saved RAT offset */

void rfc_init(rfc_radio_setup_t *radio_setup)
{
    DEBUG("rfc_init(%08lx)\n", (uint32_t)radio_setup);

    _radio_setup = radio_setup;
    _state = FSM_OFF;
    _switch_sclk_hf = false;
    _rat_offset = 0;

    /* Log out the available modes, just to be sure */
    DEBUG("rfc_init: available RF modes = %08lx\n", PRCM->RFCMODEHWOPT);

    /* We don't use the modes on PRCM->RFCMODEHWOPT since that is not documented,
     * writing a 0 means to select the mode automatically. On cc13x0 is needed
     * to write one of the valid modes specified at PRCM->RFCMODEHWOPT */
    PRCM->RFCMODESEL = 0;
}

int rfc_enable(void)
{
    DEBUG("rfc_enable()\n");

    assert(_state == FSM_OFF);

    /* Request to switch to the crystal to enable radio operation. It takes
     * a while for the XTAL to be ready. This doesn't make the switch, it can
     * be done later when the XOSC_HF is stabilized */
    if (osc_clock_source_get(OSC_SRC_CLK_HF) != OSC_XOSC_HF) {
        _switch_sclk_hf = true;
        DEBUG("rfc_init: request switch of SCLK_HF to use OSC_XOSC_HF\n");
        osc_clock_source_set(OSC_SRC_CLK_HF, OSC_XOSC_HF);
    }

    /* Power on RF Core */
    rfc_power_on();

    return 0;
}

void rfc_disable(void)
{
    /* TODO: stop rat, synthesizer, etc */
    rfc_power_off();
}

/**
 * @brief   Turns on the radio core.
 *
 *  - Switches the high frequency clock to the xosc crystal on
 *  CC26X2/CC13X2.
 *  - Powers on the radio core power domain
 *  - Enables the radio core power domain
 */
static void rfc_power_on(void)
{
    DEBUG("rfc_power_on()\n");

    unsigned key = irq_disable();

    /* Enable RF Core power domain */
    PRCM->PDCTL0RFC = 1;
    PRCM->PDCTL1RFC = 1;
    while (PRCM->PDSTAT1RFC != 1 && PRCM->PDSTAT0RFC != 1) {}

    DEBUG("rfc_power_on: domain enabled\n");

    /* Enable RF Core clocks */
    PRCM->RFCCLKG |= RFCCLKG_CLK_EN;
    PRCM->CLKLOADCTL |= CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) {}

    /* Disable and remove previous interrupts (if any) */
    rfc_cpe_int_clear(0xFFFFFFFF);
    rfc_cpe_int_enable_clear();

    /* Select all interrupts for RF_CPE0_IRQN, and IRQ_INTERNAL_ERROR for
     * RF_CPE1_IRQN, don't change order! */
    rfc_cpe0_int_select(0xFFFFFFFF);
    rfc_cpe1_int_select(IRQ_INTERNAL_ERROR);

    NVIC_ClearPendingIRQ(RF_CPE0_IRQN);
    NVIC_ClearPendingIRQ(RF_CPE1_IRQN);
    NVIC_EnableIRQ(RF_CPE0_IRQN);
    NVIC_EnableIRQ(RF_CPE1_IRQN);

    rfc_cpe_int_enable(IRQ_BOOT_DONE | IRQ_MODULES_UNLOCKED);

    _state = FSM_POWER_UP;

    /* Let CPE boot */
    DEBUG("rfc_power_on: booting RF Core\n");
    rfc_clock_enable();

    irq_restore(key);
}

/**
 * @brief   Turns off the radio core.
 *
 * - Switches off the power and resources for the radio core.
 * - Disables the interrupts
 * - Disables the radio core power domain
 * - Powers off the radio core power domain
 * - Switches the high frequency clock to the RCOSC to save power
 */
static void rfc_power_off(void)
{
    DEBUG("rfc_power_off()\n");

    unsigned key = irq_disable();

    /* Disable RF Core clocks */
    PRCM->RFCCLKG &= ~RFCCLKG_CLK_EN;
    PRCM->CLKLOADCTL |= CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) {}

    /* Power off RF Core domain */
    PRCM->PDCTL0RFC = 0;
    PRCM->PDCTL1RFC = 0;
    while (PRCM->PDSTAT1RFC == 1 && PRCM->PDSTAT0RFC == 1) {}

    if (osc_clock_source_get(OSC_SRC_CLK_HF) != OSC_RCOSC_HF) {
        DEBUG("rfc_power_off: switching SCLK_HF to OSC_RCOSC_HF\n");
        /* Request to switch to the RC osc for low power mode. */
        osc_clock_source_set(OSC_SRC_CLK_HF, OSC_RCOSC_HF);
        /* Switch the HF clock source (cc26x2ware executes this from ROM) */
        osc_hf_source_switch();
    }

    _switch_sclk_hf = false;
    _state = FSM_OFF;

    irq_restore(key);
}

static void fsm_power_up(void)
{
    DEBUG("fsm_power_up()\n");

    if (rfc_cpe_int_get(IRQ_MODULES_UNLOCKED)) {
        rfc_cpe_int_clear(IRQ_MODULES_UNLOCKED);
        rfc_cpe_int_disable(IRQ_MODULES_UNLOCKED);
        DEBUG("fsm_power_up: modules unlocked!\n");
    }

    if (rfc_cpe_int_get(IRQ_BOOT_DONE)) {
        rfc_cpe_int_clear(IRQ_BOOT_DONE);
        rfc_cpe_int_disable(IRQ_MODULES_UNLOCKED);
        DEBUG("fsm_power_up: boot done\n");
    }

    /* Perform the switch to XOSC_HF */
    if (_switch_sclk_hf) {
        DEBUG("fsm_power_up: switch SCLK_HF to OSC_XOSC_HF\n");
        /* Block until the high frequency clock source is ready */
        while (!osc_hf_source_ready()) {}

        /* Switch the HF clock source (this get executed from ROM) */
        osc_hf_source_switch();
    }

    /* Turn on the clock line to the radio core, this is necessary to use the
     * CMD_SYNC_START_RAT and the CMD_SYNC_STOP_RAT commands. */
    aon_rtc_ctl_rtc_upd(true);

    /* Enable last command done interrupt */
    rfc_cpe_int_enable(IRQ_LAST_COMMAND_DONE);

    /* Run radio setup command after we start the RAT */
    _start_rat.pNextOp = (rfc_op_t *)_radio_setup;
    _start_rat.rat0 = _rat_offset;

    DEBUG("fsm_power_up: executing radio setup command\n");
    uint32_t cmdsta = rfc_execute_sync((uint32_t)&_start_rat);
    if (cmdsta != CMDSTA_Done) {
        DEBUG("fsm_power_up: radio setup failed! CMDSTA = %lx\n", cmdsta);
    }

    _state = FSM_ON;
}

static void fsm_on(void)
{
    DEBUG("fsm_on()\n");

    if (rfc_cpe_int_get(IRQ_LAST_COMMAND_DONE)) {
        rfc_cpe_int_clear(IRQ_LAST_COMMAND_DONE);
        DEBUG("fsm_on: command finished\n");
    }
}

void isr_rfc_cpe0(void)
{
    DEBUG("isr_rfc_cpe0()\n");

    DEBUG("isr_rfc_cpe0: state is %d\n", _state);

    switch (_state) {
        case FSM_POWER_UP:
            fsm_power_up();
            break;

        case FSM_ON:
            fsm_on();
            break;

        default:
            DEBUG("isr_rfc_cpe0: interrupt fired on invalid state\n");
            break;
    }

    cortexm_isr_end();
}

void isr_rfc_cpe1(void)
{
    DEBUG("isr_rfc_cpe1()\n");

    DEBUG("isr_rfc_cpe1: internal error");

    /* TODO: dump RFC_DBELL, RFC_PWR and PRCM (related with RF Core)
     * registers for debug purposes if anything goes wrong. */

    cortexm_isr_end();
}
