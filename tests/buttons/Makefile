APPLICATION = buttons
include ../Makefile.tests_common

USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include

test:
# `testrunner` calls `make term` recursively, results in duplicated `TERMFLAGS`.
# So clears `TERMFLAGS` before run.
	TERMFLAGS= tests/01-run.py
