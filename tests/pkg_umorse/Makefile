APPLICATION = pkg_umorse
include ../Makefile.tests_common

USEMODULE += xtimer

USEPKG += umorse

CFLAGS += -DUMORSE_DELAY_DIT=120

FEATURES_REQUIRED += periph_gpio

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
