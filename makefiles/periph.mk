MODULE ?= periph

# enable submodules
SUBMODULES := 1

# don't fail if requested submodule does not have corresponding *.c file
SUBMODULES_NOFORCE := 1

# in case of e.g., <cpu>_periph_common, use periph_* as submodule prefix
BASE_MODULE := periph

# periph_common matches the submodule selection, but shouldn't. So filter out here.
USEMODULE := $(filter-out periph_common, $(USEMODULE))

include $(RIOTBASE)/Makefile.base
