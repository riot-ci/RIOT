/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Configuration of the MR-O-QPSK PHY on the AT86RF215 chip
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "at86rf215.h"
#include "at86rf215_internal.h"

#include "debug.h"

/* IEEE Std 802.15.4g™-2012 Amendment 3
 * Table 68d—Total number of channels and first channel center frequencies for SUN PHYs
 * Table 68e—Center frequencies for the MR-O-QPSK PHY operating in the 868–870 MHz band
 */

// FIXME: center frequency / spacing is not uniform in sub-GHz band
#define QPSK_CHANNEL_SPACING_SUBGHZ     (650U)     /* kHz */
#define QPSK_CENTER_FREQUENCY_SUBGHZ    (868300U)  /* Hz  */

#define QPSK_CHANNEL_SPACING_24GHZ      (5000U)    /* kHz */
#define QPSK_CENTER_FREQUENCY_24GHZ     (2350000U - CCF0_24G_OFFSET) /* Hz  */

/* Table 6-103. O-QPSK Transmitter Frontend Configuration */
static uint8_t _TXCUTC_PARAMP(uint8_t chips)
{
    switch (chips) {
    case BB_FCHIP100:
        return RF_PARAMP32U;
    case BB_FCHIP200:
        return RF_PARAMP16U;
    case BB_FCHIP1000:
    case BB_FCHIP2000:
        return RF_PARAMP4U;
    }

    return 0;
}

/* Table 6-103. O-QPSK Transmitter Frontend Configuration */
static uint8_t _TXCUTC_LPFCUT(uint8_t chips)
{
    switch (chips) {
    case BB_FCHIP100:
    case BB_FCHIP200:
        return RF_FLC400KHZ;
    case BB_FCHIP1000:
    case BB_FCHIP2000:
        return RF_FLC1000KHZ;
    }

    return 0;
}

/* Table 6-103. O-QPSK Transmitter Frontend Configuration */
static uint8_t _TXDFE_SR(uint8_t chips)
{
    switch (chips) {
    case BB_FCHIP100:
        return RF_SR_400K;
    case BB_FCHIP200:
        return RF_SR_800K;
    case BB_FCHIP1000:
        return RF_SR_4000K;
    case BB_FCHIP2000:
        return RF_SR_4000K;
    }

    return 0;
}

/* Table 6-103. O-QPSK Transmitter Frontend Configuration */
static uint8_t _TXDFE_RCUT(uint8_t chips)
{
    return (chips == BB_FCHIP2000 ? RF_RCUT_FS_BY_2 : RF_RCUT_FS_BY_2P6);
}

/* Table 6-105. O-QPSK Receiver Frontend Configuration (Filter Settings) */
static uint8_t _RXBWC_BW(uint8_t chips)
{
    switch (chips) {
    case BB_FCHIP100:
        return RF_BW160KHZ_IF250KHZ;
    case BB_FCHIP200:
        return RF_BW250KHZ_IF250KHZ;
    case BB_FCHIP1000:
        return RF_BW1000KHZ_IF1000KHZ;
    case BB_FCHIP2000:
        return RF_BW2000KHZ_IF2000KHZ;
    }

    return 0;
}

/* Table 6-105. O-QPSK Receiver Frontend Configuration (Filter Settings) */
static uint8_t _RXDFE_SR(uint8_t chips)
{
    switch (chips) {
    case BB_FCHIP100:
        return RF_SR_400K;
    case BB_FCHIP200:
        return RF_SR_800K;
    case BB_FCHIP1000:
    case BB_FCHIP2000:
        return RF_SR_4000K;
    }

    return 0;
}

/* Table 6-105. O-QPSK Receiver Frontend Configuration (Filter Settings) */
static uint8_t _RXDFE_RCUT(uint8_t chips)
{
    switch (chips) {
    case BB_FCHIP100:
    case BB_FCHIP200:
        return RF_RCUT_FS_BY_5P3;
    case BB_FCHIP1000:
        return RF_RCUT_FS_BY_8;
    case BB_FCHIP2000:
        return RF_RCUT_FS_BY_4;
    }

    return 0;
}

/* Table 6-106. O-QPSK Receiver Frontend Configuration (AGC Settings) */
static inline uint8_t _AGCC(uint8_t chips)
{
    if (chips > BB_FCHIP200) {
        return (2 << AGCC_AVGS_SHIFT) | AGCC_EN_MASK;
    } else {
        return AGCC_EN_MASK;
    }
}

/* Table 6-100. MR-O-QPSK Modes */
static uint32_t _get_bitrate(uint8_t chips, uint8_t mode)
{
    switch (chips) {
    case BB_FCHIP100:
        return  6250 * (1 << mode);
    case BB_FCHIP200:
        return 12500 * (1 << mode);
    case BB_FCHIP1000:
    case BB_FCHIP2000:
        return mode ? 125000 * (1 << (mode - 1)) : 31250;
    }

    return 0;
}

