include ../Makefile.tests_common

USEPKG += minmea

# The MSP-430 toolchain lacks mktime and NAN
FEATURES_BLACKLIST += arch_msp430

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
