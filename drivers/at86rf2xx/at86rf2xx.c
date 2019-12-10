/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
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
 * @brief       Implementation of public functions for AT86RF2xx drivers
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */


#include "luid.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_netdev.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_properties.h"
#include "at86rf212b.h"
#include "at86rf231.h"
#include "at86rf233.h"
#include "at86rf233.h"
#include "at86rfa1.h"
#include "at86rfr2.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void at86rf2xx_setup(at86rf2xx_t *dev)
{
    assert(dev->base.dev_type < AT86RF2XX_DEV_TYPE_NUM_OF);
    netdev_t *netdev = (netdev_t *)dev;
    netdev->driver = &at86rf2xx_driver;
    /* State to return after receiving or transmitting */
    dev->base.idle_state = AT86RF2XX_STATE_TRX_OFF;
    /* radio state is P_ON when first powered-on */
    dev->base.state = AT86RF2XX_STATE_P_ON;
    dev->base.pending_tx = 0;

    switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            /* set all interrupts off */
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, 0x00);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            /* set all interrupts off */
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, 0x00);
            break;
        }
#endif
    }
}

void at86rf212b_setup(at86rf212b_t *devs, const at86rf212b_params_t *params,
                      uint8_t num)
{
#if IS_USED(MODULE_AT86RF212B)
    for (int i = 0; i < num; i++) {
        devs->base.dev_type = AT86RF2XX_DEV_TYPE_AT86RF212B;
        devs->params = *params;
        at86rf2xx_setup((at86rf2xx_t *)devs);
        devs++;
        params++;
    }
#else
    (void)devs;
    (void)params;
    (void)num;
#endif
}

void at86rf231_setup(at86rf231_t *devs, const at86rf231_params_t *params,
                     uint8_t num)
{
#if IS_USED(MODULE_AT86RF231)
    for (int i = 0; i < num; i++) {
        devs->base.dev_type = AT86RF2XX_DEV_TYPE_AT86RF231;
        devs->params = *params;
        at86rf2xx_setup((at86rf2xx_t *)devs);
        devs++;
        params++;
    }
#else
    (void)devs;
    (void)params;
    (void)num;
#endif
}

void at86rf232_setup(at86rf232_t *devs, const at86rf232_params_t *params,
                     uint8_t num)
{
#if IS_USED(MODULE_AT86RF232)
    for (int i = 0; i < num; i++) {
        devs->base.dev_type = AT86RF2XX_DEV_TYPE_AT86RF232;
        devs->params = *params;
        at86rf2xx_setup((at86rf2xx_t *)devs);
        devs++;
        params++;
    }
#else
    (void)devs;
    (void)params;
    (void)num;
#endif
}

void at86rf233_setup(at86rf233_t *devs, const at86rf233_params_t *params,
                     uint8_t num)
{
#if IS_USED(MODULE_AT86RF233)
    for (int i = 0; i < num; i++) {
        devs->base.dev_type = AT86RF2XX_DEV_TYPE_AT86RF233;
        devs->params = *params;
        at86rf2xx_setup((at86rf2xx_t *)devs);
        devs++;
        params++;
    }
#else
    (void)devs;
    (void)params;
    (void)num;
#endif
}

void at86rfa1_setup(at86rfa1_t *dev)
{
#if IS_USED(MODULE_AT86RFA1)
    dev->base.dev_type = AT86RF2XX_DEV_TYPE_AT86RFA1;
    at86rf2xx_setup((at86rf2xx_t *)dev);
#else
    (void)dev;
#endif
}

void at86rfr2_setup(at86rfr2_t *dev)
{
#if IS_USED(MODULE_AT86RFR2)
    dev->base.dev_type = AT86RF2XX_DEV_TYPE_AT86RFR2;
    at86rf2xx_setup((at86rf2xx_t *)dev);
#else
    (void)dev;
#endif
}

