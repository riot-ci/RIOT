
void _init_dma(spi_t bus, const volatile void *reg_rx, volatile void *reg_tx);

void _init_qspi(spi_t bus);
void _qspi_acquire(spi_mode_t mode, spi_clk_t clk);
void _qspi_release(void);
void _qspi_blocking_transfer(const void *out, void *in, size_t len);

void _init_spi(spi_t bus, SercomSpi *dev);
void _spi_acquire(spi_t bus, spi_mode_t mode, spi_clk_t clk);
void _spi_release(spi_t bus);
void _spi_blocking_transfer(spi_t bus, const void *out, void *in, size_t len);
