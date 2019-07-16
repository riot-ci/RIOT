include ../Makefile.tests_common

USEPKG += micropython

# MicroPython needs a larger stack
CFLAGS += -DTHREAD_STACKSIZE_MAIN=4096

#
include $(RIOTBASE)/Makefile.include
