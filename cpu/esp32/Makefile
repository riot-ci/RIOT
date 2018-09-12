# Define the module that is built:
MODULE = cpu

EXCLUDE += esp-eth
EXCLUDE += esp-now
EXCLUDE += esp-wifi
EXCLUDE += freertos
EXCLUDE += periph
EXCLUDE += vendor

# Add a list of subdirectories, that should also be built:
DIRS += periph
DIRS += freertos
DIRS += vendor

ifneq (, $(filter esp_now, $(USEMODULE)))
    DIRS += esp-now
endif

ifneq (, $(filter esp_eth, $(USEMODULE)))
    DIRS += esp-eth
endif

ifneq (, $(filter esp_wifi, $(USEMODULE)))
    DIRS += esp-wifi
endif

include $(RIOTBASE)/Makefile.base
