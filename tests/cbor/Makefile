APPLICATION = cbor
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno
BOARD_BLACKLIST += nucleo32-f031
BOARD_BLACKLIST += qemu-i368
BOARD_BLACKLIST += waspmote-pro

USEMODULE += cbor
USEMODULE += cbor_ctime
USEMODULE += cbor_float
USEMODULE += cbor_semantic_tagging

include $(RIOTBASE)/Makefile.include

test:
# `testrunner` calls `make term` recursively, results in duplicated `TERMFLAGS`.
# So clears `TERMFLAGS` before run.
	TERMFLAGS= tests/01-run.py
