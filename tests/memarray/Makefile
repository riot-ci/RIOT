include ../Makefile.tests_common
USEMODULE += memarray

# Used for invoking _ps_handler
USEMODULE += shell_commands
USEMODULE += ps

NUMBER_OF_TESTS ?= 12
MAX_NUMBER_BLOCKS ?= 10

CFLAGS += -DNUMBER_OF_TESTS=$(NUMBER_OF_TESTS)
CFLAGS += -DMAX_NUMBER_BLOCKS="$(MAX_NUMBER_BLOCKS)"

include $(RIOTBASE)/Makefile.include

# Export for Python test script
export NUMBER_OF_TESTS
export MAX_NUMBER_BLOCKS
