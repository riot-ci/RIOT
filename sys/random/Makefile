# module name is used below, thus set explicitly
MODULE = random

SRC := random.c init.c

BASE_MODULE := prng
SUBMODULES := 1

ifneq (,$(filter prng_fortuna,$(USEMODULE)))
  DIRS += fortuna
endif

ifneq (,$(filter prng_tinymt32,$(USEMODULE)))
  DIRS += tinymt32
endif

ifeq (,$(RIOT_CI_BUILD))
  # random seed value newly generated on each build
  RANDOM_SEED := $(shell $(RIOTTOOLS)/randhex/randhex.py 32)
else
  # hardcoded canary value to not break CI test cache
  RANDOM_SEED := 0x1
endif

CFLAGS += -DRANDOM_SEED_DEFAULT=$(RANDOM_SEED)U
.PHONY: $(BINDIR)/$(MODULE)/init.o

include $(RIOTBASE)/Makefile.base
