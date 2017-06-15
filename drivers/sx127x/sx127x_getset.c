/*
 * Copyright (C) 2016 Unwired Devices <info@unwds.com>
 *               2017 Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx127x
 * @{
 * @file
 * @brief       Implementation of get and set functions for SX127X
 *
 * @author      Eugene P. <ep@unwds.com>
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "sx127x.h"
#include "sx127x_registers.h"
#include "sx127x_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


uint8_t sx127x_get_state(sx127x_t *dev)
{
    return dev->settings.state;
}

void sx127x_set_state(sx127x_t *dev, uint8_t state)
{
    DEBUG("[DEBUG] Change state: %d\n", state);

    dev->settings.state = state;
}

void sx127x_set_modem(sx127x_t *dev, uint8_t modem)
{
    DEBUG("[DEBUG] set modem: %d\n", modem);

    dev->settings.modem = modem;

    switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            sx127x_set_op_mode(dev, SX127X_RF_OPMODE_SLEEP);
            sx127x_reg_write(dev, SX127X_REG_OPMODE,
                             (sx127x_reg_read(dev, SX127X_REG_OPMODE) &
                              SX127X_RF_LORA_OPMODE_LONGRANGEMODE_MASK) |
                             SX127X_RF_LORA_OPMODE_LONGRANGEMODE_OFF);

            sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1, 0x00);
            sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1, 0x30);
            break;
        case SX127X_MODEM_LORA:
            sx127x_set_op_mode(dev, SX127X_RF_OPMODE_SLEEP);
            sx127x_reg_write(dev, SX127X_REG_OPMODE,
                             (sx127x_reg_read(dev, SX127X_REG_OPMODE) &
                              SX127X_RF_LORA_OPMODE_LONGRANGEMODE_MASK) |
                             SX127X_RF_LORA_OPMODE_LONGRANGEMODE_ON);

            sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1, 0x00);
            sx127x_reg_write(dev, SX127X_REG_DIOMAPPING2, 0x00);
            break;
        default:
            break;
    }
}

uint8_t sx127x_get_syncword(sx127x_t *dev)
{
    return sx127x_reg_read(dev, SX127X_REG_LR_SYNCWORD);
}

void sx127x_set_syncword(sx127x_t *dev, uint8_t syncword)
{
    DEBUG("[DEBUG] Set syncword: %d\n", syncword);

    sx127x_reg_write(dev, SX127X_REG_LR_SYNCWORD, syncword);
}

uint32_t sx127x_get_channel(sx127x_t *dev)
{
    return (((uint32_t)sx127x_reg_read(dev, SX127X_REG_FRFMSB) << 16) |
            (sx127x_reg_read(dev, SX127X_REG_FRFMID) << 8) |
            (sx127x_reg_read(dev, SX127X_REG_FRFLSB))) * SX127X_FREQUENCY_RESOLUTION;
}

void sx127x_set_channel(sx127x_t *dev, uint32_t channel)
{
    DEBUG("[DEBUG] Set channel: %lu\n", channel);

    /* Save current operating mode */
    dev->settings.channel = channel;
    uint8_t prev_mode = sx127x_reg_read(dev, SX127X_REG_OPMODE);

    sx127x_set_op_mode(dev, SX127X_RF_OPMODE_STANDBY);

    channel = (uint32_t)((double) channel / (double) SX127X_FREQUENCY_RESOLUTION);

    /* Write frequency settings into chip */
    sx127x_reg_write(dev, SX127X_REG_FRFMSB, (uint8_t)((channel >> 16) & 0xFF));
    sx127x_reg_write(dev, SX127X_REG_FRFMID, (uint8_t)((channel >> 8) & 0xFF));
    sx127x_reg_write(dev, SX127X_REG_FRFLSB, (uint8_t)(channel & 0xFF));

    /* Restore previous operating mode */
    sx127x_reg_write(dev, SX127X_REG_OPMODE, prev_mode);
}

