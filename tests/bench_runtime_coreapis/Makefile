include ../Makefile.tests_common

# we use thread flags in this benchmark by default, disable on demand
USEMODULE += core_thread_flags
USEMODULE += benchmark

# no need to build this test for all targets, so just build for some selected
# boards
BOARD_WHITELIST += native nrf52dk samr21-xpro z1 arduino-mega2560

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
