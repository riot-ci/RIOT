include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031

USEMODULE += posix
USEMODULE += pthread
USEMODULE += xtimer

CFLAGS += -DNATIVE_AUTO_EXIT

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
