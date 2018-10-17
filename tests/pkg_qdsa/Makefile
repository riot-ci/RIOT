include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += all

BOARDS_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT\)

USEPKG += qDSA
USEMODULE += random
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
