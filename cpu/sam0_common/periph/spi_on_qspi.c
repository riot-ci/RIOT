#include "cpu.h"
#include "assert.h"
#include "periph/spi.h"
#include "spi_internal.h"

void _init_qspi(spi_t bus)
{
    /* reset the peripheral */
    QSPI->CTRLA.bit.SWRST = 1;

    QSPI->CTRLB.reg = QSPI_CTRLB_MODE_SPI
                    | QSPI_CTRLB_CSMODE_LASTXFER
                    | QSPI_CTRLB_DATALEN_8BITS;

    /* set up DMA channels */
    _init_dma(bus, &QSPI->RXDATA.reg, &QSPI->TXDATA.reg);
}

void _qspi_acquire(spi_mode_t mode, spi_clk_t clk)
{
    /* datasheet says SCK = MCK / (BAUD + 1) */
    /* but BAUD = 0 does not work, assume SCK = MCK / BAUD */
    uint32_t baud = CLOCK_CORECLOCK > (2 * clk)
                  ? (CLOCK_CORECLOCK + clk - 1) / clk
                  : 1;

    /* bit order is reversed from SERCOM SPI */
    uint32_t _mode = (mode >> 1)
                   | (mode << 1);
    _mode &= 0x3;

    QSPI->CTRLA.bit.ENABLE = 1;
    QSPI->BAUD.reg = QSPI_BAUD_BAUD(baud) | _mode;
}

void _qspi_release(void)
{
    QSPI->CTRLA.bit.ENABLE = 0;
}

void _qspi_blocking_transfer(const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = out_buf ? out_buf[i] : 0;

        /* transmit byte on MOSI */
        QSPI->TXDATA.reg = tmp;

        /* wait until byte has been sampled on MISO */
        while (QSPI->INTFLAG.bit.RXC == 0) {}

        /* consume the byte */
        tmp = QSPI->RXDATA.reg;

        if (in_buf) {
            in_buf[i] = tmp;
        }
    }
}
