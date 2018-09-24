# Default RIOT bootloader
APPLICATION = riotboot

# Default testing board
BOARD ?= samr21-xpro

# Select the boards that were tested and work with riotboot
BOARD_WHITELIST += samr21-xpro iotlab-m3

# Disable unused modules
CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += auto_init

# Shared interface to read RIOT header
USEMODULE += riot_hdr
USEMODULE += slot_util

# RIOT codebase
RIOTBASE ?= $(CURDIR)/../../

include $(RIOTBASE)/Makefile.include

# limit riotboot bootloader size
# TODO: Manage to set this variable for boards which already embed a
# bootloader, currently it will be overwritten
ROM_LEN := $(RIOTBOOT_LEN)
