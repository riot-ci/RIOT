APPLICATION = rng
include ../Makefile.tests_common

# override PRNG if desired (see sys/random for alternatives)
# USEMODULE += prng_minstd

USEMODULE += fmt
USEMODULE += printf_float
USEMODULE += random
USEMODULE += shell
USEMODULE += xtimer

FEATURES_OPTIONAL += periph_hwrng

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
