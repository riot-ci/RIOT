# Target triple for the build. Use arm-none-eabi if you are unsure.
export TARGET_ARCH ?= arm-none-eabi

# Currently only arm7tdmi-s is supported, so we just make this the default but
# allow to override it for other ARM7 CPUs
MCPU ?= arm7tdmi-s
# define build specific options
export CFLAGS_CPU   = -mcpu=$(MCPU)

export CFLAGS_LINK  = -ffunction-sections -fdata-sections -fno-builtin -fshort-enums
export CFLAGS_DBG  ?= -ggdb -g3
export CFLAGS_OPT  ?= -Os

export CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT)

export ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG)
export LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts
export LINKER_SCRIPT ?= $(CPU).ld
export LINKFLAGS += -T$(LINKER_SCRIPT) -Wl,--fatal-warnings

export LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) -static -lgcc -nostartfiles
export LINKFLAGS += -Wl,--gc-sections

# Export the peripheral drivers to be linked into the final binary:
export USEMODULE += periph

# include common periph code
export USEMODULE += arm7_common_periph

# use newlib as libc
export USEMODULE += newlib

# set default for CPU_MODEL
export CPU_MODEL ?= $(CPU)

# CPU depends on the ARM7 common module, so include it:
include $(RIOTCPU)/arm7_common/Makefile.include

# use the nano-specs of Newlib when available
USEMODULE += newlib_nano
export USE_NANO_SPECS = 1

# Avoid overriding the default rule:
all:

# Rule to generate assembly listings from ELF files:
%.lst: %.elf
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@
