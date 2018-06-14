include ../Makefile.tests_common

# no need to build this test for all targets, so just build for some selected
# boards
BOARD_WHITELIST += native nrf52dk samr21-xpro z1 arduino-mega2560

# disabled by default:
# USEMODULE += core_thread_flags
#
# enabled by default:
# DISABLE_MODULE += core_msg

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
