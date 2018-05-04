include ../Makefile.tests_common

# exclude boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := nucleo32-f031

# Modules to include.
USEMODULE += pthread
USEMODULE += random
USEMODULE += xtimer

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
