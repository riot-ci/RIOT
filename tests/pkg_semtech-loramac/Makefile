BOARD ?= b-l072z-lrwan1

include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042 nucleo32-l031

FEATURES_BLACKLIST += arch_msp430

LORA_DRIVER ?= sx1276
LORA_REGION ?= EU868

USEPKG += semtech-loramac
USEMODULE += $(LORA_DRIVER)

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += fmt

CFLAGS += -DREGION_$(LORA_REGION)

include $(RIOTBASE)/Makefile.include
