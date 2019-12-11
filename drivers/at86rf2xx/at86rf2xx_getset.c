/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
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
 * @brief       Getter and setter functions for the AT86RF2xx drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <string.h>

#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_communication.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_properties.h"
#include "at86rf212b.h"
#include "at86rf231.h"
#include "at86rf232.h"
#include "at86rf233.h"
#include "at86rfa1.h"
#include "at86rfr2.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void at86rf2xx_get_addr_short(const at86rf2xx_t *dev, network_uint16_t *addr)
{
    memcpy(addr, dev->base.netdev.short_addr, sizeof(*addr));
}

void at86rf2xx_set_addr_short(at86rf2xx_t *dev, const network_uint16_t *addr)
{
    memcpy(dev->base.netdev.short_addr, addr, sizeof(*addr));
#if IS_USED(MODULE_SIXLOWPAN)
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    dev->base.netdev.short_addr[0] &= 0x7F;
#endif
    /* device use lsb first, not network byte order */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_0,
                        dev->base.netdev.short_addr[1]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_1,
                        dev->base.netdev.short_addr[0]);
}

void at86rf2xx_get_addr_long(const at86rf2xx_t *dev, eui64_t *addr)
{
    memcpy(addr, dev->base.netdev.long_addr, sizeof(*addr));
}

void at86rf2xx_set_addr_long(at86rf2xx_t *dev, const eui64_t *addr)
{
    memcpy(dev->base.netdev.long_addr, addr, sizeof(*addr));
    for (int i = 0; i < 8; i++) {
        /* device use lsb first, not network byte order */
        at86rf2xx_reg_write(dev, (AT86RF2XX_REG__IEEE_ADDR_0 + i),
                dev->base.netdev.long_addr[IEEE802154_LONG_ADDRESS_LEN - 1 - i]);
    }
}

uint8_t at86rf2xx_get_chan(const at86rf2xx_t *dev)
{
    return dev->base.netdev.chan;
}

void at86rf2xx_set_chan(at86rf2xx_t *dev, uint8_t channel)
{
    if (channel < at86rf2xx_min_channels[dev->base.dev_type] ||
        channel > at86rf2xx_max_channels[dev->base.dev_type]) {
        return;
    }
    dev->base.netdev.chan = channel;
    at86rf2xx_configure_phy(dev);
}

uint16_t at86rf2xx_get_pan(const at86rf2xx_t *dev)
{
    return dev->base.netdev.pan;
}

void at86rf2xx_set_pan(at86rf2xx_t *dev, uint16_t pan)
{
    le_uint16_t le_pan = byteorder_btols(byteorder_htons(pan));

    DEBUG("pan0: %u, pan1: %u\n", le_pan.u8[0], le_pan.u8[1]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_0, le_pan.u8[0]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_1, le_pan.u8[1]);
}

int16_t at86rf2xx_get_txpower(const at86rf2xx_t *dev)
{
    uint8_t txpower = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_TX_PWR);

    switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            txpower &= AT86RF212B_PHY_TX_PWR_MASK__TX_PWR;
            DEBUG("[at86rf212b] txpower value: %x\n", txpower);
            return at86rf212b_tx_pow_to_dbm((const at86rf212b_t *)dev, txpower);
        }
#endif
#if IS_USED(MODULE_AT86RF231)
        case AT86RF2XX_DEV_TYPE_AT86RF231: {
            txpower &= AT86RF231_PHY_TX_PWR_MASK__TX_PWR;
            DEBUG("[at86rf231] txpower value: %x\n", txpower);
            return at86rf231_tx_pow_to_dbm((const at86rf231_t *)dev, txpower);
        }
#endif
#if IS_USED(MODULE_AT86RF232)
        case AT86RF2XX_DEV_TYPE_AT86RF232: {
            txpower &= AT86RF232_PHY_TX_PWR_MASK__TX_PWR;
            DEBUG("[at86rf232] txpower value: %x\n", txpower);
            return at86rf232_tx_pow_to_dbm((const at86rf232_t *)dev, txpower);
        }
#endif
#if IS_USED(MODULE_AT86RF233)
        case AT86RF2XX_DEV_TYPE_AT86RF233: {
            txpower &= AT86RF233_PHY_TX_PWR_MASK__TX_PWR;
            DEBUG("[at86rf233] txpower value: %x\n", txpower);
            return at86rf233_tx_pow_to_dbm((const at86rf233_t *)dev, txpower);
        }
