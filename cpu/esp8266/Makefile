# Define the module that is built:
MODULE = cpu

# Add a list of subdirectories, that should also be built:
DIRS += periph
DIRS += sdk
DIRS += vendor

ifneq (, $(filter esp_now, $(USEMODULE)))
    DIRS += esp-now
endif

include $(RIOTBASE)/Makefile.base
