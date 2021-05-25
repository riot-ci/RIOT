include ../Makefile.tests_common

# Edhoc related packages
USEPKG += edhoc-c
USEMODULE += edhoc-c_crypto_tinycrypt
USEMODULE += edhoc-c_cbor_nanocbor
USEMODULE += embunit
USEMODULE += random

CFLAGS += -DTHREAD_STACKSIZE_MAIN=3*THREAD_STACKSIZE_LARGE
CFLAGS += -DDEBUG_ASSERT_VERBOSE

include $(RIOTBASE)/Makefile.include
