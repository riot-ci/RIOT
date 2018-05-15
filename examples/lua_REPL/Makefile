APPLICATION = lua_repl

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

CFLAGS_OPT = -Og

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# This is a lot of stack, should be fixed in the future.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=8000 -DDEBUG_ASSERT_VERBOSE -DLUA_DEBUG

# USEMODULE += tlsf-malloc
USEPKG += lua
#USEMODULE += newlib

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