#endif
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            txpower &= AT86RFA1_PHY_TX_PWR_MASK__TX_PWR;
            DEBUG("[at86rfa1] txpower value: %x\n");
            return at86rfa1_tx_pow_to_dbm((const at86rfa1_t *)dev, txpower);
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            txpower &= AT86RFR2_PHY_TX_PWR_MASK__TX_PWR;
            DEBUG("[at86rfr2] txpower value: %x\n");
            return at86rfr2_tx_pow_to_dbm((const at86rfr2_t *)dev, txpower);
        }
#endif
        default:
            /* Should never be reached */
            DEBUG("[at86rf2xx] Unsupported device type\n");
            return (int16_t)txpower;
    }
}

void at86rf2xx_set_txpower(const at86rf2xx_t *dev, int16_t dbm)
{
    uint8_t txpower = 0;
    switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            txpower = at86rf212b_dbm_to_tx_pow((const at86rf212b_t *)dev, dbm);
            DEBUG("[at86rf212b] txpower value: %x\n", txpower);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF231)
        case AT86RF2XX_DEV_TYPE_AT86RF231: {
            txpower = at86rf231_dbm_to_tx_pow((const at86rf231_t *)dev, dbm);
            DEBUG("[at86rf231] txpower value: %x\n", txpower);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF232)
        case AT86RF2XX_DEV_TYPE_AT86RF232: {
            txpower = at86rf232_dbm_to_tx_pow((const at86rf232_t *)dev, dbm);
            DEBUG("[at86rf232] txpower value: %x\n", txpower);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF233)
        case AT86RF2XX_DEV_TYPE_AT86RF233: {
            txpower = at86rf233_dbm_to_tx_pow((const at86rf233_t *)dev, dbm);
            DEBUG("[at86rf233] txpower value: %x\n", txpower);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            txpower = at86rfa1_dbm_to_tx_pow((const at86rfa1_t *)dev, dbm);
            DEBUG("[at86rfa1] txpower value: %x\n");
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            txpower = at86rfr2_dbm_to_tx_pow((const at86rfr2_t *)dev, dbm);
            DEBUG("[at86rfr2] txpower value: %x\n");
            break;
        }
#endif
        default:
            /* Should never be reached */
            DEBUG("[at86rf2xx] Unsupported device type\n");
            return;
    }

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR, txpower);
}

int16_t at86rf2xx_get_rxsensitivity(const at86rf2xx_t *dev)
{
    uint8_t rxsens = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RX_SYN)
                     & AT86RF2XX_RX_SYN_MASK__RX_PDT_LEVEL;

    switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            DEBUG("[at86rf212b] rxsens value: %x\n", rxsens);
            return at86rf212b_rx_sens_to_dbm((const at86rf212b_t *)dev, rxsens);
        }
#endif
#if IS_USED(MODULE_AT86RF231)
        case AT86RF2XX_DEV_TYPE_AT86RF231: {
            DEBUG("[at86rf231] rxsens value: %x\n", rxsens);
            return at86rf231_rx_sens_to_dbm((const at86rf231_t *)dev, rxsens);
        }
#endif
#if IS_USED(MODULE_AT86RF232)
        case AT86RF2XX_DEV_TYPE_AT86RF232: {
            DEBUG("[at86rf232] rxsens value: %x\n", rxsens);
            return at86rf232_rx_sens_to_dbm((const at86rf232_t *)dev, rxsens);
        }
#endif
#if IS_USED(MODULE_AT86RF233)
        case AT86RF2XX_DEV_TYPE_AT86RF233: {
            DEBUG("[at86rf233] rxsens value: %x\n", rxsens);
            return at86rf233_rx_sens_to_dbm((const at86rf233_t *)dev, rxsens);
        }
#endif
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            DEBUG("[at86rfa1] rxsens value: %x\n");
            return at86rfa1_rx_sens_to_dbm((const at86rfa1_t *)dev, rxsens);
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            DEBUG("[at86rfr2] rxsens value: %x\n");
            return at86rfr2_rx_sens_to_dbm((const at86rfr2_t *)dev, rxsens);
        }
#endif
        default:
            /* Should never be reached */
            DEBUG("[at86rf2xx] Unsupported device type\n");
            return (int16_t)rxsens;
    }
}