size_t at86rf2xx_get_size(const at86rf2xx_t *dev)
{
    switch (dev->base.dev_type) {
        default: return sizeof(at86rf2xx_t);
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            return sizeof(at86rf212b_t);
        }
#endif
#if IS_USED(MODULE_AT86RF231)
        case AT86RF2XX_DEV_TYPE_AT86RF231: {
            return sizeof(at86rf231_t);
        }
#endif
#if IS_USED(MODULE_AT86RF232)
        case AT86RF2XX_DEV_TYPE_AT86RF232: {
            return sizeof(at86rf232_t);
        }
#endif
#if IS_USED(MODULE_AT86RF233)
        case AT86RF2XX_DEV_TYPE_AT86RF233: {
            return sizeof(at86rf233_t);
        }
#endif
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            return sizeof(at86rfa1_t);
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            return sizeof(at86rfr2_t);
        }
#endif
    }
}

static void at86rf2xx_disable_clock_output(at86rf2xx_t *dev)
{
    switch (dev->base.dev_type) {
        default:;
            uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_0);
            tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL);
            tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL);
            tmp |= (AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_0, tmp);
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: break;
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: break;
#endif
    }
}

static void at86rf2xx_enable_smart_idle(at86rf2xx_t *dev)
{
    switch (dev->base.dev_type) {
        default:
            (void)dev;
            break;
#if IS_USED(MODULE_AT86RF233)
        case AT86RF2XX_DEV_TYPE_AT86RF233: {
            uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_RPC);
            tmp |= (AT86RF2XX_TRX_RPC_MASK__RX_RPC_EN |
                    AT86RF2XX_TRX_RPC_MASK__PDT_RPC_EN |
                    AT86RF2XX_TRX_RPC_MASK__PLL_RPC_EN |
                    AT86RF2XX_TRX_RPC_MASK__XAH_TX_RPC_EN |
                    AT86RF2XX_TRX_RPC_MASK__IPAN_RPC_EN);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_RPC, tmp);
            at86rf2xx_set_rxsensitivity(dev, AT86RF233_RSSI_BASE_VAL);
            break;
        }
#endif
    }
}

void at86rf2xx_reset(at86rf2xx_t *dev)
{
    eui64_t addr_long;

    at86rf2xx_hardware_reset(dev);

    netdev_ieee802154_reset(&dev->base.netdev);

    /* Reset state machine to ensure a known state */
    if (dev->base.state == AT86RF2XX_STATE_P_ON) {
        at86rf2xx_set_state(dev, AT86RF2XX_STATE_FORCE_TRX_OFF);
    }

    /* get an 8-byte ID to use as hardware address */
    luid_base(addr_long.uint8, IEEE802154_LONG_ADDRESS_LEN);

    /* modify last byte to make ID unique */
    luid_get(&addr_long.uint8[IEEE802154_LONG_ADDRESS_LEN - 1], 1);

    /* make sure we mark the address as non-multicast and not globally unique */
    addr_long.uint8[0] &= ~(0x01);
    addr_long.uint8[0] |=  (0x02);
    /* set short and long address */
    at86rf2xx_set_addr_long(dev, &addr_long);
    at86rf2xx_set_addr_short(dev, &addr_long.uint16[ARRAY_SIZE(addr_long.uint16) - 1]);

    /* set default channel */
    at86rf2xx_set_chan(dev, at86rf2xx_default_channels[dev->base.dev_type]);
    /* set default TX power */
    at86rf2xx_set_txpower(dev, AT86RF2XX_DEFAULT_TXPOWER);
    /* set default options */
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA, true);

    static const netopt_enable_t enable = NETOPT_ENABLE;
    netdev_ieee802154_set(&dev->base.netdev, NETOPT_ACK_REQ,
                          &enable, sizeof(enable));

    /* enable safe mode (protect RX FIFO until reading data starts) */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2,
                        AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE);

    switch (dev->base.dev_type) {
#if IS_USED(MODULE_AT86RF212B)
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            at86rf2xx_set_page(dev, AT86RF212B_DEFAULT_PAGE);
            break;
        }
#endif
    }

    switch (dev->base.dev_type) {
        default:;
            uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_1);
            tmp &= ~(AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_1, tmp);
            break;
#if IS_USED(MODULE_AT86RFA1)
        /* don't populate masked interrupt flags to IRQ_STATUS register */
        case AT86RF2XX_DEV_TYPE_AT86RFA1: break;
