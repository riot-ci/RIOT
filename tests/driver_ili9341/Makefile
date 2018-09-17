BOARD ?= stm32f429i-disc1
include ../Makefile.tests_common

USEMODULE += ili9341
USEMODULE += xtimer

CFLAGS += -DILI9341_LE_MODE

include $(RIOTBASE)/Makefile.include
