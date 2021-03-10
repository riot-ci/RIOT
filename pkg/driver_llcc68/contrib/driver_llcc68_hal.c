/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_llcc68
 * @{
 *
 * @file
 * @brief       HAL implementation for the LLCC68 LoRa radio driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "fmt.h"
#include "ztimer.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "llcc68.h"
#include "llcc68_hal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define LLCC68_SPI_SPEED    (SPI_CLK_1MHZ)
#define LLCC68_SPI_MODE     (SPI_MODE_0)

llcc68_hal_status_t llcc68_hal_write(const void *context,
                                     const uint8_t *command, const uint16_t command_length,
                                     const uint8_t *data, const uint16_t data_length)
{
    (void)data;
    (void)data_length;
    llcc68_t *dev = (llcc68_t *)context;

    /* wait for the device to not be busy anymore */
    while (gpio_read(dev->params->busy_pin)) {}

    spi_acquire(dev->params->spi, SPI_CS_UNDEF, LLCC68_SPI_MODE, LLCC68_SPI_SPEED);
    spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, data_length != 0, command, NULL, command_length);
    if (data_length) {
        spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, false, data, NULL, data_length);
    }
    spi_release(dev->params->spi);
    return 0;
}

llcc68_hal_status_t llcc68_hal_read(const void* context,
                                    const uint8_t *command, const uint16_t command_length,
                                    uint8_t *data, const uint16_t data_length)
{
    llcc68_t *dev = (llcc68_t *)context;

    /* wait for the device to not be busy anymore */
    while (gpio_read(dev->params->busy_pin)) {}

    spi_acquire(dev->params->spi, SPI_CS_UNDEF, LLCC68_SPI_MODE, LLCC68_SPI_SPEED);
    spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, true, command, NULL, command_length);
    spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, false, NULL, data, data_length);
    spi_release(dev->params->spi);
    return 0;
}

llcc68_hal_status_t llcc68_hal_reset(const void *context)
{
    DEBUG("[llcc68_hal] reset\n");
    llcc68_t *dev = (llcc68_t *)context;
    gpio_set(dev->params->reset_pin);
    gpio_clear(dev->params->reset_pin);
    ztimer_sleep(ZTIMER_USEC, 100);
    gpio_set(dev->params->reset_pin);
    return 0;
}

llcc68_hal_status_t llcc68_hal_wakeup(const void *context)
{
    DEBUG("[llcc68_hal] wakeup\n");
    llcc68_t *dev = (llcc68_t *)context;
    spi_acquire(dev->params->spi, SPI_CS_UNDEF, LLCC68_SPI_MODE, LLCC68_SPI_SPEED);
    gpio_clear(dev->params->nss_pin);
    gpio_set(dev->params->nss_pin);
    spi_release(dev->params->spi);

    /* it takes 500us for the radio device to be ready after waking up */
    ztimer_sleep(ZTIMER_USEC, 500);
    return 0;
}
