/*
 * Copyright (C) 2016 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx127x
 * @{
 * @file
 * @brief       Netdev adaptation for the sx127x driver
 *
 * @author      Eugene P. <ep@unwds.com>
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @}
 */

#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "net/netopt.h"
#include "net/netdev.h"
#include "sx127x_registers.h"
#include "sx127x_internal.h"
#include "sx127x_netdev.h"
#include "sx127x.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Internal helper functions */
static uint8_t _get_tx_len(const struct iovec *vector, unsigned count);
static int _set_state(sx127x_t *dev, netopt_state_t state);
static int _get_state(sx127x_t *dev, void *val);

/* Netdev driver api functions */
static int _send(netdev_t *netdev, const struct iovec *vector, unsigned count);
static int _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static int _init(netdev_t *netdev);
static void _isr(netdev_t *netdev);
static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev_t *netdev, netopt_t opt, void *val, size_t len);

const netdev_driver_t sx127x_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static int _send(netdev_t *netdev, const struct iovec *vector, unsigned count)
{
    sx127x_t *dev = (sx127x_t*) netdev;
    uint8_t size;
    size = _get_tx_len(vector, count);
    switch (dev->settings.modem) {
        case SX127X_MODEM_FSK:
            sx127x_write_fifo(dev, &size, 1);
            for (size_t i = 0 ; i < count ; i++) {
                sx127x_write_fifo(dev, vector[i].iov_base, vector[i].iov_len);
            }
            break;

        case SX127X_MODEM_LORA:
            if (dev->settings.lora.iq_inverted) {
                sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ,
                                 ((sx127x_reg_read(dev, SX127X_REG_LR_INVERTIQ) &
                                   SX127X_RF_LORA_INVERTIQ_TX_MASK & SX127X_RF_LORA_INVERTIQ_RX_MASK) |
                                  SX127X_RF_LORA_INVERTIQ_RX_OFF | SX127X_RF_LORA_INVERTIQ_TX_ON));
                sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ2,
                                 SX127X_RF_LORA_INVERTIQ2_ON);
            }
            else {
                sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ,
                                 ((sx127x_reg_read(dev, SX127X_REG_LR_INVERTIQ) &
                                   SX127X_RF_LORA_INVERTIQ_TX_MASK & SX127X_RF_LORA_INVERTIQ_RX_MASK) |
                                  SX127X_RF_LORA_INVERTIQ_RX_OFF | SX127X_RF_LORA_INVERTIQ_TX_OFF));
                sx127x_reg_write(dev, SX127X_REG_LR_INVERTIQ2, SX127X_RF_LORA_INVERTIQ2_OFF);
            }

            /* Initializes the payload size */
            sx127x_reg_write(dev, SX127X_REG_LR_PAYLOADLENGTH, size);

            /* Full buffer used for Tx */
            sx127x_reg_write(dev, SX127X_REG_LR_FIFOTXBASEADDR, 0x00);
            sx127x_reg_write(dev, SX127X_REG_LR_FIFOADDRPTR, 0x00);

            /* FIFO operations can not take place in Sleep mode
             * So wake up the chip */
            if ((sx127x_reg_read(dev, SX127X_REG_OPMODE) &
                 ~SX127X_RF_OPMODE_MASK) == SX127X_RF_OPMODE_SLEEP) {
                sx127x_set_standby(dev);
                xtimer_usleep(SX127X_RADIO_WAKEUP_TIME); /* wait for chip wake up */
            }

            /* Write payload buffer */
            for (size_t i = 0;i < count ; i++) {
                sx127x_write_fifo(dev, vector[i].iov_base, vector[i].iov_len);
            }
            break;
        default:
            puts("sx127x_netdev, Unsupported modem");
            break;
    }

    /* Enable TXDONE interrupt */
    sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGSMASK,
                     SX127X_RF_LORA_IRQFLAGS_RXTIMEOUT |
                     SX127X_RF_LORA_IRQFLAGS_RXDONE |
                     SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                     SX127X_RF_LORA_IRQFLAGS_VALIDHEADER |
                     /* SX127X_RF_LORA_IRQFLAGS_TXDONE | */
                     SX127X_RF_LORA_IRQFLAGS_CADDONE |
                     SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL |
                     SX127X_RF_LORA_IRQFLAGS_CADDETECTED);

    /* Set TXDONE interrupt to the DIO0 line */
    sx127x_reg_write(dev, SX127X_REG_DIOMAPPING1,
                     (sx127x_reg_read(dev, SX127X_REG_DIOMAPPING1) &
                      SX127X_RF_LORA_DIOMAPPING1_DIO0_MASK) |
                     SX127X_RF_LORA_DIOMAPPING1_DIO0_01);

    /* Start TX timeout timer */
    xtimer_set(&dev->_internal.tx_timeout_timer, dev->settings.lora.tx_timeout);

    /* Put chip into transfer mode */
    sx127x_set_state(dev, SX127X_RF_TX_RUNNING);
    sx127x_set_op_mode(dev, SX127X_RF_OPMODE_TRANSMITTER);

    return 0;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    /* Clear IRQ */
    volatile uint8_t irq_flags = 0;

    sx127x_t *dev = (sx127x_t*) netdev;
    sx127x_reg_write(dev,  SX127X_REG_LR_IRQFLAGS, SX127X_RF_LORA_IRQFLAGS_RXDONE);

    irq_flags = sx127x_reg_read(dev,  SX127X_REG_LR_IRQFLAGS);
    if ( (irq_flags & SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR_MASK) ==
         SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR) {
        /* Clear IRQ */
        sx127x_reg_write(dev,  SX127X_REG_LR_IRQFLAGS,
                         SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR);

        if (!dev->settings.lora.rx_continuous) {
            sx127x_set_state(dev,  SX127X_RF_IDLE);
        }

        xtimer_remove(&dev->_internal.rx_timeout_timer);
        netdev->event_callback(netdev, NETDEV_EVENT_CRC_ERROR);
        return -EBADMSG;
    }

    netdev_sx127x_rx_info_t *rx_info = info;
    if(info) {
        /* there is no LQI for LoRa */
        rx_info->lqi = 0;
        uint8_t snr_value = sx127x_reg_read(dev,  SX127X_REG_LR_PKTSNRVALUE);
        if (snr_value & 0x80) { /* The SNR is negative */
            /* Invert and divide by 4 */
            rx_info->snr = -1 * ((~snr_value + 1) & 0xFF) >> 2;
        }
        else {
            /* Divide by 4 */
            rx_info->snr = (snr_value & 0xFF) >> 2;
        }

        int16_t rssi = sx127x_reg_read(dev, SX127X_REG_LR_PKTRSSIVALUE);

        if (rx_info->snr < 0) {
#if defined(MODULE_SX1272)
            rx_info->rssi = SX127X_RSSI_OFFSET + rssi + (rssi >> 4) + rx_info->snr;
#else /* MODULE_SX1276 */
            if (dev->settings.channel > SX127X_RF_MID_BAND_THRESH) {
                rx_info->rssi = SX127X_RSSI_OFFSET_HF + rssi + (rssi >> 4) + rx_info->snr;
            }
            else {
                rx_info->rssi = SX127X_RSSI_OFFSET_LF + rssi + (rssi >> 4) + rx_info->snr;
            }
#endif
        }
        else {
#if defined(MODULE_SX1272)
            rx_info->rssi = SX127X_RSSI_OFFSET + rssi + (rssi >> 4);
#else /* MODULE_SX1276 */
            if (dev->settings.channel > SX127X_RF_MID_BAND_THRESH) {
                rx_info->rssi = SX127X_RSSI_OFFSET_HF + rssi + (rssi >> 4);
            }
            else {
                rx_info->rssi = SX127X_RSSI_OFFSET_LF + rssi + (rssi >> 4);
            }
#endif
        }
    }

    uint8_t size = sx127x_reg_read(dev, SX127X_REG_LR_RXNBBYTES);
    if (buf == NULL) {
        return size;
    }

    if (size > len) {
        return -ENOBUFS;
    }

    if (!dev->settings.lora.rx_continuous) {
        sx127x_set_state(dev,  SX127X_RF_IDLE);
    }

    xtimer_remove(&dev->_internal.rx_timeout_timer);

    /* Read the last packet from FIFO */
    uint8_t last_rx_addr = sx127x_reg_read(dev, SX127X_REG_LR_FIFORXCURRENTADDR);
    sx127x_reg_write(dev, SX127X_REG_LR_FIFOADDRPTR, last_rx_addr);
    sx127x_read_fifo(dev, (uint8_t *) buf, size);

    return size;
}

