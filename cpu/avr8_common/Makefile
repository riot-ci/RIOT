# define the module that is build (not strictly necessary)
MODULE = avr8_common

# add a list of subdirectories, that should also be build
DIRS = avr_libc_extra

ifneq (,$(filter cpu_avr8_common_cxx,$(USEMODULE)))
  DIRS += cxx
endif

include $(RIOTBASE)/Makefile.base
