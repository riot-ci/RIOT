include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos nucleo32-f031 nucleo32-f042 nucleo32-l031 \
                             telosb wsn430-v1_3b wsn430-v1_4
USEMODULE += embunit
USEMODULE += fmt
USEPKG += tinycbor

TEST_ON_CI_WHITELIST += all
include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
