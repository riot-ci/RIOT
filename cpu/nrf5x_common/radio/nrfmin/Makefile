MODULE = nrfmin

SRC = nrfmin.c

ifneq (,$(filter netdev_default,$(USEMODULE)))
  SRC += nrfmin_gnrc.c
endif

include $(RIOTBASE)/Makefile.base
