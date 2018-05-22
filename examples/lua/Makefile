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

LUA_PATH := $(BINDIR)/lua

# add directory of generated *.lua.h files to include path
CFLAGS += -I$(LUA_PATH)

# generate .lua.h header files of .lua files
LUA = $(wildcard *.lua)

LUA_H := $(LUA:%.lua=$(LUA_PATH)/%.lua.h)

$(LUA_PATH)/:
	@mkdir -p $@

$(LUA_H): | $(LUA_PATH)/
$(LUA_H): $(LUA_PATH)/%.lua.h: %.lua

	xxd -i $< | sed 's/^unsigned/const/g' > $@

$(RIOTBUILD_CONFIG_HEADER_C): $(LUA_H)
