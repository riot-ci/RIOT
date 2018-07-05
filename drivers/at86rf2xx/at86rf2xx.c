/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 *               2015 Freie Universität Berlin
 *               2017 HAW Hamburg
 *               2018 RWTH Aachen
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
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include "luid.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_netdev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "board.h"
#define ENABLE_HEX_DUMP_TX  (0)
#if ENABLE_HEX_DUMP_TX
#include "od.h"
#endif

void at86rf2xx_setup(at86rf2xx_t *dev, const at86rf2xx_params_t *params)
{
    netdev_t *netdev = (netdev_t *)dev;

    netdev->driver = &at86rf2xx_driver;
    /* initialize device descriptor */
    memcpy(&dev->params, params, sizeof(at86rf2xx_params_t));
    dev->idle_state = AT86RF2XX_STATE_TRX_OFF;
    /* radio state is P_ON when first powered-on */
    dev->state = AT86RF2XX_STATE_P_ON;
    dev->pending_tx = 0;

#ifdef MODULE_AT86RFR2
    /* Store device pointer for interrupts */
    at86rfr2_dev = (netdev_t *)dev;

    /* set all interrupts off */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, 0x00);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK1, 0x00);

#if defined(RXTX_LED_ENABLE) || defined(DEBUG_ATRFR2_PINS)
    enable_rxtx_led();
#endif
#endif
}

void at86rf2xx_reset(at86rf2xx_t *dev)
{
    eui64_t addr_long;

    DEBUG("at86rf2xx_reset(): start.\n");

    at86rf2xx_hardware_reset(dev);

    /* Reset state machine to ensure a known state */
    if (dev->state == AT86RF2XX_STATE_P_ON) {
        at86rf2xx_set_state(dev, AT86RF2XX_STATE_FORCE_TRX_OFF);
    }

    /* reset options and sequence number */
    dev->netdev.seq = 0;
    dev->netdev.flags = 0;

    /* get an 8-byte unique ID to use as hardware address */
    luid_get(addr_long.uint8, IEEE802154_LONG_ADDRESS_LEN);
    /* make sure we mark the address as non-multicast and not globally unique */
    addr_long.uint8[0] &= ~(0x01);
    addr_long.uint8[0] |=  (0x02);
    /* set short and long address */
    at86rf2xx_set_addr_long(dev, ntohll(addr_long.uint64.u64));
    at86rf2xx_set_addr_short(dev, ntohs(addr_long.uint16[0].u16));

    /* set default PAN id */
    at86rf2xx_set_pan(dev, AT86RF2XX_DEFAULT_PANID);
    /* set default channel */
    at86rf2xx_set_chan(dev, AT86RF2XX_DEFAULT_CHANNEL);
    /* set default TX power */
    at86rf2xx_set_txpower(dev, AT86RF2XX_DEFAULT_TXPOWER);
    /* set default options */
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_START, false);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_END, true);

    /* enable safe mode (protect RX FIFO until reading data starts) */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2,
                        AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE);

#ifdef MODULE_NETSTATS_L2
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_TX_END, true);
#endif

    /* set default protocol */
#ifdef MODULE_GNRC_SIXLOWPAN
    dev->netdev.proto = GNRC_NETTYPE_SIXLOWPAN;
#elif MODULE_GNRC
    dev->netdev.proto = GNRC_NETTYPE_UNDEF;
#endif

#ifdef MODULE_AT86RF212B
    at86rf2xx_set_page(dev, AT86RF2XX_DEFAULT_PAGE);
#endif

