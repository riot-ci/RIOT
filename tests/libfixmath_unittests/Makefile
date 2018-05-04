include ../Makefile.tests_common

# arduino-mega2560: builds locally but breaks travis (possibly because of
# differences in the toolchain)
FEATURES_BLACKLIST += arch_avr8

# The MSP boards don't feature round(), exp(), and log(), which are used in the unittests
FEATURES_BLACKLIST += arch_msp430

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042 nucleo32-l031

USEMODULE += libfixmath-unittests

ifneq (,$(filter native,$(BOARD)))
  export LINKFLAGS += -lm
endif

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
