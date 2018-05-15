SRC := $(filter-out loadlib.c lua.c luac.c,$(wildcard *.c))

# This is very important because lua defaults to 64 bit integers
CFLAGS += -DLUA_32BITS -fstack-usage -Wstack-usage=128 \
          -Wno-error=stack-usage=128 -fconserve-stack \
          -DLUAL_BUFFERSIZE=64 \
          -DLUA_MAXCAPTURES=16 \
          -DL_MAXLENNUM=50

include $(RIOTBASE)/Makefile.base
