include ../Makefile.tests_common

DRIVER ?= bme680_i2c

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno \
                             chronos msb-430 msb-430h nucleo-f031k6 telosb \
                             wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += $(DRIVER)
USEMODULE += xtimer

ifeq ($(ENABLE_FP),1)
  USEMODULE += bme680_fp
endif

include $(RIOTBASE)/Makefile.include
