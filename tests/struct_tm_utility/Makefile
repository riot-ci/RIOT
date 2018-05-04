include ../Makefile.tests_common

DISABLE_MODULE += auto_init

USEMODULE += shell
USEMODULE += timex

# The MSP-430 toolchain lacks sscanf:
FEATURES_BLACKLIST += arch_msp430

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
