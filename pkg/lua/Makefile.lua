SRC := $(filter-out lua.c luac.c,$(wildcard *.c))

# This builds for native using POSIX system calls and some extra libraries, and
# removes a compiler warning that warns against using tmpnam().
ifeq ($(BOARD),native)
CFLAGS += -DLUA_USE_LINUX
endif

# This redefines a lua prng function, avoiding the need for the _times system
# call, which RIOT doesn't provide
CFLAGS += -D'l_randomizePivot()=0'

include $(RIOTBASE)/Makefile.base
