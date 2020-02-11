MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS += periph
DIRS += $(RIOTCPU)/cortexm_common
DIRS += families/$(filter cpu_efm32%,$(USEMODULE))

include $(RIOTBASE)/Makefile.base