void at86rf2xx_set_rxsensitivity(const at86rf2xx_t *dev, int16_t dbm)
{
    uint8_t rxsens = 0;
    switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            rxsens = at86rf212b_dbm_to_rxsens((const at86rf212b_t *)dev, dbm);
            DEBUG("[at86rf212b] rxsens value: %x\n", rxsens);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF231)
        case AT86RF2XX_DEV_TYPE_AT86RF231: {
            rxsens = at86rf231_dbm_to_rxsens((const at86rf231_t *)dev, dbm);
            DEBUG("[at86rf231] rxsens value: %x\n", rxsens);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF232)
        case AT86RF2XX_DEV_TYPE_AT86RF232: {
            rxsens = at86rf232_dbm_to_rxsens((const at86rf232_t *)dev, dbm);
            DEBUG("[at86rf232] rxsens value: %x\n", rxsens);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RF233)
        case AT86RF2XX_DEV_TYPE_AT86RF233: {
            rxsens = at86rf233_dbm_to_rxsens((const at86rf233_t *)dev, dbm);
            DEBUG("[at86rf233] rxsens value: %x\n", rxsens);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            rxsens = at86rfa1_dbm_to_rxsens((const at86rfa1_t *)dev, dbm);
            DEBUG("[at86rfa1] rxsens value: %x\n", rxsens);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            rxsens = at86rfr2_dbm_to_rxsens((const at86rfr2_t *)dev, dbm);
            DEBUG("[at86rfr2] rxsens value: %x\n", rxsens);
            break;
        }
#endif
        default:
            /* Should never be reached */
            (void)rxsens;
            DEBUG("[at86rf2xx] Unsupported device type\n");
            return;
    }

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RX_SYN);
    tmp &= ~(AT86RF2XX_RX_SYN_MASK__RX_PDT_LEVEL);
    tmp |= (rxsens & AT86RF2XX_RX_SYN_MASK__RX_PDT_LEVEL);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RX_SYN, tmp);
}

uint8_t at86rf2xx_get_max_retries(const at86rf2xx_t *dev)
{
    return (at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0) >> 4);
}

void at86rf2xx_set_max_retries(const at86rf2xx_t *dev, uint8_t max)
{
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);

    tmp &= ~(AT86RF2XX_XAH_CTRL_0_MASK__MAX_FRAME_RETRIES);
    tmp |= ((max > AT86RF2XX_MAX_FRAME_RETRIES)
            ? AT86RF2XX_MAX_FRAME_RETRIES
            : max) << 4;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);
}

uint8_t at86rf2xx_get_csma_max_retries(const at86rf2xx_t *dev)
{
    uint8_t tmp;

    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= AT86RF2XX_XAH_CTRL_0_MASK__MAX_CSMA_RETRIES;
    tmp >>= 1;
    return tmp;
}

#define AT86RF2XX_NO_CSMA           (7)
void at86rf2xx_set_csma_max_retries(const at86rf2xx_t *dev, int8_t retries)
{
    retries = (retries > AT86RF2XX_MAX_CSMA_RETRIES)
              ? AT86RF2XX_MAX_CSMA_RETRIES
              : retries;  /* valid values: 0-5 */
    /* max < 0 => disable CSMA (set to 7) */
    retries = (retries < 0) ? AT86RF2XX_NO_CSMA : retries;
    DEBUG("[at86rf2xx] opt: Set CSMA retries to %u\n", retries);

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= ~(AT86RF2XX_XAH_CTRL_0_MASK__MAX_CSMA_RETRIES);
    tmp |= (retries << 1);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);
}

#define AT86RF2XX_CCA_MAX_BE    (8)
void at86rf2xx_set_csma_backoff_exp(const at86rf2xx_t *dev,
                                    uint8_t min, uint8_t max)
{
    max = (max > AT86RF2XX_CCA_MAX_BE) ? AT86RF2XX_CCA_MAX_BE : max;
    min = (min > max) ? max : min;
    DEBUG("[at86rf2xx] opt: Set min BE=%u, max BE=%u\n", min, max);

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_BE, (max << 4) | (min));
}

void at86rf2xx_set_csma_seed(const at86rf2xx_t *dev, const uint8_t entropy[2])
{
    if (entropy == NULL) {
        DEBUG("[at86rf2xx] opt: CSMA seed entropy is nullpointer\n");
        return;
    }
    DEBUG("[at86rf2xx] opt: Set CSMA seed to 0x%x 0x%x\n", entropy[0], entropy[1]);

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_0, entropy[0]);

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
    tmp &= ~(AT86RF2XX_CSMA_SEED_1_MASK__CSMA_SEED_1);
    tmp |= entropy[1] & AT86RF2XX_CSMA_SEED_1_MASK__CSMA_SEED_1;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
}