uint32_t sx127x_get_time_on_air(sx127x_t *dev)
{
    uint32_t air_time = 0;

    uint8_t pkt_len = dev->settings.time_on_air_pkt_len;
    switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            air_time = round(
                            (8 * (dev->settings.fsk.preamble_len +
                                  ((sx127x_reg_read(dev, SX127X_REG_SYNCCONFIG) &
                                    ~SX127X_RF_SYNCCONFIG_SYNCSIZE_MASK) + 1) +
                                  ((dev->settings.fsk.fix_len == 0x01) ? 0.0 : 1.0 ) +
                                  (((sx127x_reg_read(dev, SX127X_REG_PACKETCONFIG1) &
                                     ~SX127X_RF_PACKETCONFIG1_ADDRSFILTERING_MASK )
                                    != 0x00 ) ? 1.0 : 0 ) + pkt_len +
                                  ((dev->settings.fsk.crc_on == 0x01) ? 2.0 : 0)
                                 ) / dev->settings.fsk.datarate) * 1e3);
            break;
        case SX127X_MODEM_LORA:
        {
            double bw = 0.0;

            /* Note: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported. */
            switch (dev->settings.lora.bandwidth) {
                case SX127X_BW_125_KHZ:
                    bw = 125e3;
                    break;
                case SX127X_BW_250_KHZ:
                    bw = 250e3;
                    break;
                case SX127X_BW_500_KHZ:
                    bw = 500e3;
                    break;
                default:
                    DEBUG("Invalid bandwith: %d\n", dev->settings.lora.bandwidth);
                    break;
            }

            /* Symbol rate : time for one symbol [secs] */
            double rs = bw / (1 << dev->settings.lora.datarate);
            double ts = 1 / rs;

            /* time of preamble */
            double t_preamble = (dev->settings.lora.preamble_len + 4.25) * ts;

            /* Symbol length of payload and time */
            double tmp =
                ceil(
                    (8 * pkt_len - 4 * dev->settings.lora.datarate + 28
                     + 16 * dev->settings.lora.crc_on
                     - (!dev->settings.lora.use_fix_len ? 20 : 0))
                    / (double) (4 * dev->settings.lora.datarate
                                - ((dev->settings.lora.low_datarate_optimize
                                    > 0) ? 2 : 0)))
                * (dev->settings.lora.coderate + 4);
            double n_payload = 8 + ((tmp > 0) ? tmp : 0);
            double t_payload = n_payload * ts;

            /* Time on air */
            double t_on_air = t_preamble + t_payload;

            /* return seconds */
            air_time = floor(t_on_air * 1e3 + 0.999);
        }
        break;
    }

    return air_time;
}

void sx127x_set_sleep(sx127x_t *dev)
{
    DEBUG("[DEBUG] Set sleep\n");

    /* Disable running timers */
    xtimer_remove(&dev->_internal.tx_timeout_timer);
    xtimer_remove(&dev->_internal.rx_timeout_timer);

    /* Put chip into sleep */
    sx127x_set_op_mode(dev, SX127X_RF_OPMODE_SLEEP);
    sx127x_set_state(dev,  SX127X_RF_IDLE);
}

void sx127x_set_standby(sx127x_t *dev)
{
    DEBUG("[DEBUG] Set standby\n");

    /* Disable running timers */
    xtimer_remove(&dev->_internal.tx_timeout_timer);
    xtimer_remove(&dev->_internal.rx_timeout_timer);

    sx127x_set_op_mode(dev, SX127X_RF_OPMODE_STANDBY);
    sx127x_set_state(dev,  SX127X_RF_IDLE);
}

