DFU ?= dfu-util
FLASHER ?= $(DFU)
HEXFILE = $(BINFILE)

ifdef ROM_OFFSET
  FLASH_ADDR = $(shell printf "0x%x" $$(($(ROM_START_ADDR) + $(ROM_OFFSET))))
else
  FLASH_ADDR = $(ROM_START_ADDR)
endif

FFLAGS ?= --device $(DFU_ID) \
          --alt $(DFU_ALT) \
          --dfuse-address $(FLASH_ADDR):leave \
          --download $(HEXFILE) \
          --reset
