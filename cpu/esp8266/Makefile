# Define the module that is built:
MODULE = cpu

EXCLUDE += esp
EXCLUDE += periph
EXCLUDE += sdk
EXCLUDE += xtensa

# Add a list of subdirectories, that should also be built:
DIRS += esp
DIRS += periph
DIRS += sdk
DIRS += xtensa

ifeq ($(ENABLE_GDBSTUB), 1)
    DIRS += esp-gdbstub
endif

include $(RIOTBASE)/Makefile.base
