include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno nucleo-f031k6

TEST_ON_CI_WHITELIST += all

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(THREAD_STACKSIZE_DEFAULT+2048\)

USEPKG += tweetnacl
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
