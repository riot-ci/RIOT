# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

TEST_ON_CI_WHITELIST += all

# Select the boards with riotboot feature
FEATURES_REQUIRED += riotboot

# Include modules to test the bootloader
USEMODULE += riotboot_slot
USEMODULE += shell

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

ifneq ($(BUILD_IN_DOCKER),1)
# HACK: When building with docker, riotboot target must be done only in docker
all: riotboot
endif

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include

# Make 'flash' and 'flash-only' work without specific command.
# This is currently hacky as there is no way of specifiying a FLASHFILE
ifneq ($(BUILD_IN_DOCKER),1)
# HACK: When building with docker, riotboot target must be done only in docker
all: riotboot/combined-slot0
endif
# openocd
ELFFILE = $(BINDIR_APP)-slot0-combined.bin
# edbg
HEXFILE = $(BINDIR_APP)-slot0-combined.bin
# murdock uses ':=' to get the flashfile variable so should also be overwritten
FLASHFILE = $(BINDIR_APP)-slot0-combined.bin
