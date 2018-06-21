include ../Makefile.tests_common

# Required on Arduino
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(5*THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF\)

USEMODULE += embunit
USEMODULE += random
USEPKG += c25519

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
