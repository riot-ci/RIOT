BOARD ?= samr21-xpro
RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

FEATURES_REQUIRED = periph_i2c
FEATURES_OPTIONAL = periph_i2c_reconfigure

USEMODULE += shell
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
