# SenseBox board
## On I2C port
I2C port is enabled by default. To disable it use ```I2C_DISABLE```. To re-enable use ```I2C_ENABLE```.

## On XBEE ports
XBEE1 and XBEE2 ports are enabled by default. To disable any of them use ```XBEEn_DISABLE```. To re-enable use ```XBEEn_ENABLE```.

On the board there is a reset circuit for this ports that delays the reset signal from the enable signal. Because of that a delay has been introduced in the ```board_init``` function, so that any SPI device connected to the ports is reseted before used.

### SX127X radio
The pin configuration for the SX127X radio on the **boards/sensebox/include/board.h** file assumes that it will be connected to the XBEE1 port. If a different configuraion is needed please modify this file.

### SD card
The pin configuration for the mSD-Bee module radio on the **boards/sensebox/include/sdcard_spi_params.h** file assumes that it will be connected to the XBEE2 port. If a different configuraion is needed please modify this file.