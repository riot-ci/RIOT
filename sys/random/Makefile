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

ifneq (,$(filter puf_sram,$(USEMODULE)))
  HAS_RNG = 1
else ifneq (,$(filter periph_hwrng periph_cpuid,$(FEATURES_USED)))
  HAS_RNG = 1
endif

ifndef HAS_RNG
  CFLAGS += -DRANDOM_SEED_DEFAULT=$(RANDOM_SEED)U
  .PHONY: $(BINDIR)/$(MODULE)/init.o
endif

include $(RIOTBASE)/Makefile.base
