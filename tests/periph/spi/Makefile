BOARD ?= samr21-xpro
RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

FEATURES_REQUIRED += periph_spi
FEATURES_OPTIONAL += periph_spi_reconfigure

USEMODULE += xtimer
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += schedstatistics

include $(RIOTBASE)/Makefile.include
