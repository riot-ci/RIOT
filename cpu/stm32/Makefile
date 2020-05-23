MODULE = cpu

DIRS = $(RIOTCPU)/cortexm_common periph vectors

ifneq (,$(filter bootloader_stm32,$(USEMODULE)))
  DIRS += bootloader
endif

include $(RIOTBASE)/Makefile.base