#endif
#if IS_USED(MODULE_AT86RFR2)
        /* don't populate masked interrupt flags to IRQ_STATUS register */
        case AT86RF2XX_DEV_TYPE_AT86RFR2: break;
#endif
    }

    /* configure smart idle listening feature */
    at86rf2xx_enable_smart_idle(dev);

    /* disable clock output to save power */
    at86rf2xx_disable_clock_output(dev);

    uint8_t en_irq_mask;
    switch (dev->base.dev_type) {
        default:
            en_irq_mask = AT86RF2XX_IRQ_STATUS_MASK__TRX_END;
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            en_irq_mask = AT86RF2XX_IRQ_STATUS_MASK__TX_END |
                        AT86RF2XX_IRQ_STATUS_MASK__RX_END;
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            en_irq_mask = AT86RF2XX_IRQ_STATUS_MASK__TX_END |
                        AT86RF2XX_IRQ_STATUS_MASK__RX_END;
            break;
        }
#endif
}
    /* enable interrupts */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, en_irq_mask);

    /* clear interrupt flags */
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);

    /* State to return after receiving or transmitting */
    dev->base.idle_state = AT86RF2XX_STATE_RX_AACK_ON;
    /* go into RX state */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_AACK_ON);

    DEBUG("at86rf2xx_reset(): reset complete.\n");
}

size_t at86rf2xx_send(at86rf2xx_t *dev, const uint8_t *data, size_t len)
{
    /* check data length */
    if (len > AT86RF2XX_MAX_PKT_LENGTH) {
        DEBUG("[at86rf2xx] Error: data to send exceeds max packet size\n");
        return 0;
    }
    at86rf2xx_tx_prepare(dev);
    at86rf2xx_tx_load(dev, data, len, 0);
    at86rf2xx_tx_exec(dev);
    return len;
}

void at86rf2xx_tx_prepare(at86rf2xx_t *dev)
{
    uint8_t state;

    dev->base.pending_tx++;
    state = at86rf2xx_set_state(dev, AT86RF2XX_STATE_TX_ARET_ON);
    if (state != AT86RF2XX_STATE_TX_ARET_ON) {
        dev->base.idle_state = state;
    }
    dev->base.tx_frame_len = IEEE802154_FCS_LEN;
}

size_t at86rf2xx_tx_load(at86rf2xx_t *dev, const uint8_t *data,
                         size_t len, size_t offset)
{
    dev->base.tx_frame_len += (uint8_t)len;
    at86rf2xx_sram_write(dev, offset + 1, data, len);
    return offset + len;
}

void at86rf2xx_tx_exec(const at86rf2xx_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;

    /* write frame length field in FIFO */
    at86rf2xx_sram_write(dev, 0, &(dev->base.tx_frame_len), 1);
    /* trigger sending of pre-loaded frame */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE,
                        AT86RF2XX_TRX_STATE__TX_START);
    if (netdev->event_callback &&
        (dev->base.flags & AT86RF2XX_OPT_TELL_TX_START)) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    }
}

bool at86rf2xx_cca(at86rf2xx_t *dev)
{
    uint8_t reg;
    uint8_t old_state = at86rf2xx_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    /* Disable RX path */
    uint8_t rx_syn = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RX_SYN);

    reg = rx_syn | AT86RF2XX_RX_SYN_MASK__RX_PDT_DIS;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RX_SYN, reg);
    /* Manually triggered CCA is only possible in RX_ON (basic operating mode) */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_ON);
    /* Perform CCA */
    reg = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    reg |= AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, reg);
    /* Spin until done (8 symbols + 12 µs = 128 µs + 12 µs for O-QPSK)*/
    do {
        reg = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS);
    } while ((reg & AT86RF2XX_TRX_STATUS_MASK__CCA_DONE) == 0);
    /* return true if channel is clear */
    bool ret = !!(reg & AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS);
    /* re-enable RX */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RX_SYN, rx_syn);
    /* Step back to the old state */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    at86rf2xx_set_state(dev, old_state);
    return ret;
}
