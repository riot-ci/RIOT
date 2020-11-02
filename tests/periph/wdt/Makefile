BOARD ?= nucleo-l152re
RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

FEATURES_REQUIRED += periph_wdt

USEMODULE += xtimer
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
