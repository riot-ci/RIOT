include ../Makefile.tests_common

FEATURES_BLACKLIST := arch_avr8 arch_mips32r2 arch_msp430
BOARD_BLACKLIST := nucleo32-f031

USEMODULE += cbor
USEMODULE += cbor_ctime
USEMODULE += cbor_float
USEMODULE += cbor_semantic_tagging

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