void sx127x_set_rx(sx127x_t *dev)
{
    DEBUG("[DEBUG] Set RX\n");

    switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1,
                             (sx127x_reg_read(dev, SX127X_REG_DIOMAPPING1 ) &
                              SX127X_RF_DIOMAPPING1_DIO0_MASK &
                              SX127X_RF_DIOMAPPING1_DIO1_MASK &
                              SX127X_RF_DIOMAPPING1_DIO2_MASK ) |
                             SX127X_RF_DIOMAPPING1_DIO0_00 |
                             SX127X_RF_DIOMAPPING1_DIO1_00 |
                             SX127X_RF_DIOMAPPING1_DIO2_11);

            sx127x_reg_write(dev, SX127X_REG_DIOMAPPING2,
                             (sx127x_reg_read(dev, SX127X_REG_DIOMAPPING2 ) &
                              SX127X_RF_DIOMAPPING2_DIO4_MASK &
                              SX127X_RF_DIOMAPPING2_MAP_MASK ) |
                             SX127X_RF_DIOMAPPING2_DIO4_11 |
                             SX127X_RF_DIOMAPPING2_MAP_PREAMBLEDETECT );

            dev->settings.fsk_packet_handler.fifo_threshold = sx127x_reg_read(dev, SX127X_REG_FIFOTHRESH) & 0x3F;

            sx127x_reg_write(dev, SX127X_REG_RXCONFIG,
                             SX127X_RF_RXCONFIG_AFCAUTO_ON |
                             SX127X_RF_RXCONFIG_AGCAUTO_ON |
                             SX127X_RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT);

            dev->settings.fsk_packet_handler.preamble_detected = false;
            dev->settings.fsk_packet_handler.sync_word_detected = false;
            dev->settings.fsk_packet_handler.nb_bytes = 0;
            dev->settings.fsk_packet_handler.size = 0;
            return;

        case SX127X_MODEM_LORA:
        {
            if (dev->settings.lora.iq_inverted) {
                sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ,
                                 ((sx127x_reg_read(dev, SX127X_REG_LR_INVERTIQ)
                                   & SX127X_RF_LORA_INVERTIQ_TX_MASK & SX127X_RF_LORA_INVERTIQ_RX_MASK)
                                  | SX127X_RF_LORA_INVERTIQ_RX_ON | SX127X_RF_LORA_INVERTIQ_TX_OFF));
                sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ2, SX127X_RF_LORA_INVERTIQ2_ON);
            }
            else {
                sx127x_reg_write(dev,
                                 SX127X_REG_LR_INVERTIQ,
                                 ((sx127x_reg_read(dev, SX127X_REG_LR_INVERTIQ)
                                   & SX127X_RF_LORA_INVERTIQ_TX_MASK & SX127X_RF_LORA_INVERTIQ_RX_MASK)
                                  | SX127X_RF_LORA_INVERTIQ_RX_OFF | SX127X_RF_LORA_INVERTIQ_TX_OFF));
                sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ2, SX127X_RF_LORA_INVERTIQ2_OFF);
            }

#if defined(MODULE_SX1276)
            /* ERRATA 2.3 - Receiver Spurious Reception of a LoRa Signal */
            if (dev->settings.lora.bandwidth < 9) {
                sx127x_reg_write(dev, SX127X_REG_LR_DETECTOPTIMIZE,
                                 sx127x_reg_read(dev, SX127X_REG_LR_DETECTOPTIMIZE) & 0x7F);
                sx127x_reg_write(dev, SX127X_REG_LR_TEST30, 0x00);
                switch (dev->settings.lora.bandwidth) {
                    case SX127X_BW_125_KHZ: /* 125 kHz */
                        sx127x_reg_write(dev, SX127X_REG_LR_TEST2F, 0x40);
                        break;
                    case SX127X_BW_250_KHZ: /* 250 kHz */
                        sx127x_reg_write(dev, SX127X_REG_LR_TEST2F, 0x40);
                        break;

                    default:
                        break;
                }
            }
            else {
                sx127x_reg_write(dev, SX127X_REG_LR_DETECTOPTIMIZE,
                                 sx127x_reg_read(dev, SX127X_REG_LR_DETECTOPTIMIZE) | 0x80);
            }
