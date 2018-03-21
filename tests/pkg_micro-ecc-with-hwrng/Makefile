include ../Makefile.tests_common

FEATURES_REQUIRED = periph_hwrng

USEPKG += micro-ecc

TEST_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
