# Add modules implementing used features
#
# This is done after the regular dependency resolution in Makefile.dep, as
# feature resolution depends on the used modules. As these modules however have
# no dependencies(except for periph_common), no dependency resolution is needed.

PERIPH_FEATURES := $(filter periph_%,$(FEATURES_USED))
# all periph features correspond to a periph submodule
# FEATURES_USED is defined in Makefile.features
USEMODULE += $(PERIPH_FEATURES)

# select cpu_check_address pseudomodule if the corresponding feature is used
USEMODULE += $(filter cpu_check_address, $(FEATURES_USED))

# include periph_common if any periph_* driver is used
ifneq (,$(filter periph_%, $(USEMODULE)))
  USEMODULE += periph_common
endif

# set all USED periph_% init modules as DEFAULT_MODULE
ifneq (,$(filter periph_init, $(USEMODULE)))
    # To enable adding periph_% modules through the environment we cant used
    # PERIPH_FEATURES since the MODULE might be have added directly as USEMODULE
    PERIPH_MODULES_NO_INIT = periph_init% periph_common
    PERIPH_MODULES = $(filter periph_%,$(USEMODULE))
    PERIPH_INIT_MODULES = $(subst periph_,periph_init_,\
      $(filter-out $(PERIPH_MODULES_NO_INIT),$(PERIPH_MODULES)))
    DEFAULT_MODULE += $(PERIPH_INIT_MODULES)
endif

# add periph_init_% modules to USEMODULE unless disabled
ifneq (,$(filter periph_init, $(USEMODULE)))
  USEMODULE += $(filter $(PERIPH_INIT_MODULES),\
    $(filter-out $(DISABLE_MODULE),$(DEFAULT_MODULE)))
endif