int8_t at86rf2xx_get_cca_threshold(const at86rf2xx_t *dev)
{
    int8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CCA_THRES);

    tmp &= AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;
    tmp <<= 1;
    return at86rf2xx_rssi_base_values[dev->base.dev_type] + tmp;
}

void at86rf2xx_set_cca_threshold(const at86rf2xx_t *dev, int8_t value)
{
    /* ensure the given value is negative, since a CCA threshold > 0 is
       just impossible: thus, any positive value given is considered
       to be the absolute value of the actually wanted threshold */
    if (value > 0) {
        value = -value;
    }
    /* transform the dBm value in the form
       that will fit in the AT86RF2XX_REG__CCA_THRES register */
    value -= at86rf2xx_rssi_base_values[dev->base.dev_type];
    value >>= 1;
    value &= AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CCA_THRES);
    tmp &= ~AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;
    value = tmp | value;
    value |= AT86RF2XX_CCA_THRES_MASK__RSVD_HI_NIBBLE; /* What is this? */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CCA_THRES, value);
}

int8_t at86rf2xx_get_ed_level(const at86rf2xx_t *dev)
{
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_ED_LEVEL);
    int8_t rssi_base_val = at86rf2xx_rssi_base_values[dev->base.dev_type];
    switch (dev->base.dev_type) {
        default: return (int8_t)tmp + rssi_base_val;
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            return (int8_t)(((int16_t)tmp * 103) / 100) + rssi_base_val;
        }
#endif
    }
}

void at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state)
{
    uint8_t tmp;

    DEBUG("set option %i to %i\n", option, state);

    /* set option field */
    dev->base.flags = (state) ? (dev->base.flags |  option)
                              : (dev->base.flags & ~option);
    /* trigger option specific actions */
    switch (option) {
        case AT86RF2XX_OPT_CSMA:
            if (state) {
                DEBUG("[at86rf2xx] opt: enabling CSMA mode" \
                      "(4 retries, min BE: 3 max BE: 5)\n");
                /* Initialize CSMA seed with hardware address */
                at86rf2xx_set_csma_seed(dev, dev->base.netdev.long_addr);
                at86rf2xx_set_csma_max_retries(dev, 4);
                at86rf2xx_set_csma_backoff_exp(dev, 3, 5);
            }
            else {
                DEBUG("[at86rf2xx] opt: disabling CSMA mode\n");
                /* setting retries to -1 means CSMA disabled */
                at86rf2xx_set_csma_max_retries(dev, -1);
            }
            break;
        case AT86RF2XX_OPT_PROMISCUOUS:
            DEBUG("[at86rf2xx] opt: %s PROMISCUOUS mode\n",
                  (state ? "enable" : "disable"));
            /* disable/enable auto ACKs in promiscuous mode */
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
            tmp = (state) ? (tmp |  AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK)
                          : (tmp & ~AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
            /* enable/disable promiscuous mode */
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_1);
            tmp = (state) ? (tmp |  AT86RF2XX_XAH_CTRL_1_MASK__AACK_PROM_MODE)
                          : (tmp & ~AT86RF2XX_XAH_CTRL_1_MASK__AACK_PROM_MODE);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_1, tmp);
            break;
        case AT86RF2XX_OPT_AUTOACK:
            DEBUG("[at86rf2xx] opt: %s auto ACKs\n",
                  (state ? "enable" : "disable"));
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
            tmp = (state) ? (tmp & ~AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK)
                          : (tmp |  AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
            break;
        case AT86RF2XX_OPT_TELL_RX_START:
            DEBUG("[at86rf2xx] opt: %s SFD IRQ\n",
                  (state ? "enable" : "disable"));
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_MASK);
            tmp = (state) ? (tmp |  AT86RF2XX_IRQ_STATUS_MASK__RX_START)
                          : (tmp & ~AT86RF2XX_IRQ_STATUS_MASK__RX_START);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, tmp);
            break;
        case AT86RF2XX_OPT_ACK_PENDING:
            DEBUG("[at86rf2xx] opt: enabling pending ACKs\n");
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
            tmp = (state) ? (tmp |  AT86RF2XX_CSMA_SEED_1_MASK__AACK_SET_PD)
                          : (tmp & ~AT86RF2XX_CSMA_SEED_1_MASK__AACK_SET_PD);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
            break;
        default:
            /* do nothing */
            break;
    }
}

