/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation based on the SPIM peripheral
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include <string.h>

#define SPI_CPU_FLASH_END    0x20000000

/**
 * @brief   array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

/**
 * @brief   array with a busy mutex for each SPI device, used to block the
 *          thread until the transfer is done
 */
static mutex_t busy[SPI_NUMOF];

static uint8_t _mbuf[SPI_NUMOF][SPI_MBUF_SIZE];


static void spi_isr_handler(void *arg);
#ifdef CPU_MODEL_NRF52832XXAA
void spi_gpio_handler(void *arg);
#endif

static inline NRF_SPIM_Type *dev(spi_t bus)
{
    return (NRF_SPIM_Type *)spi_config[bus].dev;
}

#ifdef CPU_MODEL_NRF52832XXAA
/**
 * @brief Work-around for transmitting 1 byte with SPIM on the nrf52832.
 * @warning Must not be used when transmitting multiple bytes.
 * @warning After this workaround is used, the user must reset the PPI channel and the GPIOTE channel before attempting to transmit multiple bytes.
 *
 * @param bus   The SPI instance that is in use.
 */
static void _setup_workaround_for_ftpan_58(spi_t bus)
{
    gpio_init_int(spi_config[bus].sclk, GPIO_OUT, GPIO_BOTH,
                  spi_gpio_handler, (void*)bus);
    gpio_irq_disable(spi_config[bus].sclk);
    uint8_t channel = gpio_int_get_exti(spi_config[bus].sclk);
    assert(channel != 0xff);

    // Stop the spim instance when SCK toggles.
    NRF_PPI->CH[spi_config[bus].ppi].EEP = (uint32_t)&NRF_GPIOTE->EVENTS_IN[channel];
    NRF_PPI->CH[spi_config[bus].ppi].TEP = (uint32_t)&dev(bus)->TASKS_STOP;
}

static void _enable_workaround(spi_t bus)
{
    /**
     * The spim instance cannot be stopped mid-byte, so it will finish
     * transmitting the first byte and then stop. Effectively ensuring
     * that only 1 byte is transmitted.
     */
    NRF_PPI->CHENSET = 1U << spi_config[bus].ppi;
    gpio_irq_enable(spi_config[bus].sclk);
}

static void _clear_workaround(spi_t bus)
{
    NRF_PPI->CHENCLR = 1U << spi_config[bus].ppi;
}
#endif

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* initialize mutex */
    mutex_init(&locks[bus]);
    mutex_init(&busy[bus]);
    mutex_lock(&busy[bus]);
    /* initialize pins */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    gpio_init(spi_config[bus].sclk, GPIO_OUT);
    gpio_init(spi_config[bus].mosi, GPIO_OUT);
    gpio_init(spi_config[bus].miso, GPIO_IN);
    /* select pins for the SPI device */
    SPI_SCKSEL  = spi_config[bus].sclk;
    SPI_MOSISEL = spi_config[bus].mosi;
    SPI_MISOSEL = spi_config[bus].miso;
#ifdef CPU_MODEL_NRF52832XXAA
    _setup_workaround_for_ftpan_58(bus);
#endif
    spi_twi_irq_register_spi(dev(bus), spi_isr_handler, (void*)bus);
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void) cs;

    mutex_lock(&locks[bus]);
    /* configure bus */
    dev(bus)->CONFIG = mode;
    dev(bus)->FREQUENCY = clk;
    /* enable the bus */
    dev(bus)->ENABLE = SPIM_ENABLE_ENABLE_Enabled;

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    /* power off everything */
    dev(bus)->ENABLE = 0;
    mutex_unlock(&locks[bus]);
}

static void _transfer(spi_t bus, const uint8_t *out_buf, uint8_t *in_buf, uint8_t transfer_len)
{
    uint8_t out_len = (out_buf) ? transfer_len : 0;
    uint8_t in_len = (in_buf) ? transfer_len : 0;
    const uint8_t *out_mbuf = out_buf;
    /**
     * Copy the out buffer in case it resides in flash, EasyDMA only works from
     * RAM
     */
    if ((out_buf < (uint8_t*)SPI_CPU_FLASH_END) && (out_len)) {
        memcpy(_mbuf[bus], out_buf, out_len);
        out_mbuf = _mbuf[bus];
    }
    dev(bus)->TXD.PTR = (uint32_t)out_mbuf;
    dev(bus)->RXD.PTR = (uint32_t)in_buf;

    dev(bus)->TXD.MAXCNT = out_len;
    dev(bus)->RXD.MAXCNT = in_len;

    /* clear any spurious END events */
    dev(bus)->EVENTS_END = 0;
    dev(bus)->TASKS_START = 1;
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    assert(out_buf || in_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    /* Enable the workaround when the length is only 1 byte */
#ifdef CPU_MODEL_NRF52832XXAA
    size_t _len = len;
    if (_len == 1) {
        _enable_workaround(bus);
    }
#endif

    /* Enable IRQ */
    dev(bus)->INTENSET = SPIM_INTENSET_END_Msk;

    do {
        size_t transfer_len = len > UINT8_MAX ? UINT8_MAX : len;
        _transfer(bus, out_buf, in_buf, transfer_len);
        /* Block until the irq releases the mutex, then lock it again for the
         * next transfer */
        mutex_lock(&busy[bus]);
        out_buf += out_buf ? transfer_len : 0;
        in_buf += in_buf? transfer_len : 0;
        len -= transfer_len;
    } while (len);

    /* Disable IRQ */
    dev(bus)->INTENCLR = SPIM_INTENCLR_END_Msk;

    /**
     * While we could always disable the workaround, only doing this when
     * required spares us some cycles by not having to write to volatile
     * registers
     */
#ifdef CPU_MODEL_NRF52832XXAA
    if (_len == 1) {
        _clear_workaround(bus);
    }
#endif

    if ((cs != SPI_CS_UNDEF) && (!cont)) {
        gpio_set((gpio_t)cs);
    }
}

void spi_isr_handler(void *arg)
{
    spi_t bus = (spi_t)arg;
    mutex_unlock(&busy[bus]);
    dev(bus)->EVENTS_END = 0;
}

#ifdef CPU_MODEL_NRF52832XXAA
void spi_gpio_handler(void *arg)
{
    spi_t bus = (spi_t)arg;
    /**
     * Immediately disable the IRQ, we only care about one PPI event per
     * transfer
     */
    gpio_irq_disable(spi_config[bus].sclk);
}
#endif
