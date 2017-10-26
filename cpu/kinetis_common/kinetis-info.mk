# Split the part number into family, subfamily, core, memory, package, speed
#KINETIS_INFO := $(shell echo $(CPU_MODEL)|nawk '{ \
#  match(toupper($$0),/M?K([ELMSVW]?)([0-9])([0-9])([A-Z])([NX]?)([0-9][0-9M][0-9])(.)(..)([0-9]+)/, arr); \
#  if (arr[1] == "") { arr[1] = "K"; } \
#  if (arr[5] == "") { arr[5] = "Z"; } \
#  for (i=1; i<=8; ++i) { print arr[i]; } }')
KINETIS_INFO := $(shell printf '%s' '$(CPU_MODEL)' | tr 'a-z' 'A-Z' |sed -E -e 's/^(M|K|S9)K([ELMSVW]?)([0-9])([0-9])([A-Z])([NX]?)([0-9][0-9M][0-9])(.)(.*)$$/\1 \2 \3 \4 \5 \6 \7 \8:\9/' -e 's/^([^ ]*)  /\1 K /' -e 's/^([^:]*):(..)([0-9]+)([A-Z]?)$$/\1 \2 \3 \4/' -e 's/  / _ /g')


export KINETIS_SERIES := $(word 2, $(KINETIS_INFO))
export KINETIS_FAMILY := $(word 3, $(KINETIS_INFO))
export KINETIS_SUBFAMILY := $(word 4, $(KINETIS_INFO))
# Core type, D = Cortex-M4, F = Cortex-M4/M7 w/ FPU, Z = Cortex-M0
export KINETIS_CORE := $(word 5, $(KINETIS_INFO))
# FlexRAM (X) or not (N), does not apply to all families
export KINETIS_FLEXMEM := $(word 6, $(KINETIS_INFO))
# ROM size, KiB
export KINETIS_ROMSIZE := $(word 7, $(KINETIS_INFO))
# some special cases for ROM size, refactor when there are too many special cases
ifeq ($(KINETIS_ROMSIZE), 1M0)
  export KINETIS_ROMSIZE = 1024
else ifeq ($(KINETIS_ROMSIZE), 1M2)
  export KINETIS_ROMSIZE = 1280
else ifeq ($(KINETIS_ROMSIZE), 2M0)
  export KINETIS_ROMSIZE = 2048
endif
# Temperature range, C = -40--+85, V = -40--+105
export KINETIS_TEMPRANGE := $(word 8, $(KINETIS_INFO))
# Chip package code
export KINETIS_PACKAGE := $(word 9, $(KINETIS_INFO))
# Maximum speed, MHz/10
export KINETIS_SPEED := $(word 10, $(KINETIS_INFO))

ifeq ($(KINETIS_CORE), Z)
  # Cortex-M0+
  export CPU_ARCH = cortex-m0plus
  export CPU_FAMILY = klx
else ifeq ($(KINETIS_CORE), D)
  # Cortex-M4
  export CPU_ARCH = cortex-m4
  export CPU_FAMILY = kx
else ifeq ($(KINETIS_CORE), F)
  # Cortex-M4F or Cortex-M7
  # TODO: Add floating point support
  export CPU_ARCH = cortex-m4
  export CPU_FAMILY = kx
endif

