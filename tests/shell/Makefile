DEVELHELP=0
include ../Makefile.tests_common

USEMODULE += app_metadata
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

DISABLE_MODULE += test_utils_interactive_sync

APP_SHELL_FMT ?= NONE

include $(RIOTBASE)/Makefile.include
