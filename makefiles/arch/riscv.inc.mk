# Target architecture for the build.
TARGET_ARCH ?= riscv-none-elf

# If TARGET_ARCH wasn't set by user, fall back to legacy riscv-none-embed
# triplet if ricsv-none-elf is missing but riscv-none-embed is present
ifeq (riscv-none-elf,$(TARGET_ARCH))
  ifeq (,$(shell which $(TARGET_ARCH)-gcc))
    ifneq (,$(shell which riscv-none-embed-gcc))
      $(info Falling back to legacy riscv-none-embed toolchain)
      TARGET_ARCH := riscv-none-embed
    endif
  endif
endif

# define build specific options
CFLAGS_CPU   = -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8
CFLAGS_LINK  = -nostartfiles -ffunction-sections -fdata-sections
CFLAGS_DBG  ?= -g3
CFLAGS_OPT  ?= -Os

LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts
LINKER_SCRIPT ?= $(CPU_MODEL).ld
LINKFLAGS += -T$(LINKER_SCRIPT)

CFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) $(CFLAGS_LINK)
ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG)
# export linker flags
LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT) -Wl,--gc-sections -static -lgcc
