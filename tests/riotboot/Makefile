include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# Include modules to test the bootloader
USEMODULE += slot_util
USEMODULE += riot_hdr

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

all: riotboot

include $(RIOTBASE)/Makefile.include
