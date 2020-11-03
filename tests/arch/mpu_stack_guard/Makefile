BOARD ?= samr21-xpro
RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

USEMODULE += mpu_stack_guard

include $(RIOTBASE)/Makefile.include

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-infinite-recursion
endif
