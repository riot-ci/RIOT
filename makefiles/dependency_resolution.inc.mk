# Perform a recursive dependency resolution: Include $(RIOTBASE)/Makefile.dep
# until no new modules and pkgs are pull in order to catch all transient
# dependencies

OLD_USEMODULE := $(sort $(USEMODULE))
OLD_USEPKG:= $(sort $(USEPKG))

# pull in dependencies of the currently used modules and pkgs
include $(RIOTBASE)/Makefile.dep

# check if required features are provided and update $(FEATURES_USED)
include $(RIOTMAKE)/features_check.inc.mk

# translate used features into used module, where needed
include $(RIOTMAKE)/features_modules.inc.mk

# sort and de-duplicate used modules and pkgs
USEMODULE := $(sort $(USEMODULE))
USEPKG := $(sort $(USEPKG))

# If set of used modules and pkgs has changed during last run, run again to
# recursively catch transitive dependencies
ifneq ($(OLD_USEMODULE) $(OLD_USEPKG),$(USEMODULE) $(USEPKG))
  include $(RIOTMAKE)/dependency_resolution.inc.mk
endif

# add default modules again, as $(DEFAULT_MODULE) might have been extended
# during dependency processing
USEMODULE += $(filter-out $(DISABLE_MODULE),$(DEFAULT_MODULE))

# Sort and de-duplicate used modules and default modules for readability
USEMODULE := $(sort $(USEMODULE))
DEFAULT_MODULE := $(sort $(DEFAULT_MODULE))
