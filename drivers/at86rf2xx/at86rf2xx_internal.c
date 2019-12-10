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
#if AT86RF2XX_NEED_SPI
#include "periph/spi.h"
#endif
#include "periph/gpio.h"
#include "xtimer.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_properties.h"
#include "at86rf212b.h"
#include "at86rf231.h"
#include "at86rf233.h"
#include "at86rf233.h"
#include "at86rfa1.h"
#include "at86rfr2.h"

/* 8-bit  MCU integrated transceivers */
#if AT86RF2XX_IN_MCU
#include <string.h>

static inline uint8_t
at86rf2xx_reg_read_mcu(const at86rf2xx_t *dev, volatile uint8_t *addr)
{
    (void)dev;
    return *addr;
}

static inline void
at86rf2xx_reg_write_mcu(const at86rf2xx_t *dev, volatile uint8_t *addr,
                        const uint8_t value)
{
    (void)dev;
    *addr = value;
}

static inline void
at86rf2xx_sram_read_mcu(const at86rf2xx_t *dev, volatile uint8_t *sram_addr,
                        uint8_t offset, uint8_t *data, size_t len)
{
    (void)dev;
    memcpy(data, (void *)(sram_addr + offset), len);
}

static inline void
at86rf2xx_sram_write_mcu(const at86rf2xx_t *dev, volatile uint8_t *sram_addr,
                         uint8_t offset, const uint8_t *data, size_t len)
{
    (void)dev;
    memcpy((void *)(sram_addr + offset), data, len);
}

static inline void
at86rf2xx_fb_read_mcu(const at86rf2xx_t *dev, uint8_t *data,
                      volatile uint8_t *fb_addr, size_t len)
{
    (void)dev;
    memcpy(data, (void *)fb_addr, len);
}

#endif

#define SPIDEV          (dev->params.spi)
#define CSPIN           (dev->params.cs_pin)

static inline void getbus(const at86rf2xx_t *dev)
{
    switch (dev->base.dev_type) {
        default:
#if AT86RF2XX_NEED_SPI
            spi_acquire(SPIDEV, CSPIN, SPI_MODE_0,
                        dev->params.spi_clk);
#else
            (void)dev;
#endif
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: { break; }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: { break; }
#endif
    }
}

uint8_t at86rf2xx_reg_read(const at86rf2xx_t *dev, uint8_t addr)
{
    uint8_t value = 0;
    switch (dev->base.dev_type) {
        default:;
#if AT86RF2XX_NEED_SPI
            uint8_t reg =
                (AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_READ | addr);
            getbus(dev);
            value = spi_transfer_reg(SPIDEV, CSPIN, reg, 0);
            spi_release(SPIDEV);
#else
            (void)dev;
            (void)addr;
#endif
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            return at86rf2xx_reg_read_mcu(dev, AT86RFA1_REG(addr));
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            return at86rf2xx_reg_read_mcu(dev, AT86RFR2_REG(addr));
            break;
        }
#endif
    }
    return value;
}

void at86rf2xx_reg_write(const at86rf2xx_t *dev, uint8_t addr, uint8_t value)
{
    switch (dev->base.dev_type) {
        default:;
#if AT86RF2XX_NEED_SPI
            uint8_t reg =
                (AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_WRITE | addr);
            getbus(dev);
            spi_transfer_reg(SPIDEV, CSPIN, reg, value);
            spi_release(SPIDEV);
#else
            (void)dev;
            (void)addr;
            (void)value;
#endif
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            at86rf2xx_reg_write_mcu(dev, AT86RFA1_REG(addr), value);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            at86rf2xx_reg_write_mcu(dev, AT86RFR2_REG(addr), value);
            break;
        }
#endif
    }
}

void at86rf2xx_sram_read(const at86rf2xx_t *dev, uint8_t offset,
                         uint8_t *data, size_t len)
{
    switch (dev->base.dev_type) {
        default:;
#if AT86RF2XX_NEED_SPI
            uint8_t reg = (AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_READ);
            getbus(dev);
            spi_transfer_byte(SPIDEV, CSPIN, true, reg);
            spi_transfer_byte(SPIDEV, CSPIN, true, offset);
            spi_transfer_bytes(SPIDEV, CSPIN, false, NULL, data, len);
            spi_release(SPIDEV);
#else
            (void)dev;
            (void)offset;
            (void)data;
            (void)len;
#endif
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            at86rf2xx_sram_read_mcu(dev, AT86RFA1_REG__TRXFBST,
                                    offset, data, len);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            at86rf2xx_sram_read_mcu(dev, AT86RFR2_REG__TRXFBST,
                                    offset, data, len);
            break;
        }
#endif
    }
}

void at86rf2xx_sram_write(const at86rf2xx_t *dev, uint8_t offset,
                          const uint8_t *data, size_t len)
{
    switch (dev->base.dev_type) {
        default:;
#if AT86RF2XX_NEED_SPI
            uint8_t reg = (AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_WRITE);
            getbus(dev);
            spi_transfer_byte(SPIDEV, CSPIN, true, reg);
            spi_transfer_byte(SPIDEV, CSPIN, true, offset);
            spi_transfer_bytes(SPIDEV, CSPIN, false, data, NULL, len);
            spi_release(SPIDEV);
#else
            (void)dev;
            (void)offset;
            (void)data;
            (void)len;
#endif
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            at86rf2xx_sram_write_mcu(dev, AT86RFA1_REG__TRXFBST,
                                     offset, data, len);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            at86rf2xx_sram_write_mcu(dev, AT86RFR2_REG__TRXFBST,
                                     offset, data, len);
            break;
        }
#endif
    }
}

void at86rf2xx_fb_start(const at86rf2xx_t *dev)
{
    switch (dev->base.dev_type) {
        default:;
#if AT86RF2XX_NEED_SPI
            uint8_t reg = AT86RF2XX_ACCESS_FB | AT86RF2XX_ACCESS_READ;
            getbus(dev);
            spi_transfer_byte(SPIDEV, CSPIN, true, reg);
#else
            (void)dev;
#endif
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: { break; }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: { break; }
#endif
    }
}

void at86rf2xx_fb_read(const at86rf2xx_t *dev,
                       uint8_t *data, size_t len)
{
    switch (dev->base.dev_type) {
        default:
#if AT86RF2XX_NEED_SPI
            spi_transfer_bytes(SPIDEV, CSPIN, true, NULL, data, len);
#else
            (void)dev;
            (void)data;
            (void)len;
#endif
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            at86rf2xx_fb_read_mcu(dev, data, AT86RFA1_REG__TRXFBST, len);
            break;
        }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            at86rf2xx_fb_read_mcu(dev, data, AT86RFR2_REG__TRXFBST, len);
            break;
        }
#endif
    }
}

void at86rf2xx_fb_stop(const at86rf2xx_t *dev)
{
    switch (dev->base.dev_type) {
        default:
#if AT86RF2XX_NEED_SPI
            /* transfer one byte (which we ignore) to release the chip select */
            spi_transfer_byte(SPIDEV, CSPIN, false, 1);
            spi_release(SPIDEV);
#else
            (void)dev;
#endif
            break;
#if IS_USED(MODULE_AT86RFA1)
        case AT86RF2XX_DEV_TYPE_AT86RFA1: { break; }
#endif
#if IS_USED(MODULE_AT86RFR2)
        case AT86RF2XX_DEV_TYPE_AT86RFR2: { break; }
#endif
    }
}

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
