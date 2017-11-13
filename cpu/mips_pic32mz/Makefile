MODULE = cpu

DIRS += $(RIOTCPU)/mips_pic32_common

ifneq (,$(filter newlib_syscalls_mips_uhi,$(USEMODULE)))
  DIRS += $(RIOTCPU)/mips32r2_common/newlib_syscalls_mips_uhi
endif

include $(RIOTBASE)/Makefile.base
