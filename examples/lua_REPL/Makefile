APPLICATION = lua

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_commands

ifneq ($(BOARD),native)
# This stack size is large enough to run Lua print() functions of
# various lengths. Other functions untested.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=4096
endif

USEPKG += lua

include $(RIOTBASE)/Makefile.include

# The code below generates a header file from any .lua scripts in the
# example directory. The header file contains a byte array of the
# ASCII characters in the .lua script.

# generate .lua.h header files of .lua files
LUA_FILES = $(wildcard *.lua)

LUA_PRECOMPILED := $(LUA_FILES:%.lua=%.lua.h)

%.lua.h: %.lua
	xxd -i $< | sed 's/^unsigned/static const/g' > $@

$(RIOTBUILD_CONFIG_HEADER_C): $(LUA_PRECOMPILED)