static void _set_legacy(at86rf215_t *dev, bool high_rate, uint8_t *chips, uint8_t *mode)
{
    /* enable/disable legacy high data rate */
    if (high_rate) {
        at86rf215_reg_or(dev, dev->BBC->RG_OQPSKC3, OQPSKC3_HRLEG_MASK);
    } else {
        at86rf215_reg_and(dev, dev->BBC->RG_OQPSKC3, ~OQPSKC3_HRLEG_MASK);
    }

    /* set the mode so that it results in the same data rate
       as the corresponding non-legacy mode to avoid having to
       distinguish between the two later on */
    if (is_subGHz(dev)) {
        *chips = BB_FCHIP1000;
        *mode  = high_rate
               ? (3 << 1) | AT86RF215_OQPSK_MODE_LEGACY
               : (2 << 1) | AT86RF215_OQPSK_MODE_LEGACY;
    } else {
        *chips = BB_FCHIP2000;
        *mode  = high_rate
               ? (3 << 1) | AT86RF215_OQPSK_MODE_LEGACY
               : (2 << 1) | AT86RF215_OQPSK_MODE_LEGACY;
    }
}

static uint8_t _set_mode(at86rf215_t *dev, uint8_t mode, bool legacy, uint8_t *chips)
{
    if (legacy) {
        _set_legacy(dev, mode > 0, chips, &mode);
    } else {
        /* mode 4 only supports 2000 kchip/s */
        if (mode == 4) {
            *chips = BB_FCHIP2000;
        }
        mode = AT86RF215_MR_OQPSK_MODE(mode);
    }

    /* TX with selected rate mode */
    at86rf215_reg_write(dev, dev->BBC->RG_OQPSKPHRTX, mode);

    /* power save mode only works when not listening to legacy frames */
    /* listening to both uses ~1mA more that just listening to legacy */
    uint8_t rxm = legacy ? RXM_LEGACY_OQPSK : RXM_MR_OQPSK;

    if (dev->flags & AT86RF215_OPT_RPC) {
        rxm |= OQPSKC2_RPC_MASK;                /* enable Reduced Power Consumption */
    }

    at86rf215_reg_write(dev, dev->BBC->RG_OQPSKC2,
                         rxm                    /* receive mode, legacy or MR-O-QPSK */
                       | OQPSKC2_FCSTLEG_MASK   /* 16 bit frame checksum */
                       | OQPSKC2_ENPROP_MASK);  /* enable Reduce Power Consumption */

    at86rf215_switch_mode(dev, legacy ? AT86RF215_MODE_LEGACY_OQPSK : AT86RF215_MODE_MR_OQPSK);
    return mode;
}

static void _set_chips(at86rf215_t *dev, uint8_t chips, uint8_t direct_modulation)
{
    /* Set Receiver Bandwidth */
    at86rf215_reg_write(dev, dev->RF->RG_RXBWC, _RXBWC_BW(chips));
    /* Set fS; fCUT for RX */
    at86rf215_reg_write(dev, dev->RF->RG_RXDFE, _RXDFE_SR(chips)
                                              | _RXDFE_RCUT(chips));
    /* Set Power Amplifier Ramp Time; fLPCUT */
    at86rf215_reg_write(dev, dev->RF->RG_TXCUTC, _TXCUTC_PARAMP(chips)
                                               | _TXCUTC_LPFCUT(chips));
    /* Set fS; fCUT for TX */
    at86rf215_reg_write(dev, dev->RF->RG_TXDFE, _TXDFE_SR(chips)
                                              | _TXDFE_RCUT(chips)
                                              | direct_modulation);

    /* set receiver gain target according to data sheet */
    at86rf215_reg_write(dev, dev->RF->RG_AGCS, 3 << AGCS_TGT_SHIFT);
    at86rf215_reg_write(dev, dev->RF->RG_AGCC, _AGCC(chips));

    /* use RC-0.8 shaping */
    at86rf215_reg_write(dev, dev->BBC->RG_OQPSKC0, chips | direct_modulation);
}

static void _set_ack_timeout(at86rf215_t *dev, uint8_t chips, uint8_t mode)
{
    dev->ack_timeout_usec = 10 * AT86RF215_ACK_PERIOD_IN_BITS * 1000000UL / _get_bitrate(chips, mode >> 1);
    DEBUG("[%s] ACK timeout: %"PRIu32" µs\n", "O-QPSK", dev->ack_timeout_usec);
}

