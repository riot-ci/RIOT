MODULE = cbor

ifneq (,$(filter cbor_ctime,$(USEMODULE)))
  # Some toolchains need to set this for strptime, specially on OSX
  CFLAGS += -D_GNU_SOURCE=1
endif

ifneq ($(shell uname -s),Darwin)
  CFLAGS += -D_XOPEN_SOURCE=600
endif

include $(RIOTBASE)/Makefile.base
