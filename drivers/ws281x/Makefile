SRC := ws281x.c # All other sources files provide ws281x_write as submodule
SUBMODULES := 1

ifneq (,$(filter ws281x_vt100,$(USEMODULE)))
  CFLAGS += -DWS281X_HAVE_PREPARE_TRANSMISSION
  CFLAGS += -DWS281X_HAVE_END_TRANSMISSION
endif

include $(RIOTBASE)/Makefile.base
