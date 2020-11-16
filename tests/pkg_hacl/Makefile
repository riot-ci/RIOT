include ../Makefile.tests_common

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(5*THREAD_STACKSIZE_DEFAULT\)
USEPKG += hacl
USEMODULE += random
USEMODULE += embunit

# upstream repo has moved - quick fix
TEST_ON_CI_BLACKLIST = all

include $(RIOTBASE)/Makefile.include
