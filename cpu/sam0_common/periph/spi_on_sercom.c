#include "cpu.h"
#include "assert.h"
#include "periph/spi.h"

/**
 * @brief   Shortcut for accessing the used SPI SERCOM device
 */
static inline SercomSpi *dev(spi_t bus)
{
    return (SercomSpi *)spi_config[bus].dev;
}

static void _reset(SercomSpi *dev)
{
    dev->CTRLA.reg |= SERCOM_SPI_CTRLA_SWRST;
    while (dev->CTRLA.reg & SERCOM_SPI_CTRLA_SWRST) {}

#ifdef SERCOM_SPI_STATUS_SYNCBUSY
    while (dev->STATUS.bit.SYNCBUSY) {}
#else
    while (dev->SYNCBUSY.bit.SWRST) {}
#endif
}

static inline void _disable(SercomSpi *dev)
{
    dev->CTRLA.reg = 0;

#ifdef SERCOM_SPI_STATUS_SYNCBUSY
    while (dev->STATUS.bit.SYNCBUSY) {}
#else
    while (dev->SYNCBUSY.reg) {}
#endif
}

static inline void _enable(SercomSpi *dev)
{
    dev->CTRLA.bit.ENABLE = 1;

#ifdef SERCOM_SPI_STATUS_SYNCBUSY
    while (dev->STATUS.bit.SYNCBUSY) {}
#else
    while (dev->SYNCBUSY.reg) {}
#endif
}

void _init_dma(spi_t bus, const volatile void *reg_rx, volatile void *reg_tx);

void _init_spi(spi_t bus, SercomSpi *dev)
{
    /* reset all device configuration */
    _reset(dev);

    /* configure base clock */
    sercom_set_gen(dev, spi_config[bus].gclk_src);

    /* enable receiver and configure character size to 8-bit
     * no synchronization needed, as SERCOM device is not enabled */
    dev->CTRLB.reg = SERCOM_SPI_CTRLB_CHSIZE(0) | SERCOM_SPI_CTRLB_RXEN;

    /* set up DMA channels */
    _init_dma(bus, &dev->DATA.reg, &dev->DATA.reg);
}

void _spi_acquire(spi_t bus, spi_mode_t mode, spi_clk_t clk)
{
    /* configure bus clock, in synchronous mode its calculated from
     * BAUD.reg = (f_ref / (2 * f_bus) - 1)
     * with f_ref := CLOCK_CORECLOCK as defined by the board
     * to mitigate the rounding error due to integer arithmetic, the
     * equation is modified to
     * BAUD.reg = ((f_ref + f_bus) / (2 * f_bus) - 1) */
    const uint8_t baud = ((sam0_gclk_freq(spi_config[bus].gclk_src) + clk) / (2 * clk) - 1);

    /* configure device to be master and set mode and pads,
     *
     * NOTE: we could configure the pads already during spi_init, but for
     * efficiency reason we do that here, so we can do all in one single write
     * to the CTRLA register */
    const uint32_t ctrla = SERCOM_SPI_CTRLA_MODE(0x3)       /* 0x3 -> master */
                         | SERCOM_SPI_CTRLA_DOPO(spi_config[bus].mosi_pad)
                         | SERCOM_SPI_CTRLA_DIPO(spi_config[bus].miso_pad)
                         | (mode << SERCOM_SPI_CTRLA_CPHA_Pos);

    /* first configuration or reconfiguration after altered device usage */
    if (dev(bus)->BAUD.reg != baud || dev(bus)->CTRLA.reg != ctrla) {
        /* disable the device */
        _disable(dev(bus));

        dev(bus)->BAUD.reg = baud;
        dev(bus)->CTRLA.reg = ctrla;
        /* no synchronization needed here, the enable synchronization below
         * acts as a write-synchronization for both registers */
    }

    /* finally enable the device */
    _enable(dev(bus));
}

void _spi_release(spi_t bus)
{
    /* disable the device */
    _disable(dev(bus));
}

void _spi_blocking_transfer(spi_t bus, const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;

        /* transmit byte on MOSI */
        dev(bus)->DATA.reg = tmp;

        /* wait until byte has been sampled on MISO */
        while (dev(bus)->INTFLAG.bit.RXC == 0) {}

        /* consume the byte */
        tmp = dev(bus)->DATA.reg;

        if (in_buf) {
            in_buf[i] = tmp;
        }
    }
}