#endif

            /* Setup interrupts */
            if (dev->settings.lora.freq_hop_on) {
                sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGSMASK,
                                 /* SX127X_RF_LORA_IRQFLAGS_RXTIMEOUT |
                                    SX127X_RF_LORA_IRQFLAGS_RXDONE |
                                    SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR | */
                                 SX127X_RF_LORA_IRQFLAGS_VALIDHEADER |
                                 SX127X_RF_LORA_IRQFLAGS_TXDONE |
                                 SX127X_RF_LORA_IRQFLAGS_CADDONE |
                                 /* SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL | */
                                 SX127X_RF_LORA_IRQFLAGS_CADDETECTED);

                /* DIO0=RxDone, DIO2=FhssChangeChannel */
                sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1,
                                 (sx127x_reg_read(dev, SX127X_REG_DIOMAPPING1) &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO0_MASK &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO2_MASK) |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO0_00 |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO2_00);
            }
            else {
                sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGSMASK,
                                 /* SX127X_RF_LORA_IRQFLAGS_RXTIMEOUT |
                                    SX127X_RF_LORA_IRQFLAGS_RXDONE |
                                    SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR | */
                                 SX127X_RF_LORA_IRQFLAGS_VALIDHEADER |
                                 SX127X_RF_LORA_IRQFLAGS_TXDONE |
                                 SX127X_RF_LORA_IRQFLAGS_CADDONE |
                                 SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                 SX127X_RF_LORA_IRQFLAGS_CADDETECTED);

                /* DIO0=RxDone */
                sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1,
                                 (sx127x_reg_read(dev, SX127X_REG_DIOMAPPING1) &
                                  SX127X_RF_LORA_DIOMAPPING1_DIO0_MASK) |
                                 SX127X_RF_LORA_DIOMAPPING1_DIO0_00);
            }

            sx127x_reg_write(dev, SX127X_REG_LR_FIFORXBASEADDR, 0);
            sx127x_reg_write(dev, SX127X_REG_LR_FIFOADDRPTR, 0);
        }
        break;
    }

    sx127x_set_state(dev, SX127X_RF_RX_RUNNING);
    if (dev->settings.window_timeout != 0) {
        xtimer_set(&(dev->_internal.rx_timeout_timer),
                   dev->settings.window_timeout);
    }

    if (dev->settings.lora.rx_continuous) {
        sx127x_set_op_mode(dev, SX127X_RF_LORA_OPMODE_RECEIVER);
    }
    else {
        sx127x_set_op_mode(dev, SX127X_RF_LORA_OPMODE_RECEIVER_SINGLE);
    }
}

void sx127x_set_max_payload_len(sx127x_t *dev, uint8_t maxlen)
{
    DEBUG("[DEBUG] Set max payload len: %d\n", maxlen);

    switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            sx127x_reg_write(dev, SX127X_REG_PAYLOADLENGTH, maxlen);
            break;

        case SX127X_MODEM_LORA:
            sx127x_reg_write(dev, SX127X_REG_LR_PAYLOADMAXLENGTH, maxlen);
            break;
    }
}

uint8_t sx127x_get_op_mode(sx127x_t *dev)
{
    return sx127x_reg_read(dev, SX127X_REG_OPMODE) & ~SX127X_RF_OPMODE_MASK;
}

void sx127x_set_op_mode(sx127x_t *dev, uint8_t op_mode)
{
    DEBUG("[DEBUG] Set op mode: %d\n", op_mode);

    /* Replace previous mode value and setup new mode value */
    sx127x_reg_write(dev, SX127X_REG_OPMODE,
                     (sx127x_reg_read(dev, SX127X_REG_OPMODE) & SX127X_RF_OPMODE_MASK) | op_mode);
}

uint8_t sx127x_get_bandwidth(sx127x_t *dev)
{
    return dev->settings.lora.bandwidth;
}

inline void _low_datarate_optimize(sx127x_t *dev)
{
    if ( ((dev->settings.lora.bandwidth == SX127X_BW_125_KHZ) &&
          ((dev->settings.lora.datarate == SX127X_SF11) ||
           (dev->settings.lora.datarate == SX127X_SF12))) ||
         ((dev->settings.lora.bandwidth == SX127X_BW_250_KHZ) &&
          (dev->settings.lora.datarate == SX127X_SF12))) {
        dev->settings.lora.low_datarate_optimize = 0x01;
    } else {
        dev->settings.lora.low_datarate_optimize = 0x00;
    }

#if defined(MODULE_SX1276)
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG3,
                     (sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG3) &
                      SX127X_RF_LORA_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK) |
                     (dev->settings.lora.low_datarate_optimize << 3));
