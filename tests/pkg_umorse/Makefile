APPLICATION = pkg_umorse
include ../Makefile.tests_common

USEMODULE += xtimer

USEPKG += umorse

UMORSE_DELAY_DIT ?= 120
CFLAGS += -DUMORSE_DELAY_DIT=$(UMORSE_DELAY_DIT)

FEATURES_REQUIRED += periph_gpio

include $(RIOTBASE)/Makefile.include
