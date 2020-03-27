APPLICATION = openwsn_sctimer

BOARD ?= iotlab-m3

include ../Makefile.tests_common

USEPKG += openwsn
USEMODULE += openwsn_sctimer

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter stm32f1,$(CPU)))
  CFLAGS += -DRTT_FREQUENCY=16384
endif
