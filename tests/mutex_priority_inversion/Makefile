include ../Makefile.tests_common

USEMODULE += xtimer

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
