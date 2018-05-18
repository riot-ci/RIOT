#
STAGING_MODULES += test

#
BROKEN := $(sort $(BROKEN))
STAGING_MODULES := $(sort $(filter-out $(BROKEN), $(STAGING_MODULES)))
STAGING_MODULES_USED := $(filter $(USEMODULE),$(STAGING_MODULES))

# list of folders with staging modules
STAGING_DIRS := $(sort $(wildcard $(STAGING_MODULES_USED:%=$(RIOTBASE)/staging/%)))
