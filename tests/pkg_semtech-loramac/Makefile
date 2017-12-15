APPLICATION = pkg_semtech-loramac
include ../Makefile.tests_common

BOARD ?= b-l072z-lrwan1

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042 nucleo32-l031

DRIVER ?= sx1276
REGION ?= EU868

USEPKG += semtech-loramac
USEMODULE += $(DRIVER)

USEMODULE += shell
USEMODULE += shell_commands

CFLAGS += -DREGION_$(REGION)

include $(RIOTBASE)/Makefile.include
