# ST-Link debug adapter
# Use st-link v2-1 by default
STLINK_VERSION ?= 2-1

# Use STLINK_VERSION to select which stlink version is used
OPENOCD_ADAPTER_INIT ?= \
  -c 'source [find interface/stlink-v$(STLINK_VERSION).cfg]' \
  -c 'transport select hla_swd'
# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  OPENOCD_ADAPTER_INIT += -c 'hla_serial $(DEBUG_ADAPTER_ID)'
endif
export OPENOCD_ADAPTER_INIT

# select OpenOCD configuration depending on CPU type
ifeq (,$(OPENOCD_CONFIG))
  export OPENOCD_CONFIG := $(RIOTMAKE)/tools/openocd-adapters/configs/$(CPU).cfg
endif
