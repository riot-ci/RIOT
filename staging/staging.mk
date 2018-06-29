# list of staging modules
STAGING_MODULES += test

# housekeeping
BROKEN := $(sort $(BROKEN))
STAGING_MODULES := $(sort $(STAGING_MODULES))
STAGING_MODULES_USED := $(filter $(USEMODULE),$(STAGING_MODULES))

# error out on dependencies to broken staging module
STAGING_MODULES_USED_BROKEN := \
  $(filter $(BROKEN), $(STAGING_MODULES_USED))

ifneq (, $(STAGING_MODULES_USED_BROKEN))
  $(error using broken staging modules: $(STAGING_MODULES_USED_BROKEN))
endif

# list of folders with staging modules
STAGING_DIRS := $(sort $(wildcard $(STAGING_MODULES_USED:%=$(RIOTBASE)/staging/%)))
