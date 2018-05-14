# Target architecture for the build. Use avr if you are unsure.
export TARGET_ARCH ?= avr

export CFLAGS_CPU   = -mmcu=$(CPU) $(CFLAGS_FPU)
export CFLAGS_LINK  = -ffunction-sections -fdata-sections -fno-builtin -fshort-enums
export CFLAGS_DBG  ?= -ggdb -g3
export CFLAGS_OPT  ?= -Os

export CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT)
export ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG)
export LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) -static -lgcc -e reset_handler -Wl,--gc-sections

OFLAGS += -j .text -j .data

# Tell the build system that the CPU depends on the atmega common files:
export USEMODULE += atmega_common

# export the peripheral drivers to be linked into the final binary
export USEMODULE += atmega_common_periph
export USEMODULE += periph_common

# the atmel port uses uart_stdio
export USEMODULE += uart_stdio

# explicitly tell the linker to link the syscalls and startup code.
# without this the interrupt vectors will not be linked correctly!
export UNDEF += $(BINDIR)/atmega_common/startup.o

ifeq ($(LTO),1)
  # avr-gcc <4.8.3 has a bug when using LTO which causes a warning to be printed always:
  # '_vector_25' appears to be a misspelled signal handler [enabled by default]
  # See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59396
  export LINKFLAGS += -Wno-error
endif
