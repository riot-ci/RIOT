/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52_common
 * @{
 *
 * @file
 * @brief       Shared IRQ handling between SPI and TWI peripherals on the nRF52
 *              devices
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"

#ifdef CPU_MODEL_NRF52840XXAA
#define SPI_TWI_IRQ_NUMOF 4
#else
#define SPI_TWI_IRQ_NUMOF 3
#endif

static spi_twi_irq_cb_t _irq[SPI_TWI_IRQ_NUMOF];
static void *_irq_arg[SPI_TWI_IRQ_NUMOF];

/* I2C and SPI share peripheral addresses */
static size_t _spi_dev2num(void *dev)
{
    if (dev == NRF_SPIM0) {
        return 0;
    }
    else if (dev == NRF_SPIM1) {
        return 1;
    }
    else if (dev == NRF_SPIM2) {
        return 2;
    }
#ifdef CPU_MODEL_NRF52840XXAA
    else if (dev == NRF_SPIM3) {
        return 3;
    }
#endif
    else {
        assert(false);
        return 0;
    }
}

static const IRQn_Type _isr[] = {
    SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,
    SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn,
    SPIM2_SPIS2_SPI2_IRQn,
#ifdef CPU_MODEL_NRF52840XXAA
    SPIM3_IRQn,
#endif /* CPU_MODEL_NRF52840XXAA */
};

void spi_twi_irq_register_spi(NRF_SPIM_Type *bus, spi_twi_irq_cb_t cb, void *arg)
{
    size_t num = _spi_dev2num(bus);
    _irq[num] = cb;
    _irq_arg[num] = arg;
    NVIC_EnableIRQ(_isr[num]);
}

void isr_spi0_twi0(void)
{
    _irq[0](_irq_arg[0]);
    cortexm_isr_end();
}

void isr_spi1_twi1(void)
{
    _irq[1](_irq_arg[1]);
    cortexm_isr_end();
}

void isr_spi2(void)
{
    _irq[2](_irq_arg[2]);
    cortexm_isr_end();
}

#ifdef CPU_MODEL_NRF52840XXAA
void isr_spi3(void)
{
    _irq[3](_irq_arg[3]);
    cortexm_isr_end();
}
#endif /* CPU_MODEL_NRF52840XXAA */
