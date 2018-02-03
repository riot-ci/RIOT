include ../Makefile.tests_common

FEATURES_REQUIRED = periph_timer
USEMODULE += random
USEMODULE += fmt

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
