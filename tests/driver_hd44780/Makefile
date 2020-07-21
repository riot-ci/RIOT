include ../Makefile.tests_common

USEMODULE += hd44780

# disable native GPIO access
DISABLE_MODULE += periph_gpio_linux

# Fails on esp32 because the driver defines default GPIOs that are used for the
# SPI flash interface.
TEST_ON_CI_BLACKLIST += esp32-wroom-32

include $(RIOTBASE)/Makefile.include
