# Define the module that is built:
MODULE = cpu

EXCLUDE += esp-idf
EXCLUDE += esp-net-espnow
EXCLUDE += esp-net-espeth
EXCLUDE += periph
EXCLUDE += freertos
EXCLUDE += xtensa

# Add a list of subdirectories, that should also be built:
DIRS += esp-idf
DIRS += periph
DIRS += freertos
DIRS += xtensa

ifneq (, $(filter esp_net_espnow, $(USEMODULE)))
    DIRS += esp-net-espnow
endif

ifneq (, $(filter esp_net_espeth, $(USEMODULE)))
    DIRS += esp-net-espeth
endif

include $(RIOTBASE)/Makefile.base