#endif
#if defined(MODULE_SX1272)
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG1,
                     (sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1) &
                      SX127X_RF_LORA_MODEMCONFIG1_LOWDATARATEOPTIMIZE_MASK) |
                     (dev->settings.lora.low_datarate_optimize));
#endif
}

inline void _update_bandwidth(sx127x_t *dev)
{
    uint8_t config1_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1);
#if defined(MODULE_SX1276)
    config1_reg &= SX1276_RF_LORA_MODEMCONFIG1_BW_MASK;
    switch (dev->settings.lora.bandwidth) {
    case SX127X_BW_125_KHZ:
        config1_reg |= SX1276_RF_LORA_MODEMCONFIG1_BW_125_KHZ;
        break;
    case SX127X_BW_250_KHZ:
        config1_reg |=  SX1276_RF_LORA_MODEMCONFIG1_BW_250_KHZ;
        break;
    case SX127X_BW_500_KHZ:
        config1_reg |=  SX1276_RF_LORA_MODEMCONFIG1_BW_500_KHZ;
        break;
    default:
        DEBUG("Unsupported bandwidth, %d", dev->settings.lora.bandwidth);
        break;
    }
#endif
#if defined(MODULE_SX1272)
    config1_reg &= SX1272_RF_LORA_MODEMCONFIG1_BW_MASK;
    switch (dev->settings.lora.bandwidth) {
    case SX127X_BW_125_KHZ:
        config1_reg |=  SX1272_RF_LORA_MODEMCONFIG1_BW_125_KHZ;
        break;
    case SX127X_BW_250_KHZ:
        config1_reg |=  SX1272_RF_LORA_MODEMCONFIG1_BW_250_KHZ;
        break;
    case SX127X_BW_500_KHZ:
        config1_reg |=  SX1272_RF_LORA_MODEMCONFIG1_BW_500_KHZ;
        break;
    default:
        DEBUG("Unsupported bandwidth, %d", dev->settings.lora.bandwidth);
        break;
    }
#endif
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG1, config1_reg);
}

void sx127x_set_bandwidth(sx127x_t *dev, uint8_t bandwidth)
{
    DEBUG("[DEBUG] Set bandwidth: %d\n", bandwidth);

    dev->settings.lora.bandwidth = bandwidth;

    _update_bandwidth(dev);

    _low_datarate_optimize(dev);

    /* ERRATA sensitivity tweaks */
    if ((dev->settings.lora.bandwidth == SX127X_BW_500_KHZ) && (SX127X_RF_MID_BAND_THRESH)) {
        /* ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth */
        sx127x_reg_write(dev, SX127X_REG_LR_TEST36, 0x02);
        sx127x_reg_write(dev, SX127X_REG_LR_TEST3A, 0x64);
    }
    else if (dev->settings.lora.bandwidth == SX127X_BW_500_KHZ) {
        /* ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth */
        sx127x_reg_write(dev, SX127X_REG_LR_TEST36, 0x02);
        sx127x_reg_write(dev, SX127X_REG_LR_TEST3A, 0x7F);
    }
    else {
        /* ERRATA 2.1 - Sensitivity Optimization with another Bandwidth */
        sx127x_reg_write(dev, SX127X_REG_LR_TEST36, 0x03);
    }
}

uint8_t sx127x_get_spreading_factor(sx127x_t *dev)
{
    return dev->settings.lora.datarate;
}

