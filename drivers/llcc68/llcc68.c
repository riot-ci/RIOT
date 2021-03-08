/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_llcc68
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LLCC68 LoRa radio driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <errno.h>

#include "llcc68_netdev.h"

#include "net/lora.h"
#include "periph/spi.h"

#include "llcc68_driver.h"
#include "llcc68.h"
#include "llcc68_params.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CONFIG_LLCC68_PKT_TYPE_DEFAULT
#define CONFIG_LLCC68_PKT_TYPE_DEFAULT          (LLCC68_PKT_TYPE_LORA)
#endif

#ifndef CONFIG_LLCC68_CHANNEL_DEFAULT
#define CONFIG_LLCC68_CHANNEL_DEFAULT           (868300000UL)   /* in Hz */
#endif

#ifndef CONFIG_LLCC68_TX_POWER_DEFAULT
#define CONFIG_LLCC68_TX_POWER_DEFAULT          (14U)           /* in dBm */
#endif

#ifndef CONFIG_LLCC68_RAMP_TIME_DEFAULT
#define CONFIG_LLCC68_RAMP_TIME_DEFAULT         (LLCC68_RAMP_10_US)
#endif

void llcc68_setup(llcc68_t *dev, const llcc68_params_t *params, uint8_t index)
{
    netdev_t *netdev = (netdev_t *)dev;
    netdev->driver = &llcc68_driver;
    dev->params = (llcc68_params_t *)params;
    netdev_register(&dev->netdev, NETDEV_LLCC68, index);
}

static void llcc68_init_default_config(llcc68_t *dev)
{
    /* call order is important */
    llcc68_set_pkt_type(dev, LLCC68_PKT_TYPE_LORA);
    llcc68_set_channel(dev, CONFIG_LLCC68_CHANNEL_DEFAULT);
    llcc68_pa_cfg_params_t pa_cfg = {
        .pa_duty_cycle = 0x02,
        .hp_max = 0x02,
        .device_sel = 0x00,
        .pa_lut = 0x01
    };
    llcc68_set_pa_cfg(dev, &pa_cfg);
    llcc68_set_tx_params(dev, CONFIG_LLCC68_TX_POWER_DEFAULT, CONFIG_LLCC68_RAMP_TIME_DEFAULT);

    dev->mod_params.bw = CONFIG_LORA_BW_DEFAULT + LLCC68_LORA_BW_125;
    dev->mod_params.sf = CONFIG_LORA_SF_DEFAULT;
    dev->mod_params.cr = CONFIG_LORA_CR_DEFAULT;
    dev->mod_params.ldro = 1;
    llcc68_set_lora_mod_params(dev, &dev->mod_params);

    dev->pkt_params.pld_len_in_bytes = 0;
    dev->pkt_params.crc_is_on = LORA_PAYLOAD_CRC_ON_DEFAULT;
    dev->pkt_params.header_type = (
        IS_ACTIVE(CONFIG_LORA_FIXED_HEADER_LEN_MODE_DEFAULT) ? true : false
    );
    dev->pkt_params.preamble_len_in_symb = CONFIG_LORA_PREAMBLE_LENGTH_DEFAULT;
    dev->pkt_params.invert_iq_is_on = (
        IS_ACTIVE(CONFIG_LORA_IQ_INVERTED_DEFAULT) ? true : false
    );
    llcc68_set_lora_pkt_params(dev, &dev->pkt_params);
}

static void _dio1_isr(void *arg)
{
    netdev_trigger_event_isr((netdev_t *)arg);
}

