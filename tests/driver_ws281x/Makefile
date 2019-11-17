BOARD ?= atmega328p
include ../Makefile.tests_common

# Update this to your needs
PIN ?= GPIO_PIN(0, 0)
N ?= 8

USEMODULE += ws281x

FEATURES_REQUIRED := arch_avr8 # <-- Currently only backend for AVR boards

include $(RIOTBASE)/Makefile.include

CFLAGS += '-DWS281X_PARAM_PIN=$(PIN)'
CFLAGS += '-DWS281X_PARAM_NUMOF=$(N)'