void sx127x_set_spreading_factor(sx127x_t *dev, uint8_t datarate)
{
    DEBUG("[DEBUG] Set spreading factor: %d\n", sf);

    if (datarate == SX127X_SF6 && !dev->settings.lora.use_fix_len) {
        /* SF 6 is only valid when using explicit header mode */
        DEBUG("Spreading Factor 6 can only be used when explicit header "
              "mode is set, this mode is not supported by this driver."
              "Ignoring.\n");
        return;
    }

    dev->settings.lora.datarate = datarate;

    uint8_t config2_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG2);
    config2_reg &= SX127X_RF_LORA_MODEMCONFIG2_SF_MASK;
    config2_reg |= datarate << 4;
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG2, config2_reg);

    _low_datarate_optimize(dev);

    switch(dev->settings.lora.datarate) {
    case SX127X_SF6:
        sx127x_reg_write(dev, SX127X_REG_LR_DETECTOPTIMIZE,
                         SX127X_RF_LORA_DETECTIONOPTIMIZE_SF6);
        sx127x_reg_write(dev, SX127X_REG_LR_DETECTIONTHRESHOLD,
                         SX127X_RF_LORA_DETECTIONTHRESH_SF6);
        break;
    default:
        sx127x_reg_write(dev, SX127X_REG_LR_DETECTOPTIMIZE,
                         SX127X_RF_LORA_DETECTIONOPTIMIZE_SF7_TO_SF12);
        sx127x_reg_write(dev, SX127X_REG_LR_DETECTIONTHRESHOLD,
                         SX127X_RF_LORA_DETECTIONTHRESH_SF7_TO_SF12);
        break;
    }
}

uint8_t sx127x_get_coding_rate(sx127x_t *dev)
{
    return dev->settings.lora.coderate;
}

void sx127x_set_coding_rate(sx127x_t *dev, uint8_t coderate)
{
    DEBUG("[DEBUG] Set coding rate: %d\n", coderate);

    dev->settings.lora.coderate = coderate;
    uint8_t config1_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1);

#if defined(MODULE_SX1276)
    config1_reg &= SX1276_RF_LORA_MODEMCONFIG1_CODINGRATE_MASK;
    config1_reg |= coderate << 1;
#endif
#if defined(MODULE_SX1272)
    config1_reg &= SX1272_RF_LORA_MODEMCONFIG1_CODINGRATE_MASK;
    config1_reg |= coderate << 3;
#endif

    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG1, config1_reg);
}

uint8_t sx127x_get_rx_single(sx127x_t *dev)
{
    return dev->settings.lora.rx_continuous ? false : true;
}

void sx127x_set_rx_single(sx127x_t *dev, uint8_t single)
{
    DEBUG("[DEBUG] Set RX single: %d\n", single);

    dev->settings.lora.rx_continuous = single ? false : true;
}

bool sx127x_get_crc(sx127x_t *dev)
{
#if defined(MODULE_SX1276)
    return (sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG2) &
            SX1276_RF_LORA_MODEMCONFIG2_RXPAYLOADCRC_MASK);
#endif
#if defined(MODULE_SX1272)
    return (sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1) &
            SX1272_RF_LORA_MODEMCONFIG1_RXPAYLOADCRC_MASK);
#endif
}

void sx127x_set_crc(sx127x_t *dev, bool crc)
{
    DEBUG("[DEBUG] Set CRC: %d\n", crc);

    dev->settings.lora.crc_on = crc;
#if defined(MODULE_SX1276)
    uint8_t config2_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG2);
    config2_reg &= SX1276_RF_LORA_MODEMCONFIG2_RXPAYLOADCRC_MASK;
    config2_reg |= crc << 2;
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG2, config2_reg);
#endif
#if defined(MODULE_SX1272)
    uint8_t config2_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1);
    config2_reg &= SX1272_RF_LORA_MODEMCONFIG1_RXPAYLOADCRC_MASK;
    config2_reg |= crc << 1;
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG1, config2_reg);
#endif
}

uint8_t sx127x_get_hop_period(sx127x_t *dev)
{
    return sx127x_reg_read(dev, SX127X_REG_LR_HOPPERIOD);
}

void sx127x_set_hop_period(sx127x_t *dev, uint8_t hop_period)
{
    DEBUG("[DEBUG] Set Hop period: %d\n", hop_period);

    dev->settings.lora.freq_hop_period = hop_period;

    uint8_t tmp = sx127x_reg_read(dev, SX127X_REG_LR_PLLHOP);
    if (dev->settings.lora.freq_hop_on) {
        tmp |= SX127X_RF_LORA_PLLHOP_FASTHOP_ON;
        sx127x_reg_write(dev, SX127X_REG_LR_PLLHOP, tmp);
        sx127x_reg_write(dev, SX127X_REG_LR_HOPPERIOD, hop_period);
    }
}

