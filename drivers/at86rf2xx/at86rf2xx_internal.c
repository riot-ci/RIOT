/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */
#include "kernel_defines.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_communication.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_properties.h"


uint8_t at86rf2xx_get_status(const at86rf2xx_t *dev)
{
    /* if sleeping immediately return state */
    if (dev->base.state == AT86RF2XX_STATE_SLEEP) {
        return dev->base.state;
    }

    return (at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
            & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS);
}

void at86rf2xx_assert_awake(at86rf2xx_t *dev)
{
    if (at86rf2xx_get_status(dev) == AT86RF2XX_STATE_SLEEP) {
        /* wake up and wait for transition to TRX_OFF */
        switch (dev->base.dev_type) {
            default:
                gpio_clear(dev->params.sleep_pin);
                break;
#if IS_USED(MODULE_AT86RFA1)
            case AT86RF2XX_DEV_TYPE_AT86RFA1: {
                /* Setting SLPTR bit in TRXPR to 0 returns the radio transceiver
                 * to the TRX_OFF state */
                *AT86RFA1_REG__TRXPR &= ~(AT86RF2XX_TRXPR_MASK__SLPTR);
                break;
            }
#endif
#if IS_USED(MODULE_AT86RFR2)
            case AT86RF2XX_DEV_TYPE_AT86RFR2: {
                /* Setting SLPTR bit in TRXPR to 0 returns the radio transceiver
                 * to the TRX_OFF state */
                *AT86RFR2_REG__TRXPR &= ~(AT86RF2XX_TRXPR_MASK__SLPTR);
                break;
            }
#endif
        }
        xtimer_usleep(at86rf2xx_wakeup_delays[dev->base.dev_type]);

        /* update state: on some platforms, the timer behind xtimer
         * may be inaccurate or the radio itself may take longer
         * to wake up due to extra capacitance on the oscillator.
         * Spin until we are actually awake
         */
        do {
            dev->base.state = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
                              & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
        } while (dev->base.state != AT86RF2XX_TRX_STATUS__TRX_OFF);
    }
}

void at86rf2xx_hardware_reset(at86rf2xx_t *dev)
{
    /* trigger hardware reset */
    switch (dev->base.dev_type) {
        default:
            gpio_clear(dev->params.reset_pin);
            xtimer_usleep(AT86RF2XX_RESET_PULSE_WIDTH);
            gpio_set(dev->params.reset_pin);
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            /* set reset Bit */
            *AT86RFA1_REG__TRXPR |= AT86RF2XX_TRXPR_MASK__TRXRST;
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            /* set reset Bit */
            *AT86RFR2_REG__TRXPR |= AT86RF2XX_TRXPR_MASK__TRXRST;
            break;
        }
#endif
    }
    xtimer_usleep(AT86RF2XX_RESET_DELAY);

    /* update state: if the radio state was P_ON (initialization phase),
     * it remains P_ON. Otherwise, it should go to TRX_OFF
     */
    do {
        dev->base.state = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
                          & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    } while ((dev->base.state != AT86RF2XX_STATE_TRX_OFF)
             && (dev->base.state != AT86RF2XX_STATE_P_ON));
}

void at86rf2xx_configure_phy(at86rf2xx_t *dev)
{
    /* we must be in TRX_OFF before changing the PHY configuration */
    uint8_t prev_state = at86rf2xx_set_state(dev, AT86RF2XX_STATE_TRX_OFF);

    switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            /* The TX power register must be updated after changing the channel if
            * moving between bands. */
            int16_t txpower = at86rf2xx_get_txpower(dev);

            uint8_t trx_ctrl2 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_2);
            uint8_t rf_ctrl0 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RF_CTRL_0);

            /* Clear previous configuration for PHY mode */
            trx_ctrl2 &= ~(AT86RF2XX_TRX_CTRL_2_MASK__FREQ_MODE);
            /* Clear previous configuration for GC_TX_OFFS */
            rf_ctrl0 &= ~AT86RF2XX_RF_CTRL_0_MASK__GC_TX_OFFS;

            if (((at86rf212b_t *)dev)->base.netdev.chan != 0) {
                /* Set sub mode bit on 915 MHz as recommended by the data sheet */
                trx_ctrl2 |= AT86RF2XX_TRX_CTRL_2_MASK__SUB_MODE;
            }

            if (((at86rf212b_t *)dev)->page == 0) {
                /* BPSK coding */
                /* Data sheet recommends using a +2 dB setting for BPSK */
                rf_ctrl0 |= AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__2DB;
            }
            else if (((at86rf212b_t *)dev)->page == 2) {
                /* O-QPSK coding */
                trx_ctrl2 |= AT86RF2XX_TRX_CTRL_2_MASK__BPSK_OQPSK_MODE;
                /* Data sheet recommends using a +1 dB setting for O-QPSK */
                rf_ctrl0 |= AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__1DB;
            }

            at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2, trx_ctrl2);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__RF_CTRL_0, rf_ctrl0);
            /* Update the TX power register to achieve the same power (in dBm) */
            at86rf2xx_set_txpower(dev, txpower);
            break;
        }
#endif
    }
    uint8_t phy_cc_cca = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    /* Clear previous configuration for channel number */
    phy_cc_cca &= ~(AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);

    /* Update the channel register */
    phy_cc_cca |= (dev->base.netdev.chan & AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, phy_cc_cca);

    /* Return to the state we had before reconfiguring */
    at86rf2xx_set_state(dev, prev_state);
}

void at86rf2xx_get_random(const at86rf2xx_t *dev, uint8_t *data, size_t len)
{
    for (size_t byteCount = 0; byteCount < len; ++byteCount) {
        uint8_t rnd = 0;
        for (uint8_t i = 0; i < 4; ++i) {
            /* bit 5 and 6 of the AT86RF2XX_REG__PHY_RSSI register contain the RND_VALUE */
            uint8_t regVal = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_RSSI)
                             & AT86RF2XX_PHY_RSSI_MASK__RND_VALUE;
            /* shift the two random bits first to the right and then to the correct position of the return byte */
            regVal = regVal >> 5;
            regVal = regVal << 2 * i;
            rnd |= regVal;
        }
        data[byteCount] = rnd;
    }
}

void at86rf2xx_enable_smart_idle(at86rf2xx_t *dev)
{
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_RPC);
    tmp |= (AT86RF2XX_TRX_RPC_MASK__RX_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__PDT_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__PLL_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__XAH_TX_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__IPAN_RPC_EN);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_RPC, tmp);
    at86rf2xx_set_rxsensitivity(dev, at86rf2xx_rssi_base_values[dev->base.dev_type]);
}