int at86rf215_configure_OQPSK(at86rf215_t *dev, uint8_t chips, uint8_t mode)
{
    uint8_t direct_modulation;
    direct_modulation = 0;

    if (chips > BB_FCHIP2000) {
        DEBUG("[%s] invalid chips: %d\n", __func__, chips);
        return -EINVAL;
    }

    if ((mode & ~IEEE802154_OQPSK_FLAG_LEGACY) > 4) {
        DEBUG("[%s] invalid mode: %d\n", __func__, mode);
        return -EINVAL;
    }

    at86rf215_await_state_end(dev, RF_STATE_TX);

    /* disable radio */
    at86rf215_reg_write(dev, dev->BBC->RG_PC, 0);

    mode = _set_mode(dev,
                     mode & ~IEEE802154_OQPSK_FLAG_LEGACY,
                     mode &  IEEE802154_OQPSK_FLAG_LEGACY,
                     &chips);

    /* enable direct modulation if the chip supports it */
    if (chips < BB_FCHIP1000 &&
               at86rf215_reg_read(dev, RG_RF_VN) >= 3) {
        /* it's bit 4 in both TXDFE & OQPSKC0 */
        direct_modulation = 1 << 4;
    }

    _set_chips(dev, chips, direct_modulation);

    /* set channel spacing */
    if (is_subGHz(dev)) {
        at86rf215_reg_write(dev, dev->RF->RG_CS, QPSK_CHANNEL_SPACING_SUBGHZ / 25);
        at86rf215_reg_write16(dev, dev->RF->RG_CCF0L, QPSK_CENTER_FREQUENCY_SUBGHZ / 25);
    } else {
        at86rf215_reg_write(dev, dev->RF->RG_CS, QPSK_CHANNEL_SPACING_24GHZ / 25);
        at86rf215_reg_write16(dev, dev->RF->RG_CCF0L, QPSK_CENTER_FREQUENCY_24GHZ / 25);
    }

    /* lowest preamble detection sensitivity */
    at86rf215_reg_write(dev, dev->BBC->RG_OQPSKC1, 0);

    /* legacy O-QPSK & listen to sync word SFD_1 */
    at86rf215_reg_write(dev, dev->BBC->RG_OQPSKC3, 0x0);

    /* make sure the channel config is still valid */
    dev->num_chans = is_subGHz(dev) ? 3 : 16;
    dev->netdev.chan = at86rf215_chan_valid(dev, dev->netdev.chan);
    at86rf215_reg_write16(dev, dev->RF->RG_CNL, dev->netdev.chan);

    _set_ack_timeout(dev, chips, mode);

    at86rf215_enable_radio(dev, BB_MROQPSK);

    return 0;
}

int at86rf215_OQPSK_set_chips(at86rf215_t *dev, uint8_t chips)
{
    uint8_t direct_modulation, mode;

    mode = at86rf215_reg_read(dev, dev->BBC->RG_OQPSKPHRTX);

    if (mode & AT86RF215_OQPSK_MODE_LEGACY) {
        DEBUG("[%s] can't set chip rate in legacy mode\n", __func__);
        return -1;
    }

    if (chips < BB_FCHIP1000 && at86rf215_reg_read(dev, RG_RF_VN) >= 3) {
        direct_modulation = 1 << 4;
    } else {
        direct_modulation = 0;
    }

    at86rf215_await_state_end(dev, RF_STATE_TX);

    _set_chips(dev, chips, direct_modulation);
    _set_ack_timeout(dev, chips, mode);
    return 0;
}

uint8_t at86rf215_OQPSK_get_chips(at86rf215_t *dev)
{
    return at86rf215_reg_read(dev, dev->BBC->RG_OQPSKC0) & OQPSKC0_FCHIP_MASK;
}

int at86rf215_OQPSK_set_mode(at86rf215_t *dev, uint8_t mode)
{
    uint8_t chips;

    if ((mode & ~IEEE802154_OQPSK_FLAG_LEGACY) > 4) {
        return -1;
    }

    chips = at86rf215_OQPSK_get_chips(dev);

    at86rf215_await_state_end(dev, RF_STATE_TX);

    mode = _set_mode(dev,
                     mode & ~IEEE802154_OQPSK_FLAG_LEGACY,
                     mode &  IEEE802154_OQPSK_FLAG_LEGACY,
                     &chips);

    _set_ack_timeout(dev, chips, mode);

    return 0;
}

uint8_t at86rf215_OQPSK_get_mode(at86rf215_t *dev)
{
    uint8_t mode = at86rf215_reg_read(dev, dev->BBC->RG_OQPSKPHRTX);

    if (mode & AT86RF215_OQPSK_MODE_LEGACY) {
        if (at86rf215_reg_read(dev, dev->BBC->RG_OQPSKC3) & OQPSKC3_HRLEG_MASK) {
            /* legacy rate modes for at86rf233 & at86rf212b */
            if (at86rf215_OQPSK_get_chips(dev) == BB_FCHIP2000) {
                return IEEE802154_OQPSK_FLAG_LEGACY | 2;
            } else {
                return IEEE802154_OQPSK_FLAG_LEGACY | 1;
            }
        } else {
            return IEEE802154_OQPSK_FLAG_LEGACY;
        }
    }

    return (mode & OQPSKPHRTX_MOD_MASK) >> OQPSKPHRTX_MOD_SHIFT;
}