bool  sx127x_get_fixed_header_len_mode(sx127x_t *dev)
{
    return dev->settings.lora.use_fix_len;
}

void sx127x_set_fixed_header_len_mode(sx127x_t *dev, bool fixed_len)
{
    DEBUG("[DEBUG] Set fixed header length: %d\n", fixed_len);

    dev->settings.lora.use_fix_len = fixed_len;

    uint8_t config1_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG1);
#if defined(MODULE_SX1276)
    config1_reg &= SX1276_RF_LORA_MODEMCONFIG1_IMPLICITHEADER_MASK;
    config1_reg |= fixed_len;
#endif
#if defined(MODULE_SX1272)
    config1_reg &= SX1272_RF_LORA_MODEMCONFIG1_IMPLICITHEADER_MASK;
    config1_reg |= fixed_len << 2;
#endif
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG1, config1_reg);
}

uint8_t sx127x_get_payload_length(sx127x_t *dev)
{
    return dev->settings.lora.payload_len;
}

void sx127x_set_payload_length(sx127x_t *dev, uint8_t len)
{
    DEBUG("[DEBUG] Set payload len: %d\n", len);

    if (dev->settings.lora.use_fix_len) {
        dev->settings.lora.payload_len = len;
        sx127x_reg_write(dev, SX127X_REG_LR_PAYLOADLENGTH, len);
    }
}

static inline uint8_t sx127x_get_pa_select(uint32_t channel)
{
#if defined(MODULE_SX1272)
    (void) channel;
    return SX127X_RF_PACONFIG_PASELECT_PABOOST;
#endif
#if defined(MODULE_SX1276)
    if (channel < SX127X_RF_MID_BAND_THRESH) {
        return SX127X_RF_PACONFIG_PASELECT_PABOOST;
    }
    else {
        return SX127X_RF_PACONFIG_PASELECT_RFO;
    }
#endif
}

uint8_t sx127x_get_power(sx127x_t *dev)
{
    return dev->settings.lora.power;
}

void sx127x_set_tx_power(sx127x_t *dev, uint8_t power)
{
    DEBUG("[DEBUG] Set power: %d\n", power);

    dev->settings.lora.power = power;

    uint8_t pa_config = sx127x_reg_read(dev, SX127X_REG_PACONFIG);
#if defined(MODULE_SX1276)
    uint8_t pa_dac = sx127x_reg_read(dev, SX1276_REG_PADAC);
#endif
#if defined(MODULE_SX1272)
    uint8_t pa_dac = sx127x_reg_read(dev, SX1272_REG_PADAC);
#endif

    pa_config = ((pa_config & SX127X_RF_PACONFIG_PASELECT_MASK) |
                 sx127x_get_pa_select(dev->settings.channel));
#if defined(MODULE_SX1276)
    /* max power is 14dBm */
    pa_config = (pa_config & SX127X_RF_PACONFIG_MAX_POWER_MASK) | 0x70;
#endif

    sx127x_reg_write(dev, SX127X_REG_PARAMP, SX127X_RF_PARAMP_0050_US);

    if ((pa_config & SX127X_RF_PACONFIG_PASELECT_PABOOST)
        == SX127X_RF_PACONFIG_PASELECT_PABOOST) {
        if (power > 17) {
            pa_dac = ((pa_dac & SX127X_RF_PADAC_20DBM_MASK) |
                      SX127X_RF_PADAC_20DBM_ON);
        } else {
            pa_dac = ((pa_dac & SX127X_RF_PADAC_20DBM_MASK) |
                      SX127X_RF_PADAC_20DBM_OFF);
        }
        if ((pa_dac & SX127X_RF_PADAC_20DBM_ON) == SX127X_RF_PADAC_20DBM_ON) {
            if (power < 5) {
                power = 5;
            }
            if (power > 20) {
                power = 20;
            }

            pa_config = ((pa_config & SX127X_RF_PACONFIG_OUTPUTPOWER_MASK) |
                         (uint8_t)((uint16_t)(power - 5) & 0x0F));
        } else {
            if (power < 2) {
                power = 2;
            }
            if (power > 17) {
                power = 17;
            }

            pa_config = ((pa_config & SX127X_RF_PACONFIG_OUTPUTPOWER_MASK) |
                         (uint8_t)((uint16_t)(power - 2) & 0x0F));
        }
    } else {
        if (power < -1) {
            power = -1;
        }
        if (power > 14) {
            power = 14;
        }

        pa_config = ((pa_config & SX127X_RF_PACONFIG_OUTPUTPOWER_MASK) |
                     (uint8_t)((uint16_t)(power + 1) & 0x0F));
    }

    sx127x_reg_write(dev, SX127X_REG_PACONFIG, pa_config);
#if defined(MODULE_SX1276)
    sx127x_reg_write(dev, SX1276_REG_PADAC, pa_dac);
#endif
#if defined(MODULE_SX1272)
    sx127x_reg_write(dev, SX1272_REG_PADAC, pa_dac);
#endif
}

