BOARD ?= b-l072z-lrwan1
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_eeprom

# Required for STM32 MCUs since eeprom and flashpage share some code
FEATURES_OPTIONAL += periph_flashpage

USEMODULE += shell

include $(RIOTBASE)/Makefile.include
