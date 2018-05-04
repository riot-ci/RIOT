include ../Makefile.tests_common

# avr8, msp430 and mips don't support ssp (yet)
FEATURES_BLACKLIST += arch_avr8 arch_msp430 arch_mips32r2

USEMODULE += ssp

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
