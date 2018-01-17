BOARD ?= b-l072z-lrwan1

include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042 nucleo32-l031

BOARD_BLACKLIST := msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

DRIVER ?= sx1276
REGION ?= EU868

USEPKG += semtech-loramac
USEMODULE += $(DRIVER)

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += fmt

CFLAGS += -DREGION_$(REGION)

include $(RIOTBASE)/Makefile.include
