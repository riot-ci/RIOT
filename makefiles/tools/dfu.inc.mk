DFU ?= dfu-util
FLASHER ?= $(DFU)
FLASHFILE ?= $(BINFILE)

_ROM_ADDR_WITH_OFFSET ?= $(shell printf "0x%x" $$(($(ROM_START_ADDR) + $(ROM_OFFSET))))
FLASH_ADDR ?= $(if $(ROM_OFFSET),$(_ROM_ADDR_WITH_OFFSET),$(ROM_START_ADDR))

FFLAGS ?= --device $(DFU_ID) \
          --alt $(DFU_ALT) \
          --dfuse-address $(FLASH_ADDR):leave \
          --download $(FLASHFILE) \
          --reset
