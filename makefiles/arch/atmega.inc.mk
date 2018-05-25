# Target architecture for the build. Use avr if you are unsure.
TARGET_ARCH ?= avr

CFLAGS_CPU   = -mmcu=$(CPU) $(CFLAGS_FPU)
CFLAGS_LINK  = -ffunction-sections -fdata-sections -fno-builtin -fshort-enums
CFLAGS_DBG  ?= -ggdb -g3
CFLAGS_OPT  ?= -Os

CFLAGS    += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT)
ASFLAGS   += $(CFLAGS_CPU) $(CFLAGS_DBG)
LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) -static -lgcc -e reset_handler -Wl,--gc-sections
OFLAGS    += -j .text -j .data

# Tell the build system that the CPU depends on the atmega common files:
USEMODULE += atmega_common

# export the peripheral drivers to be linked into the final binary
USEMODULE += atmega_common_periph
USEMODULE += periph_common

# Export the peripheral drivers to be linked into the final binary, for now
# only atmega126rfr2 has periph drivers
ifeq ($(CPU), atmega256rfr2)
  USEMODULE += periph
endif

# the atmel port uses uart_stdio
USEMODULE += uart_stdio

# explicitly tell the linker to link the syscalls and startup code.
# without this the interrupt vectors will not be linked correctly!
UNDEF += $(BINDIR)/atmega_common/startup.o

ifeq ($(LTO),1)
  # avr-gcc <4.8.3 has a bug when using LTO which causes a warning to be printed always:
  # '_vector_25' appears to be a misspelled signal handler [enabled by default]
  # See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59396
  LINKFLAGS += -Wno-error
endif
