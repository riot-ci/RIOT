include ../Makefile.tests_common

USEPKG += lua

BOARD_WHITELIST += native samr21-xpro

ifneq ($(BOARD),native)
  # This stack size is large enough to run Lua print() functions of
  # various lengths. Other functions untested.
  CFLAGS += -DTHREAD_STACKSIZE_MAIN='(THREAD_STACKSIZE_DEFAULT+2048)'
endif

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
