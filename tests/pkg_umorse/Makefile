APPLICATION = pkg_umorse
include ../Makefile.tests_common

USEMODULE += xtimer

USEPKG += umorse

CFLAGS += -DUMORSE_DELAY_DIT=120

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
