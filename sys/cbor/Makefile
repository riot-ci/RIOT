MODULE = cbor

ifneq ($(shell uname -s),Darwin)
	CFLAGS += -D_XOPEN_SOURCE=600
else
	CFLAGS += -D_GNU_SOURCE=1
endif

include $(RIOTBASE)/Makefile.base
