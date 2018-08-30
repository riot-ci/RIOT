# staging.mk
# this makefile is being included by staging/Makefile.include and staging/Makefile.dep.

#
# list of staging modules and applications
#
STAGING_MODULES += test
STAGING_APPS += test/hello-staging
#BROKEN += test

#
# housekeeping
#
STAGING_MODULES := $(sort $(STAGING_MODULES))
STAGING_MODULES_USED := $(filter $(USEMODULE),$(STAGING_MODULES))

BROKEN := $(sort $(BROKEN))

# error out on dependencies to broken staging module
STAGING_MODULES_USED_BROKEN := \
  $(sort $(filter $(BROKEN), $(STAGING_MODULES_USED)))

ifneq (, $(STAGING_MODULES_USED_BROKEN))
  FEATURES_REQUIRED += $(patsubst %,broken-staging-module-%,$(STAGING_MODULES_USED_BROKEN))
endif

# list of folders with staging modules
STAGING_DIRS := $(sort $(wildcard $(STAGING_MODULES_USED:%=$(RIOTBASE)/staging/%)))

# create app make file list to be consumed by makefiles/app_dirs.inc.mk
STAGING_APP_MAKEFILES := \
    $(sort \
        $(wildcard \
            $(patsubst %,$(RIOTBASE)/staging/%/Makefile,$(STAGING_APPS)) \
    ))
