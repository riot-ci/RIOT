MODULE = stm32_vectors

NO_AUTO_SRC = 1

SRC = $(CPU_LINE).c

# (file triggers compiler bug. see #5775)
SRC_NOLTO += $(SRC_FILE)

include $(RIOTBASE)/Makefile.base