static int _init(netdev_t *netdev)
{
    sx127x_t *sx127x = (sx127x_t*) netdev;

    sx127x->irq = 0;
    sx127x_radio_settings_t settings;
    settings.channel = SX127X_CHANNEL_DEFAULT;
    settings.modem = SX127X_MODEM_DEFAULT;
    settings.state = SX127X_RF_IDLE;

    sx127x->settings = settings;

    /* Launch initialization of driver and device */
    DEBUG("init_radio: initializing driver...\n");
    sx127x_init(sx127x);

    sx127x_init_radio_settings(sx127x);
    /* Put chip into sleep */
    sx127x_set_sleep(sx127x);

    DEBUG("init_radio: sx127x initialization done\n");

    return 0;
}

static void _isr(netdev_t *netdev)
{
    sx127x_t *dev = (sx127x_t *) netdev;

    uint8_t irq = dev->irq;
    dev->irq = 0;

    if (irq & SX127X_IRQ_DIO0) {
        sx127x_on_dio0(dev);
    }

    if (irq & SX127X_IRQ_DIO1) {
        sx127x_on_dio1(dev);
    }

    if (irq & SX127X_IRQ_DIO2) {
        sx127x_on_dio2(dev);
    }

    if (irq & SX127X_IRQ_DIO3) {
        sx127x_on_dio3(dev);
    }

    if (irq & SX127X_IRQ_DIO4) {
        sx127x_on_dio4(dev);
    }

    if (irq & SX127X_IRQ_DIO5) {
        sx127x_on_dio5(dev);
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    sx127x_t *dev = (sx127x_t*) netdev;
    switch(opt) {
        case NETOPT_STATE:
            return _get_state((sx127x_t*) netdev, val);

        case NETOPT_CHANNEL:
            *((uint32_t*) val) = sx127x_get_channel((sx127x_t*) netdev);
            return sizeof(uint32_t);

        case NETOPT_LORA_BANDWIDTH:
            *((uint8_t*) val) = sx127x_get_bandwidth((sx127x_t*) netdev);
            return sizeof(uint8_t);

        case NETOPT_LORA_SPREADING_FACTOR:
            *((uint8_t*) val) = sx127x_get_spreading_factor((sx127x_t*) netdev);
            return sizeof(uint8_t);

        case NETOPT_LORA_CODING_RATE:
            *((uint8_t*) val) = sx127x_get_coding_rate((sx127x_t*) netdev);
            return sizeof(uint8_t);

        case NETOPT_LORA_SINGLE_RECEIVE:
            *((uint8_t*) val) = sx127x_get_rx_single((sx127x_t*) netdev);
            return sizeof(uint8_t);

        case NETOPT_LORA_SYNCWORD:
            *((uint8_t*) val) = sx127x_get_syncword((sx127x_t*) netdev);
            return sizeof(uint8_t);

        case NETOPT_CRC:
            *((netopt_enable_t*) val) = sx127x_get_crc((sx127x_t*) netdev) ? NETOPT_ENABLE : NETOPT_DISABLE;
            break;

        case NETOPT_LORA_HOP:
            *((netopt_enable_t*) val) = dev->settings.lora.freq_hop_on ? NETOPT_ENABLE : NETOPT_DISABLE;
            break;

        case NETOPT_LORA_HOP_PERIOD:
            *((uint8_t*) val) = sx127x_get_hop_period(dev);
            break;

        case NETOPT_LORA_TIME_ON_AIR:
            *((uint32_t*) val) = sx127x_get_time_on_air(dev);
            break;

        case NETOPT_LORA_RANDOM:
            *((uint32_t*) val) = sx127x_random(dev);
            break;

        default:
            break;
    }
    return 0;
}

static int _set(netdev_t *netdev, netopt_t opt, void *val, size_t len)
{
    sx127x_t *dev = (sx127x_t*) netdev;
    switch(opt)
    {
        case NETOPT_STATE:
            return _set_state((sx127x_t*) netdev, *((netopt_state_t*) val));

        case NETOPT_CHANNEL:
            sx127x_set_channel((sx127x_t*) netdev, *((uint32_t*) val));
            return sizeof(uint32_t);

        case NETOPT_LORA_BANDWIDTH:
            sx127x_set_bandwidth((sx127x_t*) netdev, *((uint8_t*) val));
            return sizeof(uint8_t);

        case NETOPT_LORA_SPREADING_FACTOR:
            sx127x_set_spreading_factor((sx127x_t*) netdev, *((uint8_t*) val));
            return sizeof(uint8_t);

        case NETOPT_LORA_CODING_RATE:
            sx127x_set_coding_rate((sx127x_t*) netdev, *((uint8_t*) val));
            return sizeof(uint8_t);

        case NETOPT_LORA_SINGLE_RECEIVE:
            sx127x_set_rx_single((sx127x_t*) netdev, *((uint8_t*) val));
            return sizeof(uint8_t);

        case NETOPT_LORA_SYNCWORD:
            sx127x_set_syncword((sx127x_t*) netdev, *((uint8_t*) val));
            return sizeof(uint8_t);

        case NETOPT_CRC:
            sx127x_set_crc((sx127x_t*) netdev, *((netopt_enable_t*) val) ? true : false);
            return sizeof(netopt_enable_t);

        case NETOPT_LORA_HOP:
            sx127x_set_freq_hop(dev, *((netopt_enable_t*) val) ? true : false);
           return sizeof(netopt_enable_t);

        case NETOPT_LORA_HOP_PERIOD:
            sx127x_set_hop_period(dev, *((uint8_t*) val));
            return sizeof(uint8_t);

        case NETOPT_LORA_FIXED_HEADER:
            sx127x_set_fixed_header_len_mode(dev, *((netopt_enable_t*) val) ? true : false);
            return sizeof(netopt_enable_t);

        case NETOPT_LORA_PAYLOAD_LENGTH:
            sx127x_set_payload_length(dev, *((uint8_t*) val));
            return sizeof(uint8_t);

        case NETOPT_TX_POWER:
            sx127x_set_tx_power(dev, *((uint8_t*) val));
            return sizeof(uint16_t);

        case NETOPT_LORA_PREAMBLE_LENGTH:
            sx127x_set_preamble_length(dev, *((uint16_t*) val));
            return sizeof(uint16_t);

        case NETOPT_LORA_SYMBOL_TIMEOUT:
            sx127x_set_symbol_timeout(dev, *((uint16_t*) val));
            return sizeof(uint16_t);

        case NETOPT_LORA_IQ_INVERT:
            sx127x_set_iq_invert(dev, *((bool*) val));
            return sizeof(bool);

        case NETOPT_LORA_TX_TIMEOUT:
            sx127x_set_tx_timeout(dev, *((uint32_t*) val));
            return sizeof(uint32_t);

        case NETOPT_LORA_RX_TIMEOUT:
            sx127x_set_rx_timeout(dev, *((uint32_t*) val));
            return sizeof(uint32_t);

        case NETOPT_LORA_MODE:
            sx127x_set_modem(dev, *((netopt_enable_t*) val) ? SX127X_MODEM_LORA : SX127X_MODEM_FSK);
            return sizeof(netopt_enable_t);

        case NETOPT_LORA_MAX_PAYLOAD:
            sx127x_set_max_payload_len(dev, *((uint8_t*) val));
            return sizeof(uint8_t);

        case NETOPT_LORA_TIME_ON_AIR:
            dev->settings.time_on_air_pkt_len = *((uint8_t*) val);
            break;

        default:
            break;
    }
    return 0;
}

static uint8_t _get_tx_len(const struct iovec *vector, unsigned count)
{
    uint8_t len = 0;

    for (int i=0 ; i < count ; i++) {
        len += vector[i].iov_len;
    }

    return len;
}

static int _set_state(sx127x_t *dev, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_SLEEP:
            sx127x_set_sleep(dev);
            break;

        case NETOPT_STATE_STANDBY:
            sx127x_set_standby(dev);
            break;

        case NETOPT_STATE_IDLE:
            dev->settings.window_timeout = 0;
            /* set permanent listening */
            sx127x_set_rx(dev);
            break;

        case NETOPT_STATE_RX:
            sx127x_set_rx(dev);
            break;

        case NETOPT_STATE_TX:
            /* TODO: Implement preloading */
            break;

        case NETOPT_STATE_RESET:
            sx127x_reset(dev);
            break;

        default:
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

static int _get_state(sx127x_t *dev, void *val)
{
    uint8_t op_mode;
    op_mode = sx127x_get_op_mode(dev);
    netopt_state_t state;
    switch(op_mode) {
        case SX127X_RF_OPMODE_SLEEP:
            state = NETOPT_STATE_SLEEP;
            break;

        case SX127X_RF_OPMODE_STANDBY:
            state = NETOPT_STATE_STANDBY;
            break;

        case SX127X_RF_OPMODE_TRANSMITTER:
            state = NETOPT_STATE_TX;
            break;

        case SX127X_RF_OPMODE_RECEIVER:
        case SX127X_RF_LORA_OPMODE_RECEIVER_SINGLE:
            state = NETOPT_STATE_IDLE;
            break;

        default:
            break;
    }
    memcpy(val, &state, sizeof(netopt_state_t));
    return sizeof(netopt_state_t);
}