/**
 * @brief Internal function to change state
 * @details For all cases but AT86RF2XX_STATE_FORCE_TRX_OFF state and
 *          cmd parameter are the same.
 *
 * @param dev       device to operate on
 * @param state     target state
 * @param cmd       command to initiate state transition
 */

static inline void _set_state(at86rf2xx_t *dev, uint8_t state, uint8_t cmd)
{
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, cmd);

    /* To prevent a possible race condition when changing to
     * RX_AACK_ON state the state doesn't get read back in that
     * case. See discussion
     * in https://github.com/RIOT-OS/RIOT/pull/5244
     */
    if (state != AT86RF2XX_STATE_RX_AACK_ON) {
        while (at86rf2xx_get_status(dev) != state) {}
    }
    /* Although RX_AACK_ON state doesn't get read back,
     * at least make sure if state transition is in progress or not
     */
    else {
        while (at86rf2xx_get_status(dev) == AT86RF2XX_STATE_IN_PROGRESS) {}
    }

    dev->base.state = state;
}

uint8_t at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state)
{
    uint8_t old_state;

    /* make sure there is no ongoing transmission, or state transition already
     * in progress */
    do {
        old_state = at86rf2xx_get_status(dev);
    } while (old_state == AT86RF2XX_STATE_BUSY_RX_AACK ||
             old_state == AT86RF2XX_STATE_BUSY_TX_ARET ||
             old_state == AT86RF2XX_STATE_IN_PROGRESS);

    if (state == AT86RF2XX_STATE_FORCE_TRX_OFF) {
        _set_state(dev, AT86RF2XX_STATE_TRX_OFF, state);
    }
    else if (state != old_state) {
        /* we need to go via PLL_ON if we are moving between RX_AACK_ON <-> TX_ARET_ON */
        if ((old_state == AT86RF2XX_STATE_RX_AACK_ON &&
             state == AT86RF2XX_STATE_TX_ARET_ON) ||
            (old_state == AT86RF2XX_STATE_TX_ARET_ON &&
             state == AT86RF2XX_STATE_RX_AACK_ON)) {
            _set_state(dev, AT86RF2XX_STATE_PLL_ON, AT86RF2XX_STATE_PLL_ON);
        }
        /* check if we need to wake up from sleep mode */
        if (state == AT86RF2XX_STATE_SLEEP) {
            /* First go to TRX_OFF */
            _set_state(dev, AT86RF2XX_STATE_TRX_OFF,
                       AT86RF2XX_STATE_FORCE_TRX_OFF);
            /* Discard all IRQ flags, framebuffer is lost anyway */
            at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);
            /* Go to SLEEP mode from TRX_OFF */
            switch (dev->base.dev_type) {
                default:
                    gpio_set(dev->params.sleep_pin);
                    break;
#if IS_USED(MODULE_AT86RFA1)
                case AT86RF2XX_DEV_TYPE_AT86RFA1: {
                    /* reset interrupts states in device */
                    ((at86rfa1_t *)dev)->irq_status = 0;
                    /* Setting SLPTR bit brings radio transceiver
                       to sleep in TRX_OFF */
                    *AT86RFA1_REG__TRXPR |= (AT86RF2XX_TRXPR_MASK__SLPTR);
                    break;
                }
#endif
#if IS_USED(MODULE_AT86RFR2)
                case AT86RF2XX_DEV_TYPE_AT86RFR2: {
                    /* reset interrupts states in device */
                    ((at86rfr2_t *)dev)->irq_status = 0;
                    /* Setting SLPTR bit brings radio transceiver
                       to sleep in TRX_OFF */
                    *AT86RFR2_REG__TRXPR |= (AT86RF2XX_TRXPR_MASK__SLPTR);
                    break;
                }
#endif
            }
            dev->base.state = state;
        }
        else {
            if (old_state == AT86RF2XX_STATE_SLEEP) {
                DEBUG("at86rf2xx: waking up from sleep mode\n");
                at86rf2xx_assert_awake(dev);
            }
            _set_state(dev, state, state);
        }
    }

    return old_state;
}
