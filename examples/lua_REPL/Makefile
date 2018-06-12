APPLICATION = lua_repl

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_WHITELIST := frdm-k64f iotlab-m3 mulle native nrf52840dk nucleo-f411re \
				   stm32f4discovery

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Uncomment the following lines to enable debugging features.
#CFLAGS_OPT = -Og
#CFLAGS += -DDEBUG_ASSERT_VERBOSE -DLUA_DEBUG

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# This is a lot of stack, should be fixed in the future.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=8000

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
