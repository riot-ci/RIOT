include ../Makefile.tests_common

USEMODULE += od
USEMODULE += shell
USEMODULE += ps
USEMODULE += event_thread_highest
USEMODULE += netdev_ieee802154_submac
USEMODULE += netdev_default

CFLAGS += -DEVENT_THREAD_HIGHEST_STACKSIZE=1024

include $(RIOTBASE)/Makefile.include