#ifdef MODULE_AT86RFR2
    /* Set idle state so device is always listening */
    dev->idle_state = AT86RF2XX_STATE_RX_AACK_ON;

    /* TODO enable necessary interrupts, investigate if other interrupts could be useful */

    /* enable interrupts IRQ_MASK */
    at86rf2xx_reg_write(dev,
                        AT86RF2XX_REG__IRQ_MASK,
/*                      AT86RF2XX_IRQ_STATUS_MASK__AWAKE         /* not*/
                        AT86RF2XX_IRQ_STATUS_MASK__TX_END        /* std */
/*                      | AT86RF2XX_IRQ_STATUS_MASK__AMI         /* maybe for ED/RSSI readout */
/*                      | AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE /* not */
                        | AT86RF2XX_IRQ_STATUS_MASK__RX_END      /* std */
/*                      | AT86RF2XX_IRQ_STATUS_MASK__RX_START    /* NEVER in Extended Op. Mode */
/*                      | AT86RF2XX_IRQ_STATUS_MASK__PLL_UNLOCK  /* not */
/*                      | AT86RF2XX_IRQ_STATUS_MASK__PLL_LOCK    /* not */
                        );

    /* enable interrupts IRQ_MASK1*/
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK1,
                        AT86RF2XX_IRQ_STATUS_MASK1__TX_START /* additional for retry counter*/
/*                      | AT86RF2XX_IRQ_STATUS_MASK1__MAF_0_AMI */
/*                      | AT86RF2XX_IRQ_STATUS_MASK1__MAF_1_AMI */
/*                      | AT86RF2XX_IRQ_STATUS_MASK1__MAF_2_AMI */
/*                      | AT86RF2XX_IRQ_STATUS_MASK1__MAF_3_AMI */
                        );

    /* clear interrupt flags by writing corresponding bit */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_STATUS, 0xff);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_STATUS1, 0xff);
    /* clear frame buffer protection */
    *AT86RF2XX_REG__TRX_CTRL_2 &= ~(1 << RX_SAFE_MODE);

    /* set PLL on */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_PLL_ON);
#else
    /* don't populate masked interrupt flags to IRQ_STATUS register */
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_1);
    tmp &= ~(AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_1, tmp);

    /* disable clock output to save power */
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_0);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL);
    tmp |= (AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_0, tmp);

    /* clear interrupt flags, AT86RF233 Manual p.33*/
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);

    /* enable interrupts */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK,
                        AT86RF2XX_IRQ_STATUS_MASK__TRX_END);
#endif

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

    dev->pending_tx++;
    state = at86rf2xx_set_state(dev, AT86RF2XX_STATE_TX_ARET_ON);
    if (state != AT86RF2XX_STATE_TX_ARET_ON) {
        dev->idle_state = state;
    }
    dev->tx_frame_len = IEEE802154_FCS_LEN;
}

size_t at86rf2xx_tx_load(at86rf2xx_t *dev, const uint8_t *data,
                         size_t len, size_t offset)
{
    dev->tx_frame_len += (uint8_t)len;
    at86rf2xx_sram_write(dev, offset + 1, data, len);
    return offset + len;
}

void at86rf2xx_tx_exec(const at86rf2xx_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;

    /* write frame length field in FIFO */
    at86rf2xx_sram_write(dev, 0, &(dev->tx_frame_len), 1);
#if ENABLE_HEX_DUMP_TX
    uint8_t len = dev->tx_frame_len;
    uint8_t data[len];
    memcpy( data, (void *)(AT86RF2XX_REG__TRXFBST), len);
    puts("SENDING:");
    od_hex_dump(data, len, OD_WIDTH_DEFAULT);
#endif

    /* trigger sending of pre-loaded frame */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE,
                        AT86RF2XX_TRX_STATE__TX_START);
    if (netdev->event_callback &&
        (dev->netdev.flags & AT86RF2XX_OPT_TELL_TX_START)) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    }
}

bool at86rf2xx_cca(at86rf2xx_t *dev)
{
    uint8_t reg;
    uint8_t old_state = at86rf2xx_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    /* Disable RX path */
    uint8_t rx_syn = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RX_SYN);

    reg = rx_syn | AT86RF2XX_RX_SYN__RX_PDT_DIS;
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
