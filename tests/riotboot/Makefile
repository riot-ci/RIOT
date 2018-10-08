include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# Select the boards that were tested and work with riotboot
BOARD_WHITELIST += samr21-xpro iotlab-m3

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