uint16_t sx127x_get_preamble_length(sx127x_t *dev)
{
    return dev->settings.lora.preamble_len;
}

void sx127x_set_preamble_length(sx127x_t *dev, uint16_t preamble)
{
    DEBUG("[DEBUG] Set preamble length: %d\n", preamble);

    dev->settings.lora.preamble_len = preamble;

    sx127x_reg_write(dev, SX127X_REG_LR_PREAMBLEMSB,
                     (preamble >> 8) & 0xFF);
    sx127x_reg_write(dev, SX127X_REG_LR_PREAMBLELSB,
                     preamble & 0xFF);
}

void sx127x_set_rx_timeout(sx127x_t *dev, uint32_t timeout)
{
    DEBUG("[DEBUG] Set RX timeout: %lu\n", timeout);

    dev->settings.lora.rx_timeout = timeout;
}

void sx127x_set_tx_timeout(sx127x_t *dev, uint32_t timeout)
{
    DEBUG("[DEBUG] Set TX timeout: %lu\n", timeout);

    dev->settings.lora.tx_timeout = timeout;
}

void sx127x_set_symbol_timeout(sx127x_t *dev, uint16_t timeout)
{
    DEBUG("[DEBUG] Set symbol timeout: %d\n", timeout);

    dev->settings.lora.rx_timeout = timeout;

    uint8_t config2_reg = sx127x_reg_read(dev, SX127X_REG_LR_MODEMCONFIG2);
    config2_reg &= SX127X_RF_LORA_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK;
    config2_reg |= (timeout >> 8) & ~SX127X_RF_LORA_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK;
    sx127x_reg_write(dev, SX127X_REG_LR_MODEMCONFIG2, config2_reg);
    sx127x_reg_write(dev, SX127X_REG_LR_SYMBTIMEOUTLSB,timeout & 0xFF);
}

void sx127x_set_iq_invert(sx127x_t *dev, bool iq_invert)
{
    DEBUG("[DEBUG] Set IQ invert: %d\n", iq_invert);

    dev->settings.lora.iq_inverted = iq_invert;

    sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ,
                     (sx127x_reg_read(dev, SX127X_REG_LR_INVERTIQ) &
                      SX127X_RF_LORA_INVERTIQ_RX_MASK &
                      SX127X_RF_LORA_INVERTIQ_TX_MASK) |
                      SX127X_RF_LORA_INVERTIQ_RX_OFF |
                     (iq_invert ? SX127X_RF_LORA_INVERTIQ_TX_ON : SX127X_RF_LORA_INVERTIQ_TX_OFF));
}

void sx127x_set_freq_hop(sx127x_t *dev, bool freq_hop_on)
{
    DEBUG("[DEBUG] Set freq hop: %d\n", freq_hop_on);

    dev->settings.lora.freq_hop_on = freq_hop_on;
}
