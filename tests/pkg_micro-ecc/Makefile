include ../Makefile.tests_common

USEMODULE += hashes
USEMODULE += micro-ecc

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
