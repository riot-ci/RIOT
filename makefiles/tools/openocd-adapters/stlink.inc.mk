# ST-Link debug adapter
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
  ifeq ($(CPU),stm32f0)
    export OPENOCD_CONFIG := $(RIOTMAKE)/tools/openocd-adapters/configs/stm32f0.cfg
  else ifeq ($(CPU),stm32f1)
    export OPENOCD_CONFIG := $(RIOTMAKE)/tools/openocd-adapters/configs/stm32f1.cfg
  else ifeq ($(CPU),stm32f2)
    export OPENOCD_CONFIG := $(RIOTMAKE)/tools/openocd-adapters/configs/stm32f2.cfg
  else ifeq ($(CPU),stm32f3)
    export OPENOCD_CONFIG := $(RIOTMAKE)/tools/openocd-adapters/configs/stm32f3.cfg
  else ifeq ($(CPU),stm32f4)
    export OPENOCD_CONFIG := $(RIOTMAKE)/tools/openocd-adapters/configs/stm32f4.cfg
  else ifeq ($(CPU),stm32f7)
    export OPENOCD_CONFIG := $(RIOTMAKE)/tools/openocd-adapters/configs/stm32f7.cfg
  else ifeq ($(CPU),stm32l0)
    export OPENOCD_CONFIG := $(RIOTMAKE)/tools/openocd-adapters/configs/stm32l0.cfg
  else ifeq ($(CPU),stm32l1)
    export OPENOCD_CONFIG := $(RIOTMAKE)/tools/openocd-adapters/configs/stm32l1.cfg
  else ifeq ($(CPU),stm32l4)
    export OPENOCD_CONFIG := $(RIOTMAKE)/tools/openocd-adapters/configs/stm32l4.cfg
  endif
endif
