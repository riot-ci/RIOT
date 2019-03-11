include ../Makefile.tests_common

BOARD_WHITELIST := arduino-uno arduino-mega2560

BOARD ?= arduino-uno

FEATURES_REQUIRED += periph_gpio
FEATURES_REQUIRED += periph_gpio_irq

USEMODULE += atmega_pcint

include $(RIOTBASE)/Makefile.include

ifeq (arduino-uno,$(BOARD))
  CFLAGS += "-DTHREAD_STACKSIZE_DEFAULT=256"
endif
