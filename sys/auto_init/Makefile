ifneq (,$(filter auto_init_gnrc_netif,$(USEMODULE)))
  DIRS += netif
endif

ifneq (,$(filter auto_init_saul,$(USEMODULE)))
  DIRS += saul
endif

ifneq (,$(filter auto_init_storage,$(USEMODULE)))
  DIRS += storage
endif

ifneq (,$(filter auto_init_can,$(USEMODULE)))
  DIRS += can
endif

SRC = auto_init.c

ifneq (,$(filter auto_init_skald,$(USEMODULE)))
  SRC += auto_init_skald.c
endif

include $(RIOTBASE)/Makefile.base
