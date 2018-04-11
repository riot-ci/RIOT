APPLICATION = lua

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := bluepill calliope-mini cc2650-launchpad \
                             cc2650stk maple-mini microbit nrf51dongle \
                             nucleo-f030r8 nucleo-f031k6 nucleo-f042k6 \
                             nucleo-f070rb nucleo-f072rb nucleo-f103rb \
                             nucleo-f302r8 nucleo-f303k8 nucleo-f334r8 \
                             nucleo-f410rb nucleo-l031k6 nucleo-l053r8 \
                             opencm904 spark-core stm32f0discovery \
                             stm32mindev

BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno \
                   chronos hifive1 jiminy-mega256rfr2 mega-xplained mips-malta \
                   msb-430 msb-430h pic32-clicker pic32-wifire telosb \
                   waspmote-pro wsn430-v1_3b wsn430-v1_4 z1


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += ps
USEMODULE += lua-contrib

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

# add directory of generated *.lua.c files to include path
CFLAGS += -I$(LUA_PATH)

# generate .lua.h header files of .lua files
LUA = $(wildcard *.lua)

LUA_PRECOMPILED := $(LUA:%.lua=$(LUA_PATH)/%.lua.h)
LUA_MODSREG_C = $(LUA_PATH)/lua_modules.c
LUA_MODSREG_H = $(LUA_PATH)/lua_modules.h

$(LUA_PATH)/:
	@mkdir -p $@

$(LUA_PRECOMPILED): | $(LUA_PATH)/
$(LUA_PRECOMPILED): $(LUA_PATH)/%.lua.h: %.lua
	xxd -i $< | sed 's/^unsigned/static const/g' > $@

LUA_PRECOMPILED_NAMES = $(LUA:%.lua=%)

$(LUA_MODSREG_H): $(LUA_PRECOMPILED)
	echo "/* Automatically generated file. DO NOT EDIT */" > $@
	echo "#ifndef RIOT_APP_LUA_MODULES_H" >> $@
	echo "#define RIOT_APP_LUA_MODULES_H" >> $@
	echo '#include "loadlib.h"'
	echo "extern struct luamodule lua_lmodules[];"
	echo "#endif /* RIOT_APP_LUA_MODULES_H */" >> $@

$(LUA_MODSREG_C): $(LUA_PRECOMPILED)
	echo "/* Automatically generated file. DO NOT EDIT */" > $@
	for n in $(LUA_PRECOMPILED_NAMES) ; do \
		echo "/* $${n} */" >> $@; \
		echo "extern const char *$${n}_lua;" >> $@; \
		echo "extern const int $${n}_lua_len;" >> $@; \
	done
	echo "#endif /* RIOT_APP_LUA_MODULES_H */" >> $@

$(RIOTBUILD_CONFIG_HEADER_C): $(LUA_MODSREG)
