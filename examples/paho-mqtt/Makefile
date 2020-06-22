APPLICATION = paho-mqtt-example

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEPKG += paho-mqtt

CFLAGS += -DESP_WIFI_SSID=\"define yours\" -DESP_WIFI_PASS=\"secret\"

ifneq (,$(filter arch_esp32,$(FEATURES_USED)))
  USEMODULE += esp_wifi
endif

ifneq ($(BOARD),native)
USEMODULE += netdev_default
endif

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

include $(RIOTBASE)/Makefile.include
