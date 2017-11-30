# name of your application
APPLICATION = memarray
include ../Makefile.tests_common

USEMODULE += memarray

# Used for invoking _ps_handler
USEMODULE += shell_commands
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