int llcc68_init(llcc68_t *dev)
{
    /* Setup SPI for LLCC68 */
    int res = spi_init_cs(dev->params->spi, dev->params->nss_pin);
    if (res != SPI_OK) {
        DEBUG("[llcc68] error: failed to initialize SPI_%i device (code %i)\n",
              dev->params->spi, res);
        return -1;
    }

    DEBUG("[llcc68] init: SPI_%i initialized with success\n", dev->params->spi);

    gpio_init(dev->params->reset_pin, GPIO_OUT);
    gpio_init(dev->params->busy_pin, GPIO_IN_PD);

    /* Initialize DIOs */
    if (gpio_is_valid(dev->params->dio1_pin)) {
        res = gpio_init_int(dev->params->dio1_pin, GPIO_IN, GPIO_RISING, _dio1_isr, dev);
        if (res < 0) {
            DEBUG("[llcc68] error: failed to initialize DIO1 pin\n");
            return res;
        }
    }
    else {
        DEBUG("[llcc68] error: no DIO1 pin defined\n");
        return -EIO;
    }

    /* Reset the device */
    llcc68_reset(dev);

    /* Configure the power regulator mode */
    llcc68_set_reg_mode(dev, LLCC68_REG_MODE_DCDC);

    /* Initialize radio with the default parameters */
    llcc68_init_default_config(dev);

    /* Configure available IRQs */
    const uint16_t irq_mask = (
        LLCC68_IRQ_TX_DONE |
        LLCC68_IRQ_RX_DONE |
        LLCC68_IRQ_PREAMBLE_DETECTED |
        LLCC68_IRQ_HEADER_VALID |
        LLCC68_IRQ_HEADER_ERROR |
        LLCC68_IRQ_CRC_ERROR |
        LLCC68_IRQ_CAD_DONE |
        LLCC68_IRQ_CAD_DETECTED |
        LLCC68_IRQ_TIMEOUT
    );
    llcc68_set_dio_irq_params(dev, irq_mask, irq_mask, 0, 0);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        llcc68_pkt_type_t pkt_type;
        llcc68_get_pkt_type(dev, &pkt_type);
        DEBUG("[llcc68] init radio: pkt type: %d\n", pkt_type);

        llcc68_chip_status_t radio_status;
        llcc68_get_status(dev, &radio_status);
        DEBUG("[llcc68] init: chip mode %d\n", radio_status.chip_mode);
        DEBUG("[llcc68] init: cmd status %d\n", radio_status.cmd_status);
    }

    return res;
}

uint32_t llcc68_get_channel(const llcc68_t *dev)
{
    return dev->channel;
}

void llcc68_set_channel(llcc68_t *dev, uint32_t freq)
{
    dev->channel = freq;
    llcc68_set_rf_freq(dev, dev->channel);
}

uint8_t llcc68_get_bandwidth(const llcc68_t *dev)
{
    return dev->mod_params.bw - LLCC68_LORA_BW_125;
}

void llcc68_set_bandwidth(llcc68_t *dev, uint8_t bandwidth)
{
    dev->mod_params.bw = bandwidth + LLCC68_LORA_BW_125;
    llcc68_set_lora_mod_params(dev, &dev->mod_params);
}

uint8_t llcc68_get_spreading_factor(const llcc68_t *dev)
{
    return dev->mod_params.sf;
}

void llcc68_set_spreading_factor(llcc68_t *dev, uint8_t sf)
{
    dev->mod_params.sf = sf;
    llcc68_set_lora_mod_params(dev, &dev->mod_params);
}

uint8_t llcc68_get_coding_rate(const llcc68_t *dev)
{
    return dev->mod_params.cr;
}

void llcc68_set_coding_rate(llcc68_t *dev, uint8_t cr)
{
    dev->mod_params.cr = cr;
    llcc68_set_lora_mod_params(dev, &dev->mod_params);
}

uint8_t llcc68_get_lora_payload_length(const llcc68_t *dev)
{
    return dev->pkt_params.pld_len_in_bytes;
}

void llcc68_set_lora_payload_length(llcc68_t *dev, uint8_t len)
{
    dev->pkt_params.pld_len_in_bytes = len;
    llcc68_set_lora_pkt_params(dev, &dev->pkt_params);
}

bool llcc68_get_lora_crc(const llcc68_t *dev)
{
    return dev->pkt_params.crc_is_on;
}

void llcc68_set_lora_crc(llcc68_t *dev, bool crc)
{
    dev->pkt_params.crc_is_on = crc;
    llcc68_set_lora_pkt_params(dev, &dev->pkt_params);
}

bool llcc68_get_lora_implicit_header(const llcc68_t *dev)
{
    return dev->pkt_params.header_type == LLCC68_LORA_PKT_IMPLICIT;
}

void llcc68_set_lora_implicit_header(llcc68_t *dev, bool mode)
{
    dev->pkt_params.header_type = (mode ? LLCC68_LORA_PKT_IMPLICIT : LLCC68_LORA_PKT_EXPLICIT);
    llcc68_set_lora_pkt_params(dev, &dev->pkt_params);
}

uint16_t llcc68_get_lora_preamble_length(const llcc68_t *dev)
{
    return dev->pkt_params.preamble_len_in_symb;
}

void llcc68_set_lora_preamble_length(llcc68_t *dev, uint16_t preamble)
{
    dev->pkt_params.preamble_len_in_symb = preamble;
    llcc68_set_lora_pkt_params(dev, &dev->pkt_params);
}

bool llcc68_get_lora_iq_invert(const llcc68_t *dev)
{
    return dev->pkt_params.invert_iq_is_on;
}

void llcc68_set_lora_iq_invert(llcc68_t *dev, bool iq_invert)
{
    dev->pkt_params.invert_iq_is_on = iq_invert;
    llcc68_set_lora_pkt_params(dev, &dev->pkt_params);
}
