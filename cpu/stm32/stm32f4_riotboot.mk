# STM32F4 uses sectors instead of pages, where the minimum sector length is 16KB
# (the first sector), therefore RIOTBOOT_LEN must be 16KB to cover a whole sector.
RIOTBOOT_LEN ?= 0x4000

# CPU_IRQ_NUMOF for STM32F4 boards is < 102+16 so (118*4 bytes = 472 bytes ~= 0x200)
# RIOTBOOT_HDR_LEN can be set to 0x200.
# Details on alignment requirements for M4 in `cpu/cortexm_common/Makefile.include`.
RIOTBOOT_HDR_LEN ?= 0x200

# Sectors don't have the same length. Per bank there can be up to 12 sectors. The
# first 4 sectors are 16kB long, the 5th is 64kB and the remaining 7 are 128kB.
# Since flash can only be erased by sector, slots can't overlap over sectors.
# RIOTBOOT_LEN is removed twice, once at the start of the flash for the bootloader,
# and a second time at the end of the flash, to get evenly sized and distributed slots.
SLOT0_LEN ?= $(shell printf "0x%x" $$((($(ROM_LEN:%K=%*1024)-2*$(RIOTBOOT_LEN)) / $(NUM_SLOTS))))
SLOT1_LEN ?= $(SLOT0_LEN)
