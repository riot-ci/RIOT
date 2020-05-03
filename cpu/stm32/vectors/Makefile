MODULE = stm32_vectors

NO_AUTO_SRC = 1

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors_$(subst stm32,,$(CPU_FAM)).c

include $(RIOTBASE)/Makefile.base