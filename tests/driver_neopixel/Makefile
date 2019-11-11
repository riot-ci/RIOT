BOARD ?= atmega328p
include ../Makefile.tests_common

ifeq (atmega328p,$(BOARD))
  PIN ?= GPIO_PIN(PORT_B, 0)
  N ?= 10
else
  ifneq (,$(filter arduino-nano arduino-uno,$(BOARD)))
    PIN ?= GPIO_PIN(PORT_D, 2) # Digital pin 2 on Arduino Uno / Nano
  else
    PIN ?= GPIO_PIN(0,0)
  endif
  N ?= 8
endif

USEMODULE += neopixel

include $(RIOTBASE)/Makefile.include

CFLAGS += '-DNEOPIXEL_PARAM_PIN=$(PIN)'
CFLAGS += '-DNEOPIXEL_PARAM_NUMOF=$(N)'