# RAM sizes are a bit arbitrary, but are usually dependent on ROM size and core speed.
# KINETIS_RAMSIZE is used inside a $(( )) shell arithmetic block, so it is OK to
# use mathematical expressions here.
# This block is not complete, but it does cover all the CPUs supported by RIOT, and then some.
ifeq ($(KINETIS_SERIES),K)
  # A majority of the models have this ratio of SRAM_L vs total SRAM size
  KINETIS_SRAM_L_SIZE = $(KINETIS_RAMSIZE)/2
  ifeq ($(KINETIS_ROMSIZE),2048)
    ifeq ($(firstword $(sort $(KINETIS_SUBFAMILY) 7)),7)
      # subfamily >= 7
      # OCRAM in K27, K28 is not mapped here.
      KINETIS_RAMSIZE = 512
    else
      KINETIS_RAMSIZE = 256
    endif
  else ifeq ($(KINETIS_ROMSIZE),1024)
    ifeq ($(firstword $(sort $(KINETIS_SUBFAMILY) 3)),3)
      # subfamily >= 3
      KINETIS_RAMSIZE = 256
    else
      KINETIS_RAMSIZE = 128
    endif
  else ifeq ($(KINETIS_ROMSIZE),512)
    ifeq ($(KINETIS_SPEED),5)
      KINETIS_RAMSIZE = 64
    else ifeq ($(KINETIS_SUBFAMILY),4)
      KINETIS_RAMSIZE = 192
    else
      KINETIS_RAMSIZE = 128
    endif
  else ifeq ($(KINETIS_ROMSIZE),256)
    ifeq ($(KINETIS_SPEED),5)
      KINETIS_RAMSIZE = 32
    else ifeq ($(KINETIS_SPEED),15)
      KINETIS_RAMSIZE = 256
    else ifeq ($(KINETIS_FAMILY)$(KINETIS_SUBFAMILY),22)
      KINETIS_RAMSIZE = 48
    else ifeq ($(KINETIS_FAMILY)$(KINETIS_SUBFAMILY),24)
      KINETIS_RAMSIZE = 256
    endif
  else ifeq ($(KINETIS_ROMSIZE),128)
    ifeq ($(KINETIS_FAMILY),0)
      KINETIS_RAMSIZE = 16
    else ifeq ($(KINETIS_SUBFAMILY)$(KINETIS_SPEED),05)
      KINETIS_RAMSIZE = 16
    else ifeq ($(KINETIS_CORE),F)
      KINETIS_RAMSIZE = 24
      KINETIS_SRAM_L_SIZE = 8
    else
      KINETIS_RAMSIZE = 32
    endif
  else ifeq ($(KINETIS_ROMSIZE),64)
    KINETIS_RAMSIZE = 16
  endif
  ifneq (,$(filter $(KINETIS_RAMSIZE),192 256))
    # There seems to be a cap on SRAM_L at 64 kB across the whole K series
    KINETIS_SRAM_L_SIZE = 64
  endif
else ifeq ($(KINETIS_SERIES),W)
  KINETIS_RAMSIZE = $(KINETIS_ROMSIZE)/8
  ifeq ($(KINETIS_CORE),D)
    KINETIS_SRAM_L_SIZE = $(KINETIS_RAMSIZE)/2
  else ifeq ($(KINETIS_FAMILY),0)
    # 01Z
    KINETIS_SRAM_L_SIZE = 4
  else ifeq ($(KINETIS_SUBFAMILY),0)
    # 20Z, 30Z, 40Z
    KINETIS_SRAM_L_SIZE = 4
  else ifeq ($(KINETIS_SUBFAMILY),1)
    # 21Z, 31Z, 41Z
    KINETIS_RAMSIZE = $(KINETIS_ROMSIZE)/4
    KINETIS_SRAM_L_SIZE = $(KINETIS_RAMSIZE)/4
  else
    # TODO: KW35/36
    $(error Unknown Kinetis W)
  endif
endif
export KINETIS_RAMSIZE
export KINETIS_SRAM_L_SIZE

# Some debug output to use in case things are not being correctly detected
ifeq ($(KINETIS_INFO_DEBUG),1)
  $(info Kinetis CPU info: $(KINETIS_INFO))
  $(info Core:      $(KINETIS_CORE))
  $(info Series:    $(KINETIS_SERIES))
  $(info Family:    $(KINETIS_FAMILY))
  $(info Subfamily: $(KINETIS_SUBFAMILY))
  $(info ROM size:  $(KINETIS_ROMSIZE))
  $(info RAM size:  $(shell printf '%s' $$(($(KINETIS_RAMSIZE)))))
  $(info SRAM_L:    $(shell printf '%s' $$(($(KINETIS_SRAM_L_SIZE)))))
endif

ifeq (,$(KINETIS_RAMSIZE))
  $(error Unknown Kinetis RAM size, update kinetis-info.mk with your CPU)
endif
