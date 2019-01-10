DEVELHELP ?= 0
include ../Makefile.tests_common

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE

DISABLE_MODULE += auto_init

# adjust stack sizes to very small values
ifneq (native,$(BOARD))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=256 -DTHREAD_STACKSIZE_IDLE=128
endif

include $(RIOTBASE)/Makefile.include
