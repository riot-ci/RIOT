MODULE = stm32_common

DIRS = periph

ifneq (,$(filter can_stm32,$(USEMODULE)))
    DIRS += can
endif

include $(RIOTBASE)/Makefile.base
