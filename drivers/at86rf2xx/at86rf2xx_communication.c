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
 * @brief       Implementation of SPI based and peripheral communication
 *              interface for AT86RF2xx drivers
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

#include "kernel_defines.h"
#if IS_USED(MODULE_AT86RF2XX_SPI)
#include "periph/spi.h"
#endif
#include "at86rf2xx.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_communication.h"

/* 8-bit  MCU integrated transceivers */
#if IS_USED(MODULE_AT86RF2XX_PERIPH)
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
#if IS_USED(MODULE_AT86RF2XX_SPI)
            spi_acquire(SPIDEV, CSPIN, SPI_MODE_0,
                        dev->params.spi_clk);
#else
            (void)dev;
#endif
            break;
#if IS_USED(MODULE_AT86RF2XX_PERIPH)
        case AT86RF2XX_DEV_TYPE_AT86RFA1:
        case AT86RF2XX_DEV_TYPE_AT86RFR2:
            break;
#endif
    }
}

uint8_t at86rf2xx_reg_read(const at86rf2xx_t *dev, uint8_t addr)
{
    uint8_t value = 0;
    switch (dev->base.dev_type) {
        default: {
#if IS_USED(MODULE_AT86RF2XX_SPI)
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
        }
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
        default: {
#if IS_USED(MODULE_AT86RF2XX_SPI)
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
        }
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
        default: {
#if IS_USED(MODULE_AT86RF2XX_SPI)
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
        }
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
        default: {
#if IS_USED(MODULE_AT86RF2XX_SPI)
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
        }
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
        default: {
#if IS_USED(MODULE_AT86RF2XX_SPI)
            uint8_t reg = AT86RF2XX_ACCESS_FB | AT86RF2XX_ACCESS_READ;
            getbus(dev);
            spi_transfer_byte(SPIDEV, CSPIN, true, reg);
#else
            (void)dev;
#endif
            break;
        }
#if IS_USED(MODULE_AT86RF2XX_PERIPH)
        case AT86RF2XX_DEV_TYPE_AT86RFA1:
        case AT86RF2XX_DEV_TYPE_AT86RFR2:
            break;
#endif
    }
}

void at86rf2xx_fb_read(const at86rf2xx_t *dev,
                       uint8_t *data, size_t len)
{
    switch (dev->base.dev_type) {
        default:
#if IS_USED(MODULE_AT86RF2XX_SPI)
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
#if IS_USED(MODULE_AT86RF2XX_SPI)
            /* transfer one byte (which we ignore) to release the chip select */
            spi_transfer_byte(SPIDEV, CSPIN, false, 1);
            spi_release(SPIDEV);
#else
            (void)dev;
#endif
            break;
#if IS_USED(MODULE_AT86RF2XX_PERIPH)
        case AT86RF2XX_DEV_TYPE_AT86RFA1:
        case AT86RF2XX_DEV_TYPE_AT86RFR2:
            break;
#endif
    }
}
