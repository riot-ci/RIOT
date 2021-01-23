MODULE = cc26x2_cc13x2_rf

SRC = cc26x2_cc13x2_rf_internal.c

ifneq (,$(filter ieee802154_radio_hal,$(USEMODULE)))
  SRC += cc26x2_cc13x2_rf_radio_ops.c
else
  SRC += cc26x2_cc13x2_rf_netdev.c
endif

include $(RIOTBASE)/Makefile.base
